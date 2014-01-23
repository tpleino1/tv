#include "terrainStorage.hh"

TerrainStorage TerrainStorage::tStorage;

TerrainStorage::TerrainStorage() 
{
	loadTerrains();
}
		
TerrainStorage::~TerrainStorage() 
{
	for (auto pair : terrains)
	{
	    delete pair.second;
	}
}

const Terrain* TerrainStorage::getTerrain(std::string name) 
{
	return tStorage.terrains[name];
}
