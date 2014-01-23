#ifndef MB_APPLICATION_HH
#define MB_APPLICATION_HH

#include <string>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>

#include "SimpleIni.hh"		// CSimpleIniA
#include "StateStack.hh"
#include "StatMeter.hh"
#include "MessageQueue.hh"
#include "ResourceId.hh"	// __Container
#include "MusicPlayer.hh"
#include "SoundPlayer.hh"
#include "Session.hh"



// A class to represent the whole application consisting
// of window, resources, events and main application loop.
class Application
{
	public:
		Application();
		// Copy and assignment constructors are disabled since
		// there should be only one Application instance.
		Application(const Application& a) = delete;
		Application& operator= (const Application& s) = delete;
		~Application();

		// Run the application after initialization
		void run();

	private:
		// One-time initialization functions
		void createStates();
		void loadResources();

		// Main application loop functions
		void handleEvents();
		void update(const sf::Time& dt);
		void render();

		void screenshot(); 	// Take a screenshot

		// Static consts for configuration filenames
		static const std::string currentConfigFile;
		static const std::string defaultConfigFile;

		// Resource and resource-like variables
		CSimpleIniA 			config;			// Runtime configuration
		sf::RenderWindow 		window;			// Window
		sf::Time 				tick;			// One time step in application loop
		FontContainer 			fonts;			// Fonts
		TextureContainer 		textures;		// Texture files, one containing multiple subtextures
		TextureRectContainer 	textureRects;	// Slicing to get subtextures
		ColorContainer			colors;			// Colors
		MusicPlayer				music;			// Music (SFML implementation is faulty)
		SoundPlayer				sound;			// Sound effects (SFML implementation is faulty)
		KeyBindings::Container 	keyBinds;		// Controls
		Session					game;			// Class to convey game session data between states

		// Overlay related variables
		bool					drawFPS;		// Show statistics (mainly FPS)
		StatMeter				fps;			// Statistics (top left)
		MessageQueue			msgs;			// Overlay messages (bottom right)

		// Application is divided into States which are stored in a stack-like ADT
		// There are states for menus and for the actual game
		StateStack 				states;
};

#endif // MB_APPLICATION_HH
