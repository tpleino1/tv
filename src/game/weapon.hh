#ifndef MB_WEAPON_HH
#define MB_WEAPON_HH

#include "item.hh"
#include "datatables.hh"

#include <SFML/System.hpp>
#include <vector>
#include <map>

//forward declarations
class Level;
class Tile;


class Weapon : public Item
{
	public:

		/* 	Constructor creates a new weapon by given id.
		*	Parameters:
		*	id: With id weapon is identified to a certain type.
		* 	table: From table we get the right values with given id.
		*	res: Acces to textures, textureRects and config-file.
		*	dir: current direction the player is facing.
		*	playerID: Identification to player that used the weapon. */
		Weapon(Item::ItemID id, std::map<Item::ItemID, WeaponData>& table, State::Resources res, int dir, int playerID);
		
		virtual ~Weapon() {}

		// Updates the time how long weapon has been "alive". If the time is over fuse, blasted is changed to true.
		void update(float dt);

		// Basic getters for blastradius, direction and blast-status.
		size_t getBlastRadius() const;
		int getDirection() const;
		bool isBlasted() const;

		friend class WeaponController;
	private:
	
		/* Calculates the blastpattern of this weapon. 
		* Parameters:
		* xy: Coordinates of the explosion.
		* l: Current level. 
		* Level::getRadius() is used with calculating the patterns. */
		std::vector<sf::Vector3i> calculateBlastPattern(sf::Vector2i xy, Level* l);
		
		int 				blastRadius;
		int 				direction; // 0=up, 1=down, 2=left, 3=right
		size_t 				power;
		float 				fuse;
		float 				eTime;
		bool 				blasted;
		bool 				moveRequest; 
		BlastPatternType 	bpType;
        int 				playerID;
};


#endif
