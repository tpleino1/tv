#ifndef MB_GAMECONTROLLER_HH
#define MB_GAMECONTROLLER_HH

#include <vector>

#include "gameEvent.hh"

class GameState;
class ShopState;

class GameController {

	protected:

		GameState* gameState;
		ShopState* shopState;

	public:

		GameController(GameState* gs) : gameState(gs), shopState(nullptr) { }
		GameController(ShopState* ss) : gameState(nullptr), shopState(ss) { }

		//void setGamestate(GameState* gs)
		virtual void update(float dt) = 0;
		virtual void invokeGameEvent(GameEvent event) = 0;

		virtual ~GameController() {}

};

#endif
