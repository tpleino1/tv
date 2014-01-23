#include "treasure.hh"

Treasure::Treasure(Treasure::Type t) : type(t)
{
	value = (type+1)*50;
	textureRect = sf::IntRect(type*64, 0, 64, 64);
}

size_t Treasure::getValue(void) const
{
	return value;
}

sf::IntRect& Treasure::getTextureRect(void)
{
	return textureRect;
}
