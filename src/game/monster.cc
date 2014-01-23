#include "monster.hh"
#include "GameState.hh"

Monster::Monster(
		unsigned int slowness,
		unsigned int health,
		unsigned int diggingStrength,
		unsigned int hitDamage,
		ApproachMethod approachMethod,
		unsigned int hearingDistance,
		const std::string& name,
		const std::string& monsterImageName,
		const std::string& monsterString) :
	Character(Character::Type::Monster, slowness, health, diggingStrength, hitDamage),
	clock(0),
	approachMethod(approachMethod),
	hearingDistance(hearingDistance),
	name(name),
	rects()
{
	State::Resources gameResources = gameState->getResources();
	static unsigned int runningID = 0;		// <- what?
	ID = runningID++;

	rects[Direction::Up] 	= gameResources.textureRects->get(monsterString + "up");
	rects[Direction::Down] 	= gameResources.textureRects->get(monsterString + "down");
	rects[Direction::Left] 	= gameResources.textureRects->get(monsterString + "left");
	rects[Direction::Right] = gameResources.textureRects->get(monsterString + "right");

	const sf::Texture * texture = &(gameResources.textures->get(monsterImageName));

	image.setTexture(texture);
	image.setTextureRect(rects[Direction::Down]);
}

void Monster::updateTexture()
{
	auto iter = rects.find(this->movementDirection);
	if (iter != rects.end())
		image.setTextureRect(iter->second);
}

/* Slowness, health, diggingStrength, hitDamage, ApproachMethod, hearingDistance, name, filename, configString */

Imp::Imp() 			: Monster(15, 20, 	0, 	10, ApproachMethod::Path, 	10, "imp", 			"monsters1", 	"monster1") { }
Demon::Demon() 		: Monster(20, 50, 	0, 	25, ApproachMethod::Path, 	30, "demon", 		"monsters1", 	"monster2") { }
Wyrm::Wyrm() 		: Monster(10, 10, 	10, 10, ApproachMethod::Direct, 20, "wyrm", 			"monsters2", 	"monster3") { }
BadAss::BadAss() 	: Monster(80, 100, 50, 75, ApproachMethod::Direct, 45, "big bad monster", 	"monsters1", 	"badass") 	{ }


