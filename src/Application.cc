#include "Application.hh"

#include <iostream>
#include <ctime>		// for screenshot()
#include <stdexcept>	// throw, runtime_error

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector3.hpp>

#include "Utilities.hh"		// changeAnchor, fromString, toString
#include "State.hh"			// Resources
#include "StateId.hh"		// Available states

#include "GameState.hh"		// For creator initialization
#include "MenuState.hh"
#include "SetupState.hh"
#include "PauseState.hh"
#include "ShopState.hh"
#include "ScoreState.hh"
#include "OptionsState.hh"

// Configuration filenames
const std::string Application::currentConfigFile = "cfg/current.ini";
const std::string Application::defaultConfigFile = "cfg/default.ini";

// Constructor
Application::Application() :
		config(),
		window(),
		tick(sf::Time::Zero),
		fonts(),
		textures(),
		textureRects(),
		colors(),
		music(),
		sound(),
		keyBinds(),
		game(),
		drawFPS(false),
		fps(),
		msgs(),
		states(State::Resources(window, fonts, textures, textureRects, colors, config, keyBinds, music, sound, msgs, game))
{
	bool useDefaults = false; 	// Resort to default configuration

	// Load configuration from current (as Unicode)
	config.SetUnicode();
	if (config.LoadFile(currentConfigFile.c_str()) < 0)
	{
		// Current config not available, try defaults
		if (config.LoadFile(defaultConfigFile.c_str()) < 0)
			throw std::runtime_error("Configuration files could not be read.");
		useDefaults = true;
		config.SaveFile(currentConfigFile.c_str());	// Save defaults as current
	}

	// Window
	sf::Vector2u cfg_screenSize = fromString<sf::Vector2u>(config.GetValue("Video", "size"));
	window.create(sf::VideoMode(cfg_screenSize.x, cfg_screenSize.y),
			config.GetValue("General", "name"),
			sf::Style::Close);
	window.setFramerateLimit(fromString<unsigned int>(config.GetValue("Video", "maxForegroundFPS")));
	window.setVerticalSyncEnabled(fromString<bool>(config.GetValue("Video", "vSync")));
	window.setKeyRepeatEnabled(false);

	// Save to local variable since its read on every iteration
	drawFPS = fromString<bool>(config.GetValue("Video", "drawFPS"));

	// Tick (cannot use const since value is read from config)
	tick = sf::seconds(fromString<float>(config.GetValue("Game", "tick")));

	// Resources
	loadResources();

	// Application icon
	const sf::Image i = textures.get("icon").copyToImage();
	window.setIcon(i.getSize().x, i.getSize().y, i.getPixelsPtr());

	// StatMeter
	fps.setFont(fonts.get("default"));
	fps.setColor(colors.get("default"));

	// Overlay messages
	msgs.setFont(fonts.get("default"));
	msgs.setColor(colors.get("default"));
	msgs.setCharSize(fromString<unsigned int>(config.GetValue("Overlay", "charSize")));
	msgs.setAnchor(Anchor::BottomRight);
	msgs.setMargin(fromString<sf::Vector2f>(config.GetValue("Overlay", "margin")));
	msgs.setFadeTime(fromString<sf::Time>(config.GetValue("Overlay", "fadeTime")));
	msgs.setPosition(fromString<sf::Vector2f>(config.GetValue("Video", "size")) +
		fromString<sf::Vector2f>(config.GetValue("Overlay", "position")));

	// Add overlay message if needed
	if (useDefaults)
		msgs.addString(currentConfigFile + " could not be read. Using defaults.", sf::seconds(2.f));

	// States, start in main menu
	createStates();
	states.push(States::Menu);
}

// Destructor (No resources are programmer's responsibility)
Application::~Application()
{ }

// Application-level loop
void Application::run()
{
	sf::Clock clock;
	sf::Time counter = sf::Time::Zero;

	// Loop until window is closed
	// G
	while (window.isOpen())
	{
		sf::Time dt = clock.restart();
		counter += dt;
		// Constant-time game update
		while (counter > tick)
		{
			counter -= tick;
			handleEvents();
			update(tick);

			if (states.isEmpty())	// For "internal" quit
				window.close();
		}

		fps.update(dt);	// Calculate stats
		render();		// Draw on screen
	}
}

// Part of application loop: Deal with events in the buffer
void Application::handleEvents()
{
	sf::Event e;
	while (window.pollEvent(e))
	{
		// Pass to StateStack
		states.handleEvent(e);

		// Close window
		if (e.type == sf::Event::Closed)
			window.close();
		// Limit FPS when in background (and disable vertical sync)
		else if (e.type == sf::Event::LostFocus)
		{
			window.setVerticalSyncEnabled(false);
			window.setFramerateLimit(fromString<unsigned int>(config.GetValue("Video", "maxBackgroundFPS")));
		}
		// Restore foreground fps or vertical sync
		else if (e.type == sf::Event::GainedFocus)
		{
			window.setFramerateLimit(fromString<unsigned int>(config.GetValue("Video", "maxForegroundFPS")));
			window.setVerticalSyncEnabled(fromString<bool>(config.GetValue("Video", "vSync")));
		}
		// Take screenshot
		else if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::F12))
			screenshot();
	}
}

// Part of application loop, inner loop (dt = tick)
void Application::update(const sf::Time& dt)
{
	states.update(dt);
	msgs.update(dt);
}

// Part of application loop
void Application::render()
{
	window.clear();		// Clear buffer

	states.draw();		// Draw States, FPS and Messages
	if (drawFPS)
	{
		window.setView(window.getDefaultView());
		window.draw(fps);
	}
	window.draw(msgs);

	window.display();	// Switch buffers
}

// Initialization function to add create functions for later state creation
void Application::createStates()
{
	states.addCreator<MenuState>(States::Menu);
	states.addCreator<OptionsState>(States::Options);
	states.addCreator<SetupState>(States::Setup);
	states.addCreator<ShopState>(States::Shop);
	states.addCreator<GameState>(States::Game);
	states.addCreator<ScoreState>(States::Score);
	states.addCreator<PauseState>(States::Pause);
}

// Initialization function to construct resources
void Application::loadResources()
{
	// Resources
	std::string path;	// folder containing resources
	CSimpleIniA::TNamesDepend keys;	// resource identifiers

	// Fonts (not modified after initialization)
	path = config.GetValue("Resources", "fontPath");
	config.GetAllKeys("Fonts", keys);
	for (auto i = keys.begin(); i != keys.end(); ++i)
		fonts.load(std::string(i->pItem), path + std::string(config.GetValue("Fonts", i->pItem)));

	// Textures
	path = config.GetValue("Resources", "texturePath");
	config.GetAllKeys("Textures", keys);
	for (auto i = keys.begin(); i != keys.end(); ++i)
	{
		textures.load(std::string(i->pItem), path + std::string(config.GetValue("Textures", i->pItem)));
		textures.get(i->pItem).setSmooth(true);
	}

	// Texture Repeating (for backgrounds, not modified after this)
	config.GetAllKeys("TextureRepeat", keys);
	for (auto i = keys.begin(); i != keys.end(); ++i)
		textures.get(i->pItem).setRepeated(fromString<bool>(config.GetValue("TextureRepeat", i->pItem)));

	// TextureRects (not modified after initialization)
	config.GetAllKeys("TextureRects", keys);
	for (auto i = keys.begin(); i != keys.end(); ++i)
		textureRects.load(std::string(i->pItem), fromString<sf::IntRect>(config.GetValue("TextureRects", i->pItem)));

	// Colors (not modified after initialization)
	config.GetAllKeys("Colors", keys);
	for (auto i = keys.begin(); i != keys.end(); ++i)
		colors.load(std::string(i->pItem), fromString<sf::Color>(config.GetValue("Colors", i->pItem)));

	// Sound (modified later in the sense that we use play/pause/stop, or change volume)
	path = config.GetValue("Resources", "soundPath");
	config.GetAllKeys("Sound", keys);
	for (auto i = keys.begin(); i != keys.end(); ++i)
		sound.addSound(std::string(i->pItem), path + std::string(config.GetValue("Sound", i->pItem)));
	sound.setVolume(fromString<float>(config.GetValue("Volume", "sound")));

	// Music (modified later in the sense that we use play/pause/stop, or change volume)
	path = config.GetValue("Resources", "musicPath");
	config.GetAllKeys("Music", keys);
	for (auto i = keys.begin(); i != keys.end(); ++i)
		music.addTrack(std::string(i->pItem), path + std::string(config.GetValue("Music", i->pItem)));
	music.setVolume(fromString<float>(config.GetValue("Volume", "music")));

	// Key Bindings (may be modified in options)
	for (unsigned int i = 0; i <= 4; i++)
		keyBinds[i] = KeyBindings(i);
	loadKeyBindings(keyBinds, config);

	// Session::Settings
	Session::Settings& opt = game.getSettings();
	config.GetAllKeys("SetupDefaults", keys);
	for (auto i = keys.begin(); i != keys.end(); ++i)
		opt.setValue(i->pItem, fromString<sf::Vector3<unsigned int>>(config.GetValue("SetupDefaults", i->pItem)).x);

	config.GetAllKeys("SetupOptions", keys);
	for (auto i = keys.begin(); i != keys.end(); ++i)
		opt.setOptions(i->pItem, fromString<sf::Rect<unsigned int>>(config.GetValue("SetupOptions", i->pItem)));
}

// Take a screenshot
void Application::screenshot()
{
	// C-type initialization (we use ctime)
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[128];

	// File name depends on the current time
	// JPG-format does not seem to work here for some reason
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 128, config.GetValue("Screenshot", "format"), timeinfo);

	// Try to save, on success add overlay message
	std::string fn = config.GetValue("Screenshot", "path") + std::string(buffer);
	if (window.capture().saveToFile(fn))
		msgs.addString("Screenshot saved to " + fn + ".", sf::seconds(2.f));
}
