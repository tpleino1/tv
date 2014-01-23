#ifndef TERRAIN_HH
#define TERRAIN_HH

#include <string>

enum TerrainType
{
	Empty,
	Solid,
	Indestructible,
	Hazard,
	//For use with nullpointers
	Void
};

/* The class for representing terrains of a map. Each different terrain is
 * created only once and then passed around as a const pointer. */
class Terrain
{	

	private:

		std::string name;
		TerrainType terrainType;

		bool destructible;
		int durability;

	public:

		Terrain(std::string n = "Empty", TerrainType t = Empty, int dur = 0) : name(n), terrainType(t)
		{
			if (t == Indestructible)
			{
				this->durability = 1;
				this->destructible = false;
			}
			else 
			{
				this->durability = dur;
				this->destructible = true;
			}
		}

		std::string getName() const
		{
			return this->name;
		}
		
		bool isDestructible() const
		{
			return this->destructible;
		}

		int getDurability() const
		{
			return this->durability;
		}

		TerrainType getTerrainType() const
		{
			return this->terrainType;
		}

};

#endif
