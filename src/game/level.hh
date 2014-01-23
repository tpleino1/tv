#ifndef MB_LEVEL_HH
#define MB_LEVEL_HH

#include <vector>
#include <memory>
#include <limits>
#include <iostream>

#include <SFML/System/Vector2.hpp>

#include "tile.hh"
#include "terrainStorage.hh"
#include "treasure.hh"
#include "monsterSpawner.hh"

class TileMap 
{

	private:

		std::vector<std::vector<Tile>> map;

		int width;
		int height;

		void recreateMap()
		{
			this->map.clear();
			while (map.size() < (unsigned)width)
			{
				this->map.push_back(std::vector<Tile>(height));
			}
		}

	public:

		friend class Level;		

		TileMap(int width, int height) : width(width), height(height)
		{
			while (map.size() < (unsigned)width)
			{
				this->map.push_back(std::vector<Tile>(height));
			}
		}

		std::vector<Tile>& operator[](int i)
		{
			return this->map.at(i);
		}

		const std::vector<Tile>& operator[](int i) const
		{
			return this->map.at(i);
		}
	
};

struct coordinateComparator 
{

	bool operator()(sf::Vector2i c1, sf::Vector2i c2)
	{
		if (c1.x != c2.x) return (c1.x < c2.x);
		else return (c1.y < c2.y);
	}

};


class Level 
{

	public:

		enum PathType 
		{
			//All tiles on the path must be open
			Open,
			//All tiles on the path must be open, except for the end point
			Reachable,
			//All tiles on the path must be destructible (other weapons count as indestructible)
			Destructible,
			//All tiles on the path must be blastable (weapons are blastable)
			Blastable,
			Any
		};

		enum RadiusType
		{
			Square,
			Diamond,
			Cross,
			Circle,
			Path,
			Visible,
			Blast
		};

		enum MapType
		{
			Campaign,
			Deathmatch
		};

		enum SpawnType
		{
			Player,
			Monster
		};

	private:

		int width;
		int height;
		
		MapType mapType;
		TileMap map;

		unsigned difficulty;

		std::vector<sf::Vector2i> spawnLocations_players;
		std::vector<sf::Vector2i> spawnLocations_monsters;
		std::vector<sf::Vector2i> otherLocations;

		sf::Vector2i exitLocation;

		std::vector<MonsterSpawner*> spawners;

		std::vector<Treasure*> treasures;

	public:

		Level(int w, int h) : width(w), height(h), map(TileMap(w,h)) 
		{
			for (int i=0; i<5; i++)
				treasures.push_back(new Treasure((Treasure::Type)i));
		}

		~Level()
		{
			for (auto s : this->spawners)
			{
				delete s;
			}

			for (auto& t : treasures)
				delete t;
		}

		/* Returns true, if a given coordinate is valid on this level.
		 * If throwException is true, this method will throw an exception
		 * instead of returning false. */
		bool validCoordinate(sf::Vector2i, bool throwException = false) const;

		// Generates a random level;
		void generateMap(int width = 20, int height = 20, unsigned nPlayers = 2, 
			MapType mt = MapType::Deathmatch, bool revealed = false, 
			unsigned difficulty = 0, unsigned nTreasure = 10, int seed = 0);
		//void generateMap(std::vector<Player> & players);

		// Checks that a given level is valid, i.e. actually playable.
		static bool validateLevel(const Level& level);

		int getWidth() const { return this->width; }

		int getHeight() const { return this->height; }

		MapType getMapType() const { return this->mapType; }

		unsigned getDifficulty() const { return this->difficulty; }

		Tile& getTile(sf::Vector2i c)
		{
			this->validCoordinate(c, true);
			return this->map[c.x][c.y];
		}

		const Tile& getTile(sf::Vector2i c) const
		{
			this->validCoordinate(c, true);
			return this->map[c.x][c.y];
		}

		/* Removes the MonsterSpawner from the given coordinates, given
		 * that the coordinates are valid and there actually is a spawner
		 * in the given location.
		 * The function will throw an exception if the coordinate is invalid,
		 * or if the spawner in the tile is not present in the levels
		 * own list of spawners. */
		void removeSpawner(sf::Vector2i c);

		// Returns the Terrain at coordinates (x,y) on the map.
		const Terrain* getTerrain(sf::Vector2i) const;

		// Returns weapon pointer at given coordinates
		Weapon* getWeapon(sf::Vector2i) const;

		// Returns effect pointer at given coordinates
		Effect* getEffect(sf::Vector2i) const;

		Treasure* getTreasure(sf::Vector2i) const;

		const std::vector<sf::Vector2i>& getPlayerSpawnLocations() const
		{
			return this->spawnLocations_players;
		}

		const std::vector<sf::Vector2i>& getMonsterSpawnLocations() const
		{
			return this->spawnLocations_monsters;
		}

		const std::vector<MonsterSpawner*>& getSpawners()
		{
			return this->spawners;
		}

		sf::Vector2i getExitLocation() const
		{
			return this->exitLocation;
		}

		int getDistance(sf::Vector2i start, sf::Vector2i end) const;

		// Returns an std::vector containing all the coordinates within
		// radius r from the given coordinates (x,y) using RadiusType rt.
		std::vector<sf::Vector2i> getRadius(int x, int y, int r,
			RadiusType rt, bool blastCheck = false) const;	
		
		/* Returns the shortest path of given PathType pt between start
		 * and end as an std::vector of Vector2i coordinates in travel
		 * order. Returns an empty vector if no path fulfilling the given
		 * requirements was found.
		 * If there are multiple shortest paths of equal length, this method
		 * will return one of them. */
		std::vector<sf::Vector2i> getPath(sf::Vector2i start, sf::Vector2i goal,
			PathType pt, int maxLength = std::numeric_limits<int>::max()) const;

		// Checks whether there is an open line-of-sight between the start and end
		// coordinates
		bool getVisibility(sf::Vector2i start, sf::Vector2i end, bool blastCheck = false) const;

		std::vector<sf::Vector2i> getNeighbours(int x, int y, bool open = false) const;

		//Reveals all tiles on the map
		void revealMap();

		// Checks if there is a treasure in given coordinates and returns its value
		// or returns zero if none found.
		size_t pickTreasure(sf::Vector2i c);

	private:

		void setPlayerSpawns(unsigned nPlayers);

		void setSpawnPoint(int x, int y, std::vector<sf::Vector2i>& spawnVector);

		void createSpawnArea(int x, int y, int size, bool walls = false);

		void createMonsterSpawners(unsigned, bool walls);

		void setMonsterSpawns(unsigned);

		bool validAreaLocation(int x, int y, int size) const;

		void createTunnels();

		void createExit();

		void createTreasure(unsigned nTreasure);

		void createLavaPockets();

};

#endif


