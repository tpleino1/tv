#ifndef MB_GAMESTATE_HH
#define MB_GAMESTATE_HH

#include <string>

#include "State.hh"
#include "PlayerHUD.hh"
#include "ClockHUD.hh"
#include "MessageQueue.hh"

#include "World.hh"

//#include "item.hh"
#include "level.hh"
#include "characterController.hh"
#include "weaponController.hh"
#include "environmentController.hh"

namespace sf
{
	class Time;
	class Event;
}

class StateStack;

// The actual game as a Application state
class GameState : public State
{
	public:
		// Constructor
		GameState(StateStack &s, State::Resources r);
		// Copy and assignment constructors are disabled since
		// there should be only one GameState instance at any one time,
		// created by the creator function in StateStack.
		GameState(const GameState& a) = delete;
		GameState& operator= (const GameState& s) = delete;
		virtual ~GameState();

		// Application loop related functions
		virtual void draw();
		virtual bool update(const sf::Time& dt);
		virtual bool handleEvent(const sf::Event& e);

		// Game Logic related functions
		void callGameEvent(GameEvent gameEvent);
		Level& getLevel();
		Tile& getTile(sf::Vector2i);
		Tile& getTile(Coordinate coords);

		// FOR DEBUGGING
		void showMessage(const std::string& message);

		unsigned getMonsterCount() const;

	private:
		enum GamePhase
		{
			Start,
			OnGoing,
			Finish
		};
	

		bool isRoundOver();

		static const sf::Time freezeTime;

		GamePhase 				phase;
		sf::Time				hourglass;

		// Game Mechanics
		WeaponController 		weaponController;
		EnvironmentController 	environmentController;
		CharacterController 	characterController;
		World 					world;
		bool					hasFoundExit;

		// HUD elements
		std::vector<PlayerHUD> 	hud;
		ClockHUD 				clk;
		MessageQueue 			msgs;
};

#endif // MB_GAMESTATE_HH
