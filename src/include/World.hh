
#ifndef MB_WORLD_HH
#define MB_WORLD_HH

#include <vector>

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "gameController.hh"
#include "level.hh"


class World : public GameController, public sf::Drawable, public sf::Transformable
{

	private:

		float tilesize;

		Level level;

		sf::Sprite background;
		std::vector<sf::RectangleShape> tiles;

		unsigned treasureCount;
		unsigned treasureValue;

		void updateTexture(sf::Vector2i);

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		void initTreasureValues();
		void updateTreasureValues(GameEvent event);

	public:

		World(GameState* gs, float tilesize = 16.0f, unsigned w = 20, unsigned h = 20);

		Level& getLevel();
		void generateLevel(int width = 75, int height = 35, unsigned nPlayers = 2,
			Level::MapType mt = Level::MapType::Deathmatch, bool revealed = false,
			unsigned difficulty = 1, unsigned treasures = 10, int seed = 0);

		sf::FloatRect getLocalBounds() const;

		virtual void update(float dt);
		virtual void invokeGameEvent(GameEvent event);

		unsigned getTreasureCount() const;
		unsigned getTreasureValue() const;

		unsigned getMonsterCount() const;


};


#endif
