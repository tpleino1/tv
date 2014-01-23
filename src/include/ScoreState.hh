#ifndef MB_SCORESTATE_HH
#define MB_SCORESTATE_HH

#include <map>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "State.hh"
#include "Button.hh"
#include "ButtonGroup.hh"
#include "Round.hh"

namespace sf
{
	class Time;
	class Event;
}

class StateStack;

// The Game Over screen displayed after a game session
class ScoreState : public State
{
	public:
		ScoreState(StateStack &s, State::Resources r);
		// Copy and assignment constructors are disabled since
		// there should be only one ScoreState instance at any one time,
		// created by the creator function in StateStack.
		ScoreState(const ScoreState& s) = delete;
		ScoreState& operator= (const ScoreState& s) = delete;
		virtual ~ScoreState();

		// Application loop related functions
		virtual void draw();
		virtual bool update(const sf::Time& dt);
		virtual bool handleEvent(const sf::Event& e);

	private:
		static const unsigned int column;

		sf::Sprite 							background;
		sf::Text 							title;
		std::map<unsigned int, sf::Text>	labelMap;
		std::map<unsigned int, Button>		buttonMap;
		ButtonGroup							group;
};

#endif // MB_SCORESTATE_HH
