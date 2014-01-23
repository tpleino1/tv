#ifndef MB_SETUPSTATE_HH
#define MB_SETUPSTATE_HH

#include <map>
#include <functional>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "State.hh"
#include "Button.hh"
#include "ButtonGroup.hh"

namespace sf
{
	class Time;
	class Event;
}

class StateStack;

// The State one uses to create a new game
class SetupState : public State
{
	public:
		SetupState(StateStack &s, State::Resources r);
		// Copy and assignment constructors are disabled since
		// there should be only one MenuState instance at any one time,
		// created by the creator function in StateStack.
		SetupState(const SetupState& s) = delete;
		SetupState& operator= (const SetupState& s) = delete;
		virtual ~SetupState();

		// Application loop related functions
		virtual void draw();
		virtual bool update(const sf::Time& dt);
		virtual bool handleEvent(const sf::Event& e);

	private:

		// enum for all elements in the state
		enum Elements
		{
			Title,
			Players,
			GameMode,
			Map,
			Monsters,
			Money,
			Treasures,
			Fog,
			Rounds,
			Length,
			Continue,
			Cancel
		};

		// Initialization function
		void createCallbacks();

		// Generic callback to be bound
		void changeSetting(const std::string& s, sf::Text* t, std::function<std::string(unsigned int)> f);

		// GUI elements
		sf::Sprite 							background;
		std::map<Elements, sf::Text> 		labelMap;
		std::map<Elements, Button> 			buttonMap;
		ButtonGroup 						group;
};

#endif // MB_SETUPSTATE_HH
