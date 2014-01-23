#ifndef MB_ENVIRONMENTCONTROLLER_HH
#define MB_ENVIRONMENTCONTROLLER_HH

#include "gameController.hh"

class EnvironmentController : public GameController
{

	private:

		float dtSum;

		void damageTerrain(GameEvent event);

		void revealTerrain(GameEvent event);

	public:

		EnvironmentController(GameState* gs) : GameController(gs), dtSum(0) {}

		virtual void update(float dt);

		virtual void invokeGameEvent(GameEvent event);

		virtual ~EnvironmentController() { }

};

#endif
