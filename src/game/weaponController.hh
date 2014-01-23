#ifndef MB_WEAPONCONTROLLER_HH
#define MB_WEAPONCONTROLLER_HH

#include <SFML/System.hpp>

#include "gameController.hh"
#include "weapon.hh"
#include "datatables.hh"


class GameState;
class GameEvent;


// WeaponController class to manage weapons and effects on the game field.
// Class inherits the abstract GameController base class.
class WeaponController : public GameController
{
	public:

		// constructor gets current GameState as a pointer, it also initializes the dataTable
		WeaponController(GameState* gs);

		// copy constructor and assignmet operation are deleted		
		WeaponController(const WeaponController&) = delete;
		WeaponController& operator=(const WeaponController&) = delete;

		virtual ~WeaponController() { }

		// Update function iterates through all the weapons and effects in the map and checks wether they need
		// to be updated, blasted, moved, or erased. Function takes time difference since the last update as a parameter. 
		virtual void update(float dt);

		// This function "listens" what's happening on the game field. Depending on the event type it calls wether blastWeapon- or
		// moveWeapon-functions or plants effects and blasts more weapons.  
		virtual void invokeGameEvent(GameEvent event);

		// returns cost of a certain item, identified with i. If i is not found from dataTable, returns zero.
        size_t getItemCost(int i);

	private:

		// weapon handling functions
		void useWeapon(Item::ItemID, int playerID, sf::Vector3i);
		void moveWeapon(sf::Vector2i);
		void moveWeapon(sf::Vector3i);
		void blastWeapon(sf::Vector2i, Weapon*);

		// Information of different weapons
		std::map<Item::ItemID, WeaponData> dataTable;

		// current level
		Level* level;

};


#endif
