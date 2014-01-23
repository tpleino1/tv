#include "effect.hh"



Effect::Effect(State::Resources res) 
: Object(fromString<float>(res.config->GetValue("Map", "tileSize"))), eTime(0.f), fuse(0.5f), over(false), offset(0)
{
	rect.setTexture(&(res.textures->get("explosion")));
	rect.setOrigin(0.5f * rect.getSize());
}

void Effect::update(float dt)
{
	eTime += dt;
	if (eTime > (fuse / (float)(25-offset)))
		moveTexture();
	if (eTime > fuse)
		over = true;
}

void Effect::moveTexture()
{
	offset++;
	rect.setTextureRect(sf::IntRect((offset%5)*205, (offset/5)*205, 205, 205));
}

bool Effect::isOver() const
{
	return over;
}