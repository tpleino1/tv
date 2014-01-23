#include "character.hh"

Character::Character(Type type, unsigned int slowness, unsigned int health, unsigned int diggingStrength, unsigned int hitDamage) :
		image(sf::Vector2f(16.f, 16.f)),
		type(type),
		x(5), y(5),
		movementDirection(Direction::None),
		clock(0),
		slowness(slowness),
		health(health),
		diggingStrength(diggingStrength),
		hitDamage(hitDamage),
		dead(false),
		action(Character::Action::NoAction)

	{
		image.setOrigin(0.5f * image.getSize());
	}

	void Character::setSpawnPoint(Coordinate coords){
		this->x = coords.x;
		this->y = coords.y;
	}

	void Character::changeCoordinates(int dx, int dy){
		this->x += dx;
		this->y += dy;
	}

	unsigned int Character::getHealth() const
	{
		return health;
	}

