#ifndef MB_MONSTER_HH
#define MB_MONSTER_HH

#include "character.hh"
#include <string>
#include <map>
#include <SFML/Graphics.hpp>
class GameState;

class Monster : public Character {

public:

	enum ApproachMethod {
		Direct,
		Path
	};

	Monster(unsigned int slowness,
			unsigned int health,
			unsigned int diggingStrength,
			unsigned int hitDamage,
			ApproachMethod approachMethod,
			unsigned int hearingDistance,
			const std::string& name,
			const std::string& monsterImageName,
			const std::string& monsterString);
	~Monster() {}

	unsigned int clock;
	ApproachMethod approachMethod;
	unsigned int hearingDistance;
	std::string name;
	unsigned int ID;
	static GameState* gameState;

	void updateTexture();

private:
	std::map<Direction, sf::IntRect> rects;

};



class Imp : public Monster
{
public:
	Imp();
};


class Demon : public Monster
{
public:
	Demon();
};


class Wyrm : public Monster
{
public:
	Wyrm();
};

class BadAss : public Monster
{
public:
	BadAss();
};

#endif
