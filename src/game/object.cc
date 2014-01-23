#include "object.hh"


Object::Object(float tileSize) 
: rect(sf::Vector2f(tileSize, tileSize))
{
	rect.setOrigin(0.5f * rect.getSize());
}

void Object::draw(sf::RenderTarget& rt)
{
	rt.draw(rect);
}

void Object::setPosition(sf::Vector2f vec)
{
    rect.setPosition(vec.x, vec.y);
}

const sf::RectangleShape& Object::getRect() const
{
    return rect;
}