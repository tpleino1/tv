#ifndef MB_OPTIONSSTATE_HH
#define MB_OPTIONSSTATE_HH

#include <map>

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

// Options: Controls and Volume
class OptionsState : public State
{
	public:
		OptionsState(StateStack &s, State::Resources r);
		// Copy and assignment constructors are disabled since
		// there should be only one OptionsState instance at any one time,
		// created by the creator function in StateStack.
		OptionsState(const OptionsState& s) = delete;
		OptionsState& operator= (const OptionsState& s) = delete;
		virtual ~OptionsState();

		// Application loop related functions
		virtual void draw();
		virtual bool update(const sf::Time& dt);
		virtual bool handleEvent(const sf::Event& e);

	private:
		// Update labels after a binding has been modified
		void updateLabels();
		void createCallbacks();

		// enum for all the buttons in the options
		// ("type" and order numbering)

		//Player___ Buttons are associated with the binding function
		static std::map<unsigned int, KeyBindings::Action> keyButtons;
		static const unsigned int column;

		const unsigned int players;
		sf::Sprite 							background;
		sf::Text 							title;
		std::map<unsigned int, sf::Text> 	labelMap;
		std::map<unsigned int, Button> 		buttonMap;
		KeyBindings::Container				localBinds; // Local copy of the key bindings
		ButtonGroup 						group;

		unsigned short 						soundVol;
		unsigned short						musicVol;
};

#endif // MB_OPTIONSSTATE_HH
