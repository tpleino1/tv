#ifndef MB_STATE_HH
#define MB_STATE_HH

#include <memory> // unique_ptr

#include <SFML/Graphics/RenderWindow.hpp>

#include "StateId.hh"		// State::ID
#include "ResourceId.hh"	// Resource typedefs
#include "SimpleIni.hh"		// CSimpleIniA
#include "KeyBindings.hh"
#include "MusicPlayer.hh"
#include "SoundPlayer.hh"
#include "MessageQueue.hh"
#include "Session.hh"

namespace sf
{
	class Time;
	class Event;
}

// State is included in StateStack.hh
// -> forward declaration needed
class StateStack;

// Abstract State class to be inherited by actual states
class State
{
	public:
		// We need pointers for polymorphism,
		// and states are unique in their nature
		typedef std::unique_ptr<State> Pointer;

		// A subclass to distribute application wide resources between different states
		class Resources
		{
		public:
			Resources(sf::RenderWindow& w,
					const FontContainer& f,
					const TextureContainer& t,
					const TextureRectContainer& tr,
					const ColorContainer& c,
					CSimpleIniA& i,
					KeyBindings::Container& k,
					MusicPlayer& m,
					SoundPlayer& s,
					MessageQueue& o,
					Session& g);

			sf::RenderWindow* 			window;			// or sf::RenderWindow* const 			window; 		// Modified when using
			const FontContainer* 		fonts;			// or const FontContainer* const 		fonts;
			const TextureContainer* 	textures;		// or const TextureContainer* const 	textures;
			const TextureRectContainer* textureRects;	// or const TextureRectContainer* const textureRects;
			const ColorContainer* 		colors;			// or const ColorContainer* const 		colors;
			CSimpleIniA* 				config;			// or CSimpleIniA* const 				config; 		// Modified by OptionsState
			KeyBindings::Container* 	keyBinds;		// or KeyBindings::Container* const 	keyBinds; 		// Modified by OptionsState
			MusicPlayer* 				music;			// or MusicPlayer* const 				music; 			// Modified when using
			SoundPlayer* 				sound;			// or SoundPlayer* const 				sound; 			// Modified when using
			MessageQueue* 				overlay;		// or MessageQueue* const 				overlay; 		// Modified when using
			Session* 					game;			// or Session* const 					game;			// Updated as game progresses
		};

		State(StateStack& s, Resources r);
		// Copy and assignment constructors are disabled due to the nature of the State.
		State(const State& s) = delete;
		State& operator= (const State& s) = delete;
		virtual ~State();

		// Pure virtual functions to be implemented in inherited classes

		// Iterated from bottom -> top
		// isFullScreen returns tells whether the layer covers to whole screen
		// That is, TRUE = LAYERS BELOW WON'T BE DRAWN, FALSE = DRAW ALSO THOSE BELOW
		virtual void draw() = 0;
		virtual bool isFullScreen() const; // defaults to true

		// Iterated from top -> bottom
		// Return values tells whether we need to traverse the stack further down
		// That is, TRUE = CONTINUE, FALSE = STOP HERE
		virtual bool update(const sf::Time& dt) = 0;
		virtual bool handleEvent(const sf::Event& e) = 0;

		Resources getResources() const;


	protected:
		// StateStack operations for inheriting classes
		void deferedPush(States::ID id);
		void deferedPop();
		void deferedClear();

	private:
		StateStack* states;
		Resources 	res;
};

#endif // MB_STATE_HH
