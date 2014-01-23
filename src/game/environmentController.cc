#include <SFML/System/Vector2.hpp>

#include "environmentController.hh"
#include "level.hh"
#include "GameState.hh"

void EnvironmentController::update(float dt)
{

	this->dtSum += dt;

	if (this->dtSum > 2000)
	{
		this->dtSum = 0;

		Level& level = this->gameState->getLevel();
		GameEvent cTerrain = GameEvent(EventType::TerrainChanged);
		std::vector<sf::Vector2i> newLava;

		for (int i = 0; i < level.getWidth(); i++)
		{
			for (int j = 0; j < level.getHeight(); j++)
			{
				Tile& tile = level.getTile(sf::Vector2i(i,j));
				if (tile.getTerrain() && tile.getTerrain() == TerrainStorage::getTerrain("Lava"))
				{
					cTerrain.addCoordinates(i, j, 0);
					int damage = tile.getDamage();
					if (tile.getTerrain()->getDurability() - damage > 1)
					{
						std::vector<sf::Vector2i> neighbours = level.getNeighbours(i, j, true);
						for (sf::Vector2i n : neighbours)
						{
							newLava.push_back(n);
							Tile& tile2 = level.getTile(n);
							tile2.resetDamage();
							tile2.damage(damage + (int)((TerrainStorage::getTerrain("Lava")->getDurability() - damage)/2.0f));		
						}
					}
				}
			}
		}

		for (sf::Vector2i n : newLava)
		{
			level.getTile(n).setTerrain(TerrainStorage::getTerrain("Lava"));
			cTerrain.addCoordinates(n.x, n.y, 0);
		}

		this->gameState->callGameEvent(cTerrain);
	}
}

void EnvironmentController::invokeGameEvent(GameEvent event)
{
	switch (event.eventType)
	{
		case (EventType::Explosion): this->damageTerrain(event); break;
		case (EventType::MonsterDigs): this->damageTerrain(event); break;
		case (EventType::PlayerDigs): this->damageTerrain(event);
		case (EventType::PlayerMoves): this->revealTerrain(event); break;
		default: break;
	}
}

void EnvironmentController::damageTerrain(GameEvent event)
{
	const std::vector<sf::Vector3i>& coords = event.getCoordinates();

	GameEvent cTerrain = GameEvent(EventType::TerrainChanged);
	
	for (unsigned i = 0; i < coords.size(); i++)
	{

		Tile& tile = this->gameState->getTile(sf::Vector2i(coords[i].x, coords[i].y));

		if (tile.getMonsterSpawner())
		{
			MonsterSpawner* spawner = tile.getMonsterSpawner();
			spawner->doDamage(coords[i].z);
			if (spawner->getDamage() >= spawner->getDurability())
			{
				this->gameState->getLevel().removeSpawner(sf::Vector2i(coords[i].x, coords[i].y));
			}
		}

		if (tile.getTerrain() == nullptr || !tile.getTerrain()->isDestructible())
		{
			continue;
		}

		tile.damage(coords[i].z);
		
		if (tile.getTerrain()->getName() == "Stone" && tile.getDamage() >= 1.5*tile.getTerrain()->getDurability())
		{
			tile.setTerrain(TerrainStorage::getTerrain("Empty"));
			cTerrain.addCoordinates(coords[i].x, coords[i].y, 0);
		}
		else if (tile.getDamage() >= tile.getTerrain()->getDurability())
		{
			if (tile.getTerrain()->getName() == "Stone")
			{
				tile.setTerrain(TerrainStorage::getTerrain("Gravel"));
				tile.resetDamage();
				cTerrain.addCoordinates(coords[i].x, coords[i].y, 0);
			}
			else 
			{
				tile.setTerrain(TerrainStorage::getTerrain("Empty"));
				cTerrain.addCoordinates(coords[i].x, coords[i].y, 0);
			}
		}
	}

	if (!cTerrain.getCoordinates().empty())
	{
		this->gameState->callGameEvent(cTerrain);
	}

}

void EnvironmentController::revealTerrain(GameEvent event)
{
	const std::vector<sf::Vector3i>& locations = event.getCoordinates();
	
	for (unsigned i = 0; i < locations.size(); i++)
	{

		sf::Vector2i coord = sf::Vector2i(locations[i].x, locations[i].y);

		//Adjust for movement direction
		/*if (event.eventType == EventType::PlayerMoves)
		{
			Direction dir = Direction(locations[i].z);

			switch (dir)
			{
				case (Direction::Up): coord.y -= 1; break;
				case (Direction::Down): coord.y += 1; break;
				case (Direction::Left): coord.x -= 1; break;
				case (Direction::Right): coord.x += 1; break;
				default: break;
			}
		}*/
		//Apparently that was unnecessary, left in as a backup

		//Reveal all visible tiles
		std::vector<sf::Vector2i> rCoords = this->gameState->getLevel().getRadius(coord.x, coord.y, 7, Level::RadiusType::Visible);
		
		for (sf::Vector2i rPoint : rCoords)
		{
			this->gameState->getTile(rPoint).reveal();
		}

		//Reveal all tiles within a radius of two
		rCoords = this->gameState->getLevel().getRadius(coord.x, coord.y, 2, Level::RadiusType::Circle);
		for (sf::Vector2i rPoint : rCoords)
		{
			this->gameState->getTile(rPoint).reveal();
		}

	}
}


