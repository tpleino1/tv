#ifndef MB_PAUSESTATE_HH
#define MB_PAUSESTATE_HH

#include <array>

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

// Game Paused -menu
class PauseState : public State
{
	public:
		PauseState(StateStack &s, State::Resources r);
		PauseState(const PauseState& s) = delete;
		PauseState& operator= (const PauseState& s) = delete;
		virtual ~PauseState();

		// Application loop related functions
		virtual void draw();
		virtual bool update(const sf::Time& dt);
		virtual bool handleEvent(const sf::Event& e);

		// PauseState is somewhat transparent
		virtual bool isFullScreen() const;

	private:
		sf::Text 				title;
		sf::Sprite 				background;
		std::array<Button, 3> 	buttons;
		ButtonGroup 			group;
};

#endif // MB_PAUSESTATE_HH
