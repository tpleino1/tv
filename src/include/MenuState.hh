#ifndef MB_MENUSTATE_HH
#define MB_MENUSTATE_HH

#include <array>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "State.hh"
#include "Button.hh"
#include "ButtonGroup.hh"
#include "player.hh"
namespace sf
{
	class Time;
	class Event;
}

class StateStack;

// Main menu of the game
class MenuState : public State
{
	public:
		MenuState(StateStack &s, State::Resources r);
		// Copy and assignment constructors are disabled since
		// there should be only one MenuState instance at any one time,
		// created by the creator function in StateStack.
		MenuState(const MenuState& s) = delete;
		MenuState& operator= (const MenuState& s) = delete;
		virtual ~MenuState();

		// Application loop related functions
		virtual void draw();
		virtual bool update(const sf::Time& dt);
		virtual bool handleEvent(const sf::Event& e);

	private:
		void createCallbacks();

		sf::Text 				title;
		sf::Sprite 				background;
		std::array<Button, 5> 	buttons;
		ButtonGroup 			group;
};

#endif // MB_MENUSTATE_HH
