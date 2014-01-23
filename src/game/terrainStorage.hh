#ifndef MB_TERRAINSTORAGE_HH
#define MB_TERRAINSTORAGE_HH

#include <map>
#include <string>

#include "terrain.hh"

class TerrainStorage
{

	private:

		std::map<std::string, Terrain*> terrains;

		TerrainStorage();
		~TerrainStorage();

		static TerrainStorage tStorage;

		void loadTerrains();

	public:

		static const Terrain* getTerrain(std::string name);

};

#endif
