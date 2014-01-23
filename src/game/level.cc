
#include <cmath>
#include <vector>
#include <memory>
#include <algorithm>
#include <deque>
#include <map>
#include <array>
#include <stdexcept>
#include <random>
#include <iostream>

#include <cstdlib>
#include <ctime>

#include "level.hh"
#include "treasure.hh"
#include "Utilities.hh"

bool Level::validCoordinate(sf::Vector2i coord, bool throwException) const
{

	if (!(coord.x >= 0 && coord.x < this->width && coord.y >= 0 && coord.y < this->height)) 
	{
		if (throwException) throw std::invalid_argument("Non-valid coordinate given");
		return false;
	}
	return true;

}


bool Level::validateLevel(const Level& l)
{
	sf::Vector2i start, end;

	if (l.getMapType() == MapType::Deathmatch)
	{
		for (unsigned i = 0; i < l.spawnLocations_players.size(); i++)
		{
			start = l.spawnLocations_players[i];
			for (unsigned j = i+1; j < l.spawnLocations_players.size(); j++)
			{
				end = l.spawnLocations_players[j];
				if (start != end)
				{
					if (l.getPath(start, end, PathType::Destructible).empty())
					{
						return false;
					}
				}
			}
		}
		return true;
	}
	else if (l.getMapType() == MapType::Campaign)
	{
		start = l.spawnLocations_players[0];
		end = l.exitLocation;
		if (l.getPath(start, end, PathType::Destructible).empty())
		{
			return false;
		}
		return true;
	}

	return true;
}

const Terrain* Level::getTerrain(sf::Vector2i coord) const
{
	validCoordinate(coord,true);
	return this->map[coord.x][coord.y].getTerrain();

}

// return the weapon pointer in certain tile
Weapon* Level::getWeapon(sf::Vector2i coord) const
{
	validCoordinate(coord, true);
	return this->map[coord.x][coord.y].getWeapon();
}

// Return the effect pointer in certain tile
Effect* Level::getEffect(sf::Vector2i coord) const
{
	validCoordinate(coord, true);
	return this->map[coord.x][coord.y].getEffect();
}

// Return the treasure pointer in certain tile
Treasure* Level::getTreasure(sf::Vector2i coord) const
{
	validCoordinate(coord, true);
	return this->map[coord.x][coord.y].getTreasure();
}

int Level::getDistance(sf::Vector2i start, sf::Vector2i end) const
{
	return std::abs(end.x-start.x)+std::abs(end.y-start.y);
}

std::vector<sf::Vector2i> Level::getPath(sf::Vector2i start, sf::Vector2i goal,
	PathType pt, int maxLength) const
{

	std::vector<sf::Vector2i> path;

	std::map<sf::Vector2i, sf::Vector2i, coordinateComparator> cameFrom;
	std::map<sf::Vector2i, int, coordinateComparator> pathLength;
	std::vector<sf::Vector2i> closed;
	std::map<sf::Vector2i, int, coordinateComparator> open;

	open[start] = this->getDistance(start, goal);
	sf::Vector2i current = start;
	pathLength[current] = 0;

	while (!open.empty())
	{

		int min_value = std::numeric_limits<int>::max();
		std::map<sf::Vector2i, int>::iterator iter;
		for (iter = open.begin(); iter != open.end(); iter++)
		{
			if (iter->second < min_value)
			{
				min_value = iter->second;
				current = iter->first;
			}
		}

		if (current == goal) break;

		if (pathLength[current] > maxLength) break;

		iter = open.find(current);
		open.erase(iter);
		closed.push_back(current);

		if (pt == PathType::Reachable)
		{
			if (this->map[current.x][current.y].getTerrain() != nullptr && 
			(this->map[current.x][current.y].getTerrain()->getTerrainType() != TerrainType::Empty))
			{
				continue;
			}
		}
		/*else if (pt == PathType::Open)
		{
			if (this->map[current.x][current.y].getTerrain() != nullptr && 
			(this->map[current.x][current.y].getTerrain()->getTerrainType() != TerrainType::Empty ||
			this->map[current.x][current.y].hasItem()))
			{
				continue;
			}
		}*/

		std::vector<sf::Vector2i> neighbours = this->getNeighbours(current.x, current.y);

		for (sf::Vector2i node : neighbours)
		{
			if (!validCoordinate(node)) continue;

			if (find(closed.begin(), closed.end(), node) == closed.end() &&
				open.find(node) == open.end())
			{
				const Terrain* ter = this->map[node.x][node.y].getTerrain();
				if (ter != nullptr)
				{
					if((pt == PathType::Any || pt == PathType::Reachable) ||
						(ter->isDestructible() && pt == PathType::Blastable) ||
						(((ter->getTerrainType() == TerrainType::Empty) ||
						(ter->isDestructible() && pt == PathType::Destructible)) && 
						!this->map[node.x][node.y].hasItem()))
					{
						open[node] = this->getDistance(node, goal);
						cameFrom[node] = current;
						pathLength[node] = pathLength[current]+1;
					}
					else
					{
						closed.push_back(node);
					}
				}
			}
		}
	}

	if (current == goal) {
		while(current != start)
		{
			path.push_back(current);
			current = cameFrom[current];
		}
		path.push_back(current);
		std::reverse(path.begin(), path.end());
	}

	return path;
}


std::vector<sf::Vector2i> Level::getRadius(int x, int y, int r,
	RadiusType rt, bool blastCheck) const
{

	std::vector<sf::Vector2i> coords;

	for (int dx = -r; dx <= r; dx++)
	{
		for (int dy = -r; dy <= r; dy++)
		{
			if (!this->validCoordinate(sf::Vector2i(x+dx, y+dy))) continue;

			if (rt != RadiusType::Square)
			{
				if (rt == RadiusType::Diamond || rt == RadiusType::Path ||
					rt == RadiusType::Visible || rt == RadiusType::Blast)
				{
					if (std::abs(dx)+std::abs(dy) > r)
					{
						continue;
					}
				}
				else if (rt == RadiusType::Cross)
				{
					if (dx != 0 && dy != 0)
					{
						continue;
					}
				}
				else if (rt == RadiusType::Circle)
				{
					//dx^2+dy^2 > r^2+(r-1)
					if ((std::pow(dx,2)+std::pow(dy,2)) > (std::pow(r,2)+r-1))
					{
						continue;
					}
				}

				if (rt == RadiusType::Visible)
				{
					if (this->getVisibility(sf::Vector2i(x,y), sf::Vector2i(x+dx,y+dy)))
					{
						std::vector<sf::Vector2i> path =
							this->getPath(sf::Vector2i(x,y), sf::Vector2i(x+dx,y+dy) , PathType::Reachable, r);
						if (path.empty())
							continue;
					}
					else continue;
				}

				if (rt == RadiusType::Blast)
				{
					if ((dx != 0 || dy != 0))
					{
						std::vector<sf::Vector2i> path =
							this->getPath(sf::Vector2i(x,y), sf::Vector2i(x+dx,y+dy) , PathType::Blastable, r);
					 	if (path.empty())
							continue;
					}
				}

				if (rt == RadiusType::Path)
				{
					if ((dx != 0 || dy != 0))
					{
						std::vector<sf::Vector2i> path =
							this->getPath(sf::Vector2i(x,y), sf::Vector2i(x+dx,y+dy) , PathType::Open, r);
					 	if (path.empty())
							continue;
					}
				}
				
			}

			coords.push_back(sf::Vector2i(x+dx,y+dy));

		}
	}

	if (blastCheck)
	{
		sf::Vector2i start = sf::Vector2i(x,y);
		for (auto iter = coords.begin(); iter != coords.end();)
		{
			if (*iter != start && !getVisibility(start, *iter, true))
			{
				iter = coords.erase(iter);
			}
			else iter++;
		}
	}

	return coords;
}

bool Level::getVisibility(sf::Vector2i start, sf::Vector2i end, bool blastCheck) const
{

	//Check that the given coordinates are valid
	this->validCoordinate(start, true);
	this->validCoordinate(end, true);

	sf::Vector2f d = sf::Vector2f(end.x-start.x, end.y-start.y);
	float length = std::sqrt(std::pow(d.x,2)+std::pow(d.y,2));
	sf::Vector2f r = d/length;
	sf::Vector2f vec = sf::Vector2f(start.x+0.5f, start.y+0.5f);
	
	for (float i = 0.0f; i < length; i++)
	{

		sf::Vector2i coords = sf::Vector2i((int)vec.x, (int)vec.y);
		if (coords == end) break;

		const Terrain* ter = this->getTile(coords).getTerrain();
		if (!blastCheck)
		{
			if (ter == nullptr || ter->getTerrainType() != TerrainType::Empty)
			{
				return false;
			}
		}
		else 
		{
			if (ter == nullptr || ter->getTerrainType() == TerrainType::Indestructible)
			{
				return false;
			}
		}

		vec += r;

	}
	return true;

}


void Level::generateMap(int w, int h, unsigned nPlayers, MapType mt, bool revealed, 
	unsigned diff, unsigned nTreasure, int seed)
{

	//Set width and height
	if (w < 20) w = 20;
	if (h < 20) h = 20;
	this->width = w;
	this->height = h;

	//Set map type
	this->mapType = mt;

	//Clamp number of players to valid range
	if (nPlayers > 4)
	{
		nPlayers = 4;
	}
	else if (nPlayers < 1)
	{
		nPlayers = 1;
	}
	/*else if (nPlayers < 2)
	{
		if (this->mapType == MapType::Deathmatch)
		{
			nPlayers = 2;
		}
		else if (this->mapType == MapType::Campaign)
		{
			if (nPlayers < 1) nPlayers = 1;
		}
	}*/

	this->difficulty = diff;
	
	std::array<std::array<float, 129>, 129> values;

	float dist_max = 64.0f;

	if (seed == 0) std::srand(std::time(0));
	else std::srand(seed);

	unsigned step = 128;
	values[0][0] = floatRNG(0.0f, 16.0f);
	values[0][step] = floatRNG(0.0f, 16.0f);
	values[step][0] = floatRNG(0.0f, 16.0f);
	values[step][step] = floatRNG(0.0f, 16.0f);

	while (step > 1) 
	{

		step = step/2;
		dist_max = dist_max/2.0f;

		//Square step: horizontal
		for (unsigned i = step; i < values.size(); i += 2*step)
		{
			for (unsigned j = 0; j < values[0].size(); j += 2*step)
			{
				values[i][j] = 0.5f*(values[i-step][j] + values[i+step][j]) + floatRNG(-dist_max, dist_max);
				if (values[i][j] > 16.0f) values[i][j] = 16.0f;
				else if (values[i][j] < 0.0f) values[i][j] = 0.0f;
			}
		}
						
		//Square step: vertical
		for (unsigned j = step; j < values[0].size(); j += 2*step)
		{
			for (unsigned i = 0; i < values.size(); i += 2*step)
			{
				values[i][j] = 0.5f*(values[i][j-step] + values[i][j+step]) + floatRNG(-dist_max, dist_max);
				if (values[i][j] > 16.0f) values[i][j] = 16.0f;
				else if (values[i][j] < 0.0f) values[i][j] = 0.0f;
			}
		}

		//Diamond step
		for (unsigned i = step; i < values.size(); i += 2*step)
		{
			for (unsigned j = step; j < values[0].size(); j += 2*step)
			{
				values[i][j] = 0.25f*(values[i-step][j] + values[i+step][j] + values[i][j-step] + values[i][j+step]) + floatRNG(-dist_max, dist_max);
				if (values[i][j] > 16.0f) values[i][j] = 16.0f;
				else if (values[i][j] < 0.0f) values[i][j] = 0.0f;
			}
		}

	}

	//Adjust values to occupy the whole [0,16] range
	float min_value = std::numeric_limits<float>::max();
	float max_value = std::numeric_limits<float>::min();
	for (unsigned i = 0; i < values.size(); i++)
	{
		for (unsigned j = 0; j < values[0].size(); j++)
		{
			if (values[i][j] < min_value) min_value = values[i][j];
			if (values[i][j] > max_value) max_value = values[i][j];
		}
	}
	if (min_value > 0.0f)
	{
		for (unsigned i = 0; i < values.size(); i++)
		{
			for (unsigned j = 0; j < values[0].size(); j++)
			{
				values[i][j] -= min_value;
			}
		}
		max_value -= min_value;
	}
	if (max_value < 16.0f)
	{
		float multiplier = 16.0f/max_value;
		for (unsigned i = 0; i < values.size(); i++)
		{
			for (unsigned j = 0; j < values[0].size(); j++)
			{
				values[i][j] *= multiplier;
			}
		}
	}
	
	//Form a map using the generated values
	float x_ratio = 128.0f/(this->width-1.0f);
	float y_ratio = 128.0f/(this->height-1.0f);

	this->map.width = this->width;
	this->map.height =  this->height;
	this->map.recreateMap();

	for (int i = 0; i < this->width; i++)
	{
		for (int j = 0; j < this->height; j++)
		{
			int x = (int)(i*x_ratio);
			int y = (int)(j*y_ratio);
			float val = values[x][y];

			if (val < 6.0f) this->map[i][j].setTerrain(TerrainStorage::getTerrain("Dirt"));
			else if (val < 7.0f) this->map[i][j].setTerrain(TerrainStorage::getTerrain("Gravel"));
			else if (val < 15.5f) this->map[i][j].setTerrain(TerrainStorage::getTerrain("Stone"));
			else this->map[i][j].setTerrain(TerrainStorage::getTerrain("Bedrock"));

			if (revealed) this->map[i][j].reveal();
			else this->map[i][j].obscure();

		}
	}

	//Create players' spawn locations
	this->setPlayerSpawns(nPlayers);

	//If generating campaign map, create exit
	if (this->mapType == MapType::Campaign)
	{
		this->createExit();
	}
	else
	{
		//If not generating a campaign map, set exitLocation out of map bounds
		this->exitLocation = sf::Vector2i(-1, -1);
	}

	//Erase possible old spawners
	this->spawners.erase(this->spawners.begin(), this->spawners.end());

	//Create monster spawners
	this->createMonsterSpawners(this->difficulty, (this->mapType == MapType::Campaign));

	//Create tunnels
	this->createTunnels();

	//Create monster spawn points
	this->setMonsterSpawns(this->difficulty);

	//Create treasure
	this->createTreasure(nTreasure);

	//Create Lava pockets
	this->createLavaPockets();

}

void Level::createExit()
{

	int x, y;

	do
	{

		x = intRNG(5,  this->getWidth()-4);
		y = intRNG(5, this->getHeight()-4);

	}
	while (!validAreaLocation(x, y, 2));

	this->createSpawnArea(x, y, 1, true);
	this->exitLocation = sf::Vector2i(x, y);
	this->map[x][y].exit = true;
	
}

void Level::createTunnels()
{
	std::uniform_int_distribution<int> dist;

	//if (this->mapType == MapType::Campaign)
	//{
		//Add other locations
		int x, y;

		for (int i = 0; i < (int)((this->getWidth()*this->getHeight())/300.0f); i++)
		{

			x = intRNG(5,  this->getWidth()-4);
			y = intRNG(5, this->getHeight()-4);

			if (this->validAreaLocation(x, y, 7))
			{
				this->otherLocations.push_back(sf::Vector2i(x,y));
			}
		}

		std::vector<sf::Vector2i> locations;

		if (this->mapType == MapType::Campaign)
		{
			locations.insert(locations.end(), this->spawnLocations_players.begin(), this->spawnLocations_players.end());
		}

		locations.insert(locations.end(), this->otherLocations.begin(), this->otherLocations.end());
		locations.push_back(this->exitLocation);

		for (MonsterSpawner* ms : this->spawners)
		{
			locations.push_back(ms->getLocation());
		}

		int i, j;
		sf::Vector2i start, end;
		while (locations.size() > 1)
		{

			i = intRNG(0, locations.size());
			start = locations[i];
			locations.erase(locations.begin() + i);

			j = intRNG(0, locations.size());
			end = locations[j];
			locations.erase(locations.begin() + j);

			std::vector<sf::Vector2i> path = this->getPath(start, end, PathType::Destructible);

			for (unsigned k = 0; k < path.size(); k++)
			{
				this->getTile(path[k]).setTerrain(TerrainStorage::getTerrain("Empty"));
			}
			
		}

	//}
}

void Level::createTreasure(unsigned nTreasure)
{
	std::uniform_int_distribution<int> dist;

	int x, y;
	unsigned tCount = 0;
	while (tCount < nTreasure)
	{

		x = intRNG(0, this->width);
		y = intRNG(0, this->height);

		TerrainType tt = this->getTile(sf::Vector2i(x,y)).getTerrainType();
		if (tt == TerrainType::Solid && this->getTile(sf::Vector2i(x,y)).getTreasure() == nullptr)
		{
			this->getTile(sf::Vector2i(x,y)).setTreasure(treasures[intRNG(0,5)]);
			tCount++;
		}
	}

}

void Level::setPlayerSpawns(unsigned nPlayers) 
{

	//Clear old spawn locations
	this->spawnLocations_players.clear();

	int x, y;

	if (this->mapType == MapType::Deathmatch)
	{

		for (unsigned i = 0; i < nPlayers; i++)
		{

			if (i == 0 || i == 2) x = intRNG(3, 7);
			else x = this->getWidth()-intRNG(3, 7);

			if (i == 0 || i == 3) y = intRNG(3, 7);
			else y = this->getHeight()-intRNG(3, 7);

			this->createSpawnArea(x, y, 1);
			this->setSpawnPoint(x, y, this->spawnLocations_players);
		}

	}
	else if (this->mapType == MapType::Campaign)
	{

		int dx, dy;

		x = intRNG(5, this->getWidth()-4);
		y = intRNG(5, this->getHeight()-4);

		this->createSpawnArea(x, y, 2, true);

		for (unsigned i = 0; i < nPlayers; i++)
		{
			if (i == 0) dx = -1;
			else if (i == 1) dx = 1;
			else dx = 0;

			if (i == 2) dy = -1; 
			else if (i == 3) dy = 1;
			else dy = 0;

			this->setSpawnPoint(x+dx, y+dy, this->spawnLocations_players);

		}
				
	}

}

void Level::createMonsterSpawners(unsigned difficulty, bool walls) 
{

	int x, y;

	unsigned nSpawns = difficulty;
	unsigned spawns = 0;
	for (unsigned i = 0; i < nSpawns*5 && spawns < nSpawns; i++) 
	{

		x = intRNG(5, this->getWidth()-4);
		y = intRNG(5, this->getHeight()-4);

		if (this->validAreaLocation(x, y, (walls ? 3 : 2)))
		{
			this->createSpawnArea(x, y, 1, walls);

			MonsterSpawner* spawner = new MonsterSpawner(sf::Vector2i(x, y), difficulty);
			this->spawners.push_back(spawner);
			this->map[x][y].spawner = spawner;
			spawns++;
		}
	}
}

void Level::setMonsterSpawns(unsigned difficulty)
{
	//Clear old spawn locations
	this->spawnLocations_monsters.clear();

	int x, y;
	std::uniform_int_distribution<int> dist;
	unsigned nSpawns = 2*difficulty;
	unsigned spawns = 0;
	for (unsigned i = 0; i < nSpawns*5 && spawns < nSpawns; i++) 
	{

		x = intRNG(5, this->getWidth()-4);
		y = intRNG(5, this->getHeight()-4);

		if (this->getTile(sf::Vector2i(x,y)).getTerrainType() == TerrainType::Empty)
		{
			bool valid = true;
			for (sf::Vector2i spawn : this->spawnLocations_players)
			{
				if (getDistance(sf::Vector2i(x,y), spawn) < 8)
				{
					valid = false;
					break;
				}
			}
			if (valid)
			{
				this->setSpawnPoint(x, y, this->spawnLocations_monsters);
				spawns++;
			}
		}
	}

}

void Level::setSpawnPoint(int x, int y, std::vector<sf::Vector2i>& spawnVector)
{
	spawnVector.push_back(sf::Vector2i(x, y));
}

void Level::createSpawnArea(int x, int y, int size, bool walls) 
{

	std::vector<sf::Vector2i> radius;

	if (walls)
	{
		radius = this->getRadius(x, y, size+1, RadiusType::Square);
		for (sf::Vector2i coords : radius)
		{
			this->getTile(coords).setTerrain(TerrainStorage::getTerrain("Steel"));
		}

		sf::Vector2i coord = sf::Vector2i(x, y);

		Direction dir = Direction(intRNG(0, 4));

		switch (dir)
		{
			case (Direction::Up): coord.y -= size+1; break;
			case (Direction::Down): coord.y += size+1; break;
			case (Direction::Left): coord.x -= size+1; break;
			case (Direction::Right): coord.x += size+1; break;
			default: break;
		}

		this->getTile(coord).setTerrain(TerrainStorage::getTerrain("Stone"));

	}

	radius = this->getRadius(x, y, size, RadiusType::Square);

	for (sf::Vector2i coords : radius)
	{
		this->getTile(coords).setTerrain(TerrainStorage::getTerrain("Empty"));
	}

}

bool Level::validAreaLocation(int x, int y, int size) const
{

	std::vector<sf::Vector2i> radius = this->getRadius(x, y, size, RadiusType::Square);

	for (sf::Vector2i coord : radius)
	{
		const Terrain* ter = this->getTile(coord).getTerrain();
		if (ter == nullptr || ter->getTerrainType() == TerrainType::Empty)
		{
			return false;
		}
	}

	return true;
	
}

void Level::revealMap()
{
	for (int i = 0; i < this->getWidth(); i++)
	{
		for (int j = 0; j < this->getHeight(); j++)
		{
			this->map[i][j].reveal();
		}
	}
}

void Level::removeSpawner(sf::Vector2i c)
{
	this->validCoordinate(c, true);

	MonsterSpawner* temp;
	if (this->getTile(c).spawner)
	{
		temp = this->getTile(c).spawner;
		this->getTile(c).spawner = nullptr;
		std::vector<MonsterSpawner*>::iterator iter = 
			std::find(this->spawners.begin(), this->spawners.end(), temp);
		if (iter != this->spawners.end())
		{
			this->spawners.erase(iter);
		}
		//This should never happen
		else throw std::logic_error("Removing non-existing spawner");
	}
}


size_t Level::pickTreasure(sf::Vector2i c)
{
	int val = 0;
	if (validCoordinate(c, true))
	{
		Treasure* t = getTile(c).getTreasure();
		if (t)
		{	
			val = t->getValue();
			getTile(c).setTreasure(nullptr);
		}
	}
	return val;
}

std::vector<sf::Vector2i> Level::getNeighbours(int x, int y, bool open) const
{
	std::vector<sf::Vector2i> neighbours;
	neighbours.push_back(sf::Vector2i(x+1, y));
	neighbours.push_back(sf::Vector2i(x-1, y));
	neighbours.push_back(sf::Vector2i(x, y+1));
	neighbours.push_back(sf::Vector2i(x, y-1));

	for (auto iter = neighbours.begin(); iter != neighbours.end();)
	{
		if (!this->validCoordinate(*iter))
		{
			iter = neighbours.erase(iter);
		}
		else iter++;
	}

	if (open)
	{
		for (auto iter = neighbours.begin(); iter != neighbours.end();)
		{
			if (this->getTile(*iter).getTerrainType() != TerrainType::Empty)
			{
				iter = neighbours.erase(iter);
			}
			else iter++;
		}
	}

	return neighbours;
}

void Level::createLavaPockets()
{
	
	int x, y;

	unsigned nPockets = (int)((this->getWidth()*this->getHeight())/500.0f) + this->difficulty/2;
	unsigned pockets = 0;
	for (unsigned i = 0; i < nPockets*3 && pockets < nPockets; i++) 
	{

		x = intRNG(2, this->getWidth()-1);
		y = intRNG(2, this->getHeight()-1);

		if (this->validAreaLocation(x, y, 1))
		{
			this->getTile(sf::Vector2i(x,y)).setTerrain(TerrainStorage::getTerrain("Lava"));
			pockets++;
		}
	}

}

