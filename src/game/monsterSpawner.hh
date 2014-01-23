
#ifndef MB_MONSTERSPAWNER_HH
#define MB_MONSTERSPAWNER_HH

#include <SFML/System/Vector2.hpp>

class MonsterSpawner
{

	private:

		sf::Vector2i location;

		int durability;
		int damage;

	public:

		int powerLevel;

		MonsterSpawner(sf::Vector2i loc, int power) : location(loc), durability(40+power), damage(0), powerLevel(power), monsterCount(0) {}

		sf::Vector2i getLocation() const
		{
			return this->location;
		}

		void setLocation(sf::Vector2i coord)
		{
			this->location = coord;
		}

		int getDurability() const
		{
			return this->durability;
		}

		int getDamage() const
		{
			return this->damage;
		}

		void doDamage(int value)
		{
			if (value > 0) this->damage += value;
		}

		int monsterCount;
};

#endif
