#include <memory>
#include <SFML/System.hpp>
#include <string>
#include <algorithm>

#include "Utilities.hh"
#include "weaponController.hh"
#include "weapon.hh"
#include "tile.hh"
#include "GameState.hh"
#include "gameEvent.hh"
#include "effect.hh"



// constructor initializes dataTable with weapon information
WeaponController::WeaponController(GameState* gs) : GameController(gs) 
{
	dataTable = initializeWeaponData(gameState->getResources());
	level = &gameState->getLevel();
}




void WeaponController::update(float dt)
{
	// iterate the map throuhg
	for (int j=0; j<level->getHeight(); j++)
	 	for (int i=0; i<level->getWidth(); i++)
		{
			sf::Vector2i vec(i, j);

			Weapon* w = level->getWeapon(vec);
			if (w)
			{
				// blast weapon if it should be blasted
				if (w->isBlasted())
					blastWeapon(vec, w);
				// move weapon if it should be moved
				else if (w->moveRequest)
				{
					w->moveRequest = false;
					moveWeapon(vec);
				}
				// otherwise update weapon
				else
					w->update(dt);
			}

			// If effect has ended remove it from the gamefield
			Effect* e = level->getEffect((vec));
			if (e)
			{
				if (e->isOver())
					level->getTile(vec).setEffect(nullptr);
				else
					e->update(dt);
			}
		}
}

void WeaponController::invokeGameEvent(GameEvent event)
{
	// Listen different events from GameState
	switch (event.eventType)
	{
		// Player uses weapon
		case WeaponUsed:
			for (auto& c : event.getCoordinates())
				// use weapon with given ItemID, playernumber and coordinates
				useWeapon(Item::ItemID(event.ID), event.dataInt, c);
			break;

		// There has been an axplosion
		case Explosion:
			for (auto& c : event.getCoordinates())
			{	
				// Mark all weapons blasted within given coordinates
				Weapon* w = level->getWeapon(sf::Vector2i(c.x,c.y));
				if (w)
					w->blasted = true;
			}	
			break;

		// Player or monster wants to move the weapon
        case WeaponMoved:
            for (auto& c : event.getCoordinates())
            {
            	// Event coordinates represent where the charecter is
            	// so first calculate where the bomb is and then move it
            	sf::Vector3i bombCoords = c;
            	switch (c.z)
			    {
			        case 0: bombCoords.y--; break;
			        case 1: bombCoords.y++; break;
			        case 2: bombCoords.x--; break;
			        case 3: bombCoords.x++; break;
			        default: break;
			    }

			    // Check that there actually is a weapon in certain tile
               	Weapon* w = level->getWeapon(sf::Vector2i(bombCoords.x, bombCoords.y));
				if (w)
				{
	            	w->direction = bombCoords.z;
    	        	w->moveRequest = true;
				}
            }
            break;

        // We are interested if the lava spreads
		case TerrainChanged:
			for (auto& c : event.getCoordinates())
			{
				//  Mark weapon blasted if lava has reached it
				const Terrain* ter = level->getTile(sf::Vector2i(c.x, c.y)).getTerrain();
				if (ter && ter == TerrainStorage::getTerrain("Lava"))
				{
					Weapon* w = level->getWeapon(sf::Vector2i(c.x,c.y));
					if (w)
						w->blasted = true;
				}
			}	
			break;

		// Other type of events are not used
		default:
			break;
	}
}

void WeaponController::useWeapon(Item::ItemID id, int dataInt, sf::Vector3i vec)
{
	// check the tile is free, player should also have checked
	if (!level->getWeapon(sf::Vector2i(vec.x, vec.y)))
	{
		// Make new weapon
		std::unique_ptr<Weapon> w(new Weapon(id, dataTable, gameState->getResources(), vec.z, dataInt));
		
		// flamethrower is not placed to the gamefield
		if (id == Item::Flamethrower)
			blastWeapon(sf::Vector2i(vec.x, vec.y), w.get());

		// Put weapon in a certain tile
		level->getTile(sf::Vector2i(vec.x,vec.y)).setWeapon(std::move(w));
		w = nullptr;

		// make grenades to move
		if (id == Item::Grenade || id == Item::LargeGrenade)
			moveWeapon(vec);
	}
}

// overloaded move function without direction
void WeaponController::moveWeapon(sf::Vector2i v)
{
	// get direction where the weapon is moving
	int dir = level->getWeapon(sf::Vector2i(v.x, v.y))->getDirection();
	moveWeapon(sf::Vector3i(v.x, v.y, dir));
}

void WeaponController::moveWeapon(sf::Vector3i c)
{
	Tile& curr = level->getTile(sf::Vector2i(c.x, c.y));
	Weapon* w = curr.getWeapon();
	if (!w)
		return;

	// mine explodes if someone tries to move it
	if (w->getID() == Item::Mine)
	{
		w->blasted = true;
		return;
	}
    
    // calculate new coordinates
    sf::Vector2i newCoords(c.x, c.y);
    switch (c.z)
    {
        case 0: newCoords.y--; break;
        case 1: newCoords.y++; break;
        case 2: newCoords.x--; break;
        case 3: newCoords.x++; break;
        default: break;
    }

    // check we are inside the gamefield
    if (newCoords.x >= 0 && newCoords.y >= 0 && newCoords.x < level->getWidth() && newCoords.y < level->getHeight())
    {
    	// check the tile is available for taking weapon in
    	Tile& t = level->getTile(sf::Vector2i(newCoords.x, newCoords.y));
    	if (t.isValidForWeapon())
    	{
    		t.setWeapon(std::move(std::unique_ptr<Weapon>(curr.releaseWeapon())));
    		return;
    	}
 	}	
 	// blast grenade if it can't move anymore or it's going over the edge of the map
 	if (w->getID() == Item::Grenade || w->getID() == Item::LargeGrenade)
 		w->blasted = true;

   
}

void WeaponController::blastWeapon(sf::Vector2i vec, Weapon* w)
{
	// create new GameEvent and identificate it with playerid
	GameEvent ge(Explosion);
    ge.dataInt = w->playerID;

    // First calculate ideal blastPattern and then a mask of every reachable tile
	std::vector<sf::Vector3i> blastPattern = w->calculateBlastPattern(vec, level);
	std::vector<sf::Vector2i> mask = level->getRadius(vec.x, vec.y, w->blastRadius, Level::RadiusType::Circle, true);
	
	// remove weapon from the gamefield;
	level->getTile(vec).setWeapon(nullptr);
    
    // Check blastPattern with mask if certain coordinate is reachable or not, remove ones that don't fit    
	for (auto iter = blastPattern.begin(); iter != blastPattern.end();)
    {
        auto it = std::find_if(mask.begin(), mask.end(), [iter](sf::Vector2i v) { return (v.x == iter->x && v.y == iter->y); });
        if (it == mask.end())
            iter = blastPattern.erase(iter);
        else
            iter++;
    }

    // Add coordinates to the GameEvent and place effects to the gamefield 
	for (auto& v : blastPattern)
	{
		ge.addCoordinates(v);
    	std::unique_ptr<Effect> e(new Effect(gameState->getResources()));
	 	level->getTile(sf::Vector2i(v.x, v.y)).setEffect(std::move(e));
	 	e = nullptr;
    }
    // report explosions back to the GameState
	gameState->callGameEvent(ge);
}


// Return items cost if it's found from the dataTable
size_t WeaponController::getItemCost(int i)
{
	if (dataTable.find((Item::ItemID)i) != dataTable.end())
    	return dataTable[(Item::ItemID)i].cost;
    return 0;
}
