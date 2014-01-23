#ifndef MB_CHARACTER_HH
#define MB_CHARACTER_HH

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "gameEvent.hh"
#include <iostream>


class Character
{

public:

	enum Action {
		NoAction,
		Moves,
		Digs,
		MovesItem,
		HitsCharacter,
		Dead
	};

	enum Type {
		Player,
		Monster
	};

	Character(Type type, unsigned int slowness, unsigned int health, unsigned int diggingStrength, unsigned int hitDamage);
	virtual ~Character() {}

	sf::RectangleShape image;
	Type type;

	int x;
	int y;
	Direction movementDirection;
	float clock;

	unsigned int slowness;
	unsigned int health;
	unsigned int diggingStrength;
	unsigned int hitDamage;
	bool dead;

	Character::Action action;

	void setSpawnPoint(Coordinate coords);

	void changeCoordinates(int dx, int dy);

	unsigned int getHealth() const;

	virtual void updateTexture() {}


};

#endif
