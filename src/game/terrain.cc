#include "terrain.hh"
#include "terrainStorage.hh"

void TerrainStorage::loadTerrains()
{
	terrains["Empty"] = new Terrain();
	terrains["Dirt"] = new Terrain("Dirt", TerrainType::Solid, 10);
	terrains["Stone"] = new Terrain("Stone", TerrainType::Solid, 100);
	terrains["Gravel"] = new Terrain("Gravel", TerrainType::Solid, 15);
	terrains["Bedrock"] = new Terrain("Bedrock", TerrainType::Indestructible);
	terrains["Steel"] = new Terrain("Steel", TerrainType::Indestructible);
	terrains["Lava"] = new Terrain("Lava", TerrainType::Hazard, 256);
}
