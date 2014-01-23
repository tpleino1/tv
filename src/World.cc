#include "World.hh"

#include <cstdlib>		// rand(), srand()
#include <ctime>		// time()
#include <functional>	// hash()

#include "GameState.hh"

World::World(GameState* gs, float tilesize, unsigned w, unsigned h) :
	GameController(gs),
	tilesize(tilesize),
	level(w, h)
{
	srand(time(NULL));

	this->background = sf::Sprite(this->gameState->getResources().textures->get("tunnel" + toString(intRNG(1, 5))));
	this->treasureCount = 0;
	this->treasureValue = 0;
}


Level& World::getLevel()
{
	return this->level;
}


void World::generateLevel(int w, int h, unsigned nPlayers, Level::MapType mt,
	bool revealed, unsigned difficulty, unsigned treasures, int seed)
{

	do
	{
		this->level.generateMap(w, h, nPlayers, mt, revealed, difficulty, treasures, seed);
		//Sets seed to zero in case created level was invalid
		seed = 0;
	}
	while (!Level::validateLevel(level));

	const float scale = 1.15;
	this->tiles.clear();
	this->tiles.reserve(this->level.getHeight() * this->level.getWidth());
	sf::RectangleShape rect = sf::RectangleShape(scale * sf::Vector2f(this->tilesize, this->tilesize));
	rect.setOrigin(0.5f * rect.getSize());
	this->tiles.insert(tiles.begin(), this->level.getHeight() * this->level.getWidth(), rect);

	for (int j = 0; j < this->level.getHeight(); j++)
	{
		for (int i = 0; i < this->level.getWidth(); i++)
		{
			this->tiles[(i+j*this->level.getWidth())].setPosition(sf::Vector2f((i+0.5f)*this->tilesize, (j+0.5f)*this->tilesize));
			this->updateTexture(sf::Vector2i(i,j));
		}
	}

	this->initTreasureValues();

	this->background.setTextureRect(sf::IntRect(0, 0, (int)this->level.getWidth()*this->tilesize, (int)this->level.getHeight()*this->tilesize));
}


sf::FloatRect World::getLocalBounds() const
{
	return sf::FloatRect(0.0f, 0.0f, this->level.getWidth()*this->tilesize, this->level.getHeight()*this->tilesize);
}

void World::draw(sf::RenderTarget& target, sf::RenderStates states) const
{

	states.transform *= getTransform();

	target.draw(this->background, states);

	sf::RectangleShape rect;
	sf::Vector2f pos;
	std::hash<int> fog_hash;
	const sf::Texture* fogTexture = &this->gameState->getResources().textures->get("terrain");
	const sf::IntRect& fogRect = this->gameState->getResources().textureRects->get("Fog");

	for (int i = 0; i < this->level.getWidth(); i++)
	{
		for (int j = 0; j < this->level.getHeight(); j++)
		{

			const Tile& tile = this->level.getTile(sf::Vector2i(i,j));
			rect = this->tiles[i+j*this->level.getWidth()];

			pos = tilesize * sf::Vector2f((float)i + 0.5f, (float)j + 0.5f);

			//Check that the tile is visible (i.e. should be drawn)
			if (!tile.isVisible())
			{
				rect.setTexture(fogTexture, true);
				rect.setTextureRect(nextRect(fogRect, 0, (fog_hash(i*j) % 6) + 1));
				rect.setFillColor(sf::Color::White);
				target.draw(rect, states);

				//Draw digging monsters
				Character* character = tile.getCharacter();
				if (character && character->type == Character::Type::Monster && character->action == Character::Action::Digs)
				{
					character->image.setPosition(pos);
					target.draw(character->image, states);
				}

				continue;
			}

			//Draw terrain
			target.draw(rect, states);

			//Draw spawners
			if (tile.getMonsterSpawner())
			{
				rect.setTexture(&this->gameState->getResources().textures->get("misc"));
				rect.setTextureRect(this->gameState->getResources().textureRects->get("portal"));
				rect.setFillColor(sf::Color::White);
				target.draw(rect, states);
			}

			//Draw exit
			else if (tile.hasExit())
			{
				rect.setTexture(&this->gameState->getResources().textures->get("misc"));
				rect.setTextureRect(this->gameState->getResources().textureRects->get("exit"));
				rect.setFillColor(sf::Color::White);
				target.draw(rect, states);
			}

			//Draw bombs
			Weapon* w = level.getWeapon(sf::Vector2i(i,j));
			if (w)
            {
                w->setPosition(pos);
				target.draw(w->getRect(), states);
			}
			
			//Draw player
			Character* character = tile.getCharacter();
			if (character != nullptr)
			{
				character->image.setPosition(pos);
				target.draw(character->image, states);
			}

			//Draw treasure
			Treasure* treasure = tile.getTreasure();
	    	if (treasure)
	    	{
				rect = this->tiles[i+j*this->level.getWidth()];
				rect.setFillColor(sf::Color(sf::Color::White));
				rect.setTexture(&this->gameState->getResources().textures->get("misc"));
				rect.setTextureRect(treasure->getTextureRect());
				rect.setRotation(0);
				target.draw(rect, states);
	    	}

			//Draw effects
			Effect* e = level.getEffect(sf::Vector2i(i,j));
			if (e)
            {
                e->setPosition(pos);
				target.draw(e->getRect(), states);
            }

		}
	}

}

void World::update(float) { }

void World::invokeGameEvent(GameEvent event)
{
	if (event.eventType == EventType::TerrainChanged)
	{
		for (sf::Vector3i coords : event.getCoordinates())
		{
			this->updateTexture(sf::Vector2i(coords.x, coords.y));
		}
	}
	else if (event.eventType == EventType::ItemPicked)
	{
		this->updateTreasureValues(event);
	}
}

//Old function saved as a back-up
/*void World::updateTexture(sf::Vector2i coords)
{
	if (!this->level.validCoordinate(coords, true)) return;

	sf::RectangleShape& rect = this->tiles[coords.x+coords.y*this->level.getWidth()];

	const Terrain* ter = this->level.getTerrain(coords);
	if (ter != nullptr)
	{
		if (ter->getName() == "Dirt") rect.setFillColor(sf::Color(128,75,25));
		else if (ter->getName() == "Empty") rect.setFillColor(sf::Color(90,40,15));
		else if (ter->getName() == "Gravel") rect.setFillColor(sf::Color(160,150,135));
		else if (ter->getName() == "Stone") rect.setFillColor(sf::Color(120,120,120));
		else if (ter->getName() == "Bedrock") rect.setFillColor(sf::Color(80,80,80));
		else if (ter->getName() == "Steel") rect.setFillColor(sf::Color(215,215,205));
		else rect.setFillColor(sf::Color(0,0,0));
	}
}*/

void World::updateTexture(sf::Vector2i coords)
{
	if (!this->level.validCoordinate(coords)) return;

	State::Resources res = this->gameState->getResources();

	sf::RectangleShape& rect = this->tiles[coords.x+coords.y*this->level.getWidth()];

	const Terrain* ter = this->level.getTerrain(coords);
	if (ter != nullptr)
	{

		rect.setRotation(90.f * intRNG(0, 4));

		if (ter->getTerrainType() != TerrainType::Empty)
		{
			rect.setFillColor(sf::Color::White);
			rect.setTexture(&res.textures->get("terrain"));
			rect.setTextureRect(nextRect(res.textureRects->get(ter->getName()), 0, intRNG(1, 7)));
		}
		else
		{
			rect.setTexture(nullptr);
			rect.setFillColor(sf::Color(0,0,0,0));
		}
	}

}

void World::updateTreasureValues(GameEvent /*event*/)
{
	initTreasureValues();
	/*for (sf::Vector3i coord : event.getCoordinates())
	{
		this->treasureCount--;
		this->treasureValue -= coord.z;
	}
	*/

}

void World::initTreasureValues()
{
	this->treasureCount = 0;
	this->treasureValue = 0;

	for (int i = 0; i < this->level.getWidth(); i++)
	{
		for (int j = 0; j < this->level.getHeight(); j++)
		{
			Treasure* t = this->level.getTile(sf::Vector2i(i, j)).getTreasure();
			if (t != nullptr)
			{
				this->treasureCount++;
				this->treasureValue += t->getValue();
			}
		}
	}
}

unsigned World::getTreasureCount() const
{
	return this->treasureCount;
}

unsigned World::getTreasureValue() const
{
	return this->treasureValue;
}

unsigned World::getMonsterCount() const
{
	return this->gameState->getMonsterCount();
}

