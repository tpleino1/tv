#include "OptionsState.hh"

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Vector2.hpp>

#include "Utilities.hh"

namespace
{
	enum Elements
	{
		SoundVolume = 0,
		MusicVolume = 1,
		KeyPlayer 	= 2,
		MoveUp		= 3,
		MoveDown	= 4,
		MoveLeft	= 5,
		MoveRight	= 6,
		UseFirst	= 7,
		UseSecond	= 8,
		Save		= 9,
		Defaults	= 10,
		MainMenu	= 11
	};
}


// Buttons are associated with a binding function, mapping to actions
std::map<unsigned int, KeyBindings::Action> OptionsState::keyButtons({
		{ MoveUp, 		KeyBindings::MoveUp },
		{ MoveDown,		KeyBindings::MoveDown },
		{ MoveLeft,		KeyBindings::MoveLeft },
		{ MoveRight,	KeyBindings::MoveRight },
		{ UseFirst, 	KeyBindings::UseFirst },
		{ UseSecond,	KeyBindings::UseSecond }
	});

const unsigned int OptionsState::column = 5;

//Constructor
OptionsState::OptionsState(StateStack& s, State::Resources r) :
		State(s, r),
		players((r.game)->getSettings().getOptions("players").top),
		background(),
		title(),
		labelMap(),
		buttonMap(),
		localBinds(),
		group((*(r.keyBinds))[0]),
		soundVol(100),
		musicVol(100)
{
	// Config values (maps should be const but the compiler does not like const)
	const sf::Vector2f 	cfg_screenSize 	= fromString<sf::Vector2f>(r.config->GetValue("Video", 	"size"));
	const sf::Vector2f 	cfg_titlePos 	= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"titlePos"));
	const sf::Vector2f 	cfg_menuPos 	= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"menuPos"));
	const sf::Vector2f 	cfg_leftMargin 	= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"leftMargin"));
	const sf::Vector2f 	cfg_midMargin 	= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"midMargin"));
	const sf::Vector2f 	cfg_margin 		= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"margin"));
	const unsigned int 	cfg_charSize 	= fromString<unsigned int>(r.config->GetValue("Menu", 	"charSize"));
	const sf::Font& 	cfg_font 		= (r.fonts)->get("default");
	const sf::Color& 	cfg_color 		= (r.colors)->get("default");

	// Label positions
	std::map<unsigned int, sf::Vector2f> cfg_elemPos({
			{ SoundVolume,		sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ 13.5f * cfg_margin },
			{ MusicVolume, 		sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ 12.5f * cfg_margin },
			{ KeyPlayer, 		sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ 10.5f * cfg_margin},
			{ MoveUp, 			sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ 9.f * cfg_margin },
			{ MoveDown, 		sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ 8.f * cfg_margin },
			{ MoveLeft, 		sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ 7.f * cfg_margin },
			{ MoveRight, 		sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ 6.f * cfg_margin },
			{ UseFirst, 		sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ 5.f * cfg_margin },
			{ UseSecond, 		sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ 4.f * cfg_margin },
			{ Save, 			sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ 2.f * cfg_margin },
			{ Defaults, 		sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ 1.f * cfg_margin },
			{ MainMenu, 		sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ 0.f * cfg_margin }
		});

	// Copy existing key bindings to local copies
	for (unsigned int i = 1; i <= players; i++)
		localBinds[i] = (*(r.keyBinds))[i];

	soundVol = fromString<unsigned int>(r.config->GetValue("Volume", "sound"));
	musicVol = fromString<unsigned int>(r.config->GetValue("Volume", "music"));

	// Background
	background.setTexture((r.textures)->get("background"));

	// Title
	title = sf::Text("OPTIONS", cfg_font, cfg_charSize);
	changeAnchor(title, Anchor::BottomRight);
	title.setPosition(sf::Vector2f(cfg_screenSize.x, 0.f) + cfg_titlePos);
	title.setColor(cfg_color);

	// "Static" labels
	labelMap[KeyPlayer] 	= sf::Text("BINDING", 			cfg_font, cfg_charSize);
	labelMap[MoveUp] 		= sf::Text("MOVE UP", 			cfg_font, cfg_charSize);
	labelMap[MoveDown] 		= sf::Text("MOVE DOWN", 		cfg_font, cfg_charSize);
	labelMap[MoveLeft] 		= sf::Text("MOVE LEFT", 		cfg_font, cfg_charSize);
	labelMap[MoveRight] 	= sf::Text("MOVE RIGHT", 		cfg_font, cfg_charSize);
	labelMap[UseFirst] 		= sf::Text("PRIMARY", 			cfg_font, cfg_charSize);
	labelMap[UseSecond] 	= sf::Text("SECONDARY", 		cfg_font, cfg_charSize);

	for (auto& l : labelMap)
	{
		changeAnchor(l.second, Anchor::BottomLeft);
		l.second.setPosition(cfg_elemPos[l.first]);
	}

	labelMap[SoundVolume] 	= sf::Text(toString(soundVol), 	cfg_font, cfg_charSize);
	changeAnchor(labelMap[SoundVolume], Anchor::BottomRight);
	labelMap[SoundVolume].setPosition(cfg_elemPos[SoundVolume] + cfg_leftMargin);

	labelMap[MusicVolume] 	= sf::Text(toString(musicVol), 	cfg_font, cfg_charSize);
	changeAnchor(labelMap[MusicVolume], Anchor::BottomRight);
	labelMap[MusicVolume].setPosition(cfg_elemPos[MusicVolume] + cfg_leftMargin);

	// "Static" buttons
	buttonMap[SoundVolume] 	= Button("SOUND VOLUME", 	cfg_font, *(r.colors));
	buttonMap[MusicVolume] 	= Button("MUSIC VOLUME", 	cfg_font, *(r.colors));
	buttonMap[Save] 		= Button("SAVE", 			cfg_font, *(r.colors));
	buttonMap[Defaults] 	= Button("DEFAULTS", 		cfg_font, *(r.colors));
	buttonMap[MainMenu] 	= Button("MAIN MENU", 		cfg_font, *(r.colors));

	for (auto& b : buttonMap)
	{
		b.second.setCharacterSize(cfg_charSize);
		changeAnchor(b.second, Anchor::BottomLeft);
		b.second.setPosition(cfg_elemPos[b.first]);
	}

	group.add(buttonMap[SoundVolume]);
	group.add(buttonMap[MusicVolume]);

	// Player related labels & Buttons
	for (unsigned int i = 0; i < players; i++)
	{
		unsigned int col = (i+1) << column;
		sf::Vector2f posMargin = cfg_leftMargin + (float)i * cfg_midMargin;

		// Label
		labelMap[KeyPlayer + col] = sf::Text("PLAYER " + toString(i+1), cfg_font, cfg_charSize);
		changeAnchor(labelMap[KeyPlayer + col], Anchor::Bottom);
		labelMap[KeyPlayer + col].setPosition(cfg_elemPos[KeyPlayer] + posMargin);

		// Buttons
		for (unsigned int j = MoveUp; j <= UseSecond; j++)
		{
			buttonMap[j + col] = Button(toString(localBinds[i+1].getKey(keyButtons[j])), cfg_font, *(r.colors));
			buttonMap[j + col].setCharacterSize(cfg_charSize);
			changeAnchor(buttonMap[j + col], Anchor::Bottom);
			buttonMap[j + col].setPosition(cfg_elemPos[j] + posMargin);
			buttonMap[j + col].setToggle(true);
			group.add(buttonMap[j + col]);
		}
	}

	group.add(buttonMap[Save]);
	group.add(buttonMap[Defaults]);
	group.add(buttonMap[MainMenu]);

	// Label Color
	for (auto& l : labelMap)
		l.second.setColor(cfg_color);

	// Buttons: Callbacks
	createCallbacks();

	// Play background music
	(r.music)->play("menu");
}

// Destructor
OptionsState::~OptionsState()
{ }

// Render: BG, labels, buttons
void OptionsState::draw()
{
	sf::RenderWindow& w = *(getResources().window);

	w.draw(background);
	w.draw(title);
	for(auto& l : labelMap)
		w.draw(l.second);
	for(auto& b : buttonMap)
		w.draw(b.second);
}

// Nothing to update
bool OptionsState::update(const sf::Time&)
{
	return false; // Stop updating here
}

// Events are for key binding and moving about in the options
bool OptionsState::handleEvent(const sf::Event& e)
{
	// Disable F12 for this screen (takes a screenshot of the whole Application)
	if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::F12))
		return false;

	bool waitForKey = false;	// Are we binding a key atm
	const unsigned int mask = (1 << column) - 1;

	for (auto& b : buttonMap) // For each binding button
	{
		auto iter = keyButtons.find(b.first & mask);
		if (iter == keyButtons.end() || !b.second.isActive())
			continue;

		waitForKey = true;
		if (e.type == sf::Event::KeyPressed)
		{
			if (e.key.code != sf::Keyboard::Escape)	// Escape cancels the binding process
			{
				for (auto& k : localBinds)
					k.second.clearKey(e.key.code);

				localBinds[(b.first & ~mask) >> column].setKey(iter->second, e.key.code);
			}
			b.second.deactivate();
		}
		break;
	}

	if (waitForKey) // We changed a key
		updateLabels();
	else if ((e.type == sf::Event::KeyPressed) &&
			(e.key.code == sf::Keyboard::Escape)) // Escape to last "back" button when not binding
		group.selectEnd(true);
	else
		group.handleEvent(e);

	return false; // Stop event handling here
}

// Refresh button labels when a change is made
// (it would not be enough to change only the modified key)
void OptionsState::updateLabels()
{
	const unsigned int mask = (1 << column) - 1;

	// Algorithm: reset anchor, edit button (may change its dimensions) and set anchor
	for (auto& b : buttonMap)
	{
		auto iter = keyButtons.find(b.first & mask);
		if (iter == keyButtons.end())
			continue;

		std::string temp = toString(localBinds[(b.first & ~mask) >> column].getKey(iter->second));
		if (temp != std::string(b.second.getString()))
		{
			changeAnchor(b.second, Anchor::BottomLeft, 	Anchor::Bottom);
			b.second.setString(temp);
			changeAnchor(b.second, Anchor::Bottom, 		Anchor::BottomLeft);
		}
	}
}

// Create callbacks
void OptionsState::createCallbacks()
{
	// Sound Volume (0 to 100 in steps of 10)
	buttonMap[SoundVolume].setCallback([this] ()
	{
		changeAnchor(labelMap[SoundVolume], Anchor::BottomLeft, Anchor::BottomRight);
		soundVol = (soundVol + 10) % 110;
		labelMap[SoundVolume].setString(soundVol == 0 ? "MUTED" : toString(soundVol));
		changeAnchor(labelMap[SoundVolume], Anchor::BottomRight, Anchor::BottomLeft);
	});

	// Music Volume (0 to 100 in steps of 10)
	buttonMap[MusicVolume].setCallback([this] ()
	{
		changeAnchor(labelMap[MusicVolume], Anchor::BottomLeft, Anchor::BottomRight);
		musicVol = (musicVol + 10) % 110;
		labelMap[MusicVolume].setString(musicVol == 0 ? "MUTED" : toString(musicVol));
		changeAnchor(labelMap[MusicVolume], Anchor::BottomRight, Anchor::BottomLeft);
	});

	// Save changes
	buttonMap[Save].setCallback([this] ()
	{
		State::Resources r = getResources();

		// Modify current config
		r.config->SetValue("Volume", "sound", toString(soundVol).c_str());
		r.config->SetValue("Volume", "music", toString(musicVol).c_str());
		for (unsigned int i = 1; i <= players; i++)
			(*(r.keyBinds))[i] = localBinds[i];
		saveKeyBindings(localBinds, *(r.config));

		// Set Volumes
		(r.sound)->setVolume(soundVol);
		(r.music)->setVolume(musicVol);

		// Save current config to disk
		const std::string savePath = r.config->GetValue("Config", "current");
		r.config->SaveFile(savePath.c_str());
	});

	// Restore defaults
	buttonMap[Defaults].setCallback([this] ()
	{
		State::Resources r = getResources();

		// Load defaults into config
		const std::string loadPath = r.config->GetValue("Config", "default");
		r.config->LoadFile(loadPath.c_str());

		// Default values
		loadKeyBindings(localBinds, *(r.config));
		soundVol = fromString<unsigned int>(r.config->GetValue("Volume", "sound"));
		musicVol = fromString<unsigned int>(r.config->GetValue("Volume", "music"));

		// Set Volumes
		(r.sound)->setVolume(soundVol);
		(r.music)->setVolume(musicVol);

		// Change volume labels (reset anchor, change text)
		changeAnchor(labelMap[SoundVolume], Anchor::BottomLeft, Anchor::BottomRight);
		changeAnchor(labelMap[MusicVolume], Anchor::BottomLeft, Anchor::BottomRight);

		labelMap[SoundVolume].setString(soundVol == 0 ? "MUTED" : toString(soundVol));
		labelMap[MusicVolume].setString(musicVol == 0 ? "MUTED" : toString(musicVol));

		changeAnchor(labelMap[SoundVolume], Anchor::BottomRight, Anchor::BottomLeft);
		changeAnchor(labelMap[MusicVolume], Anchor::BottomRight, Anchor::BottomLeft);

		updateLabels();	// Refresh Control labels
	});

	// Main Menu
	buttonMap[MainMenu].setCallback([this] () { deferedPop(); });

	// Key binds are not handled with callbacks
}
