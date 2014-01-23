#include "StatMeter.hh"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>

#include "Utilities.hh"

// Update every ___ seconds
const sf::Time StatMeter::smoothing(sf::seconds(0.5));

// Constructor
StatMeter::StatMeter() :
	text(), time(sf::Time::Zero), frames(0)
{
	text.setCharacterSize(10u);
	text.setPosition(4.f, 4.f);
}

// Destructor
StatMeter::~StatMeter()
{ }

// Basic "setter"
void StatMeter::setColor(const sf::Color& c)
{
	text.setColor(c);
}

// Basic "setter"
void StatMeter::setFont(const sf::Font& f)
{
	text.setFont(f);
}

// Ran every iteration, calculate in bursts with a length of smoothing
void StatMeter::update(const sf::Time& dt)
{
	time += dt;
	frames += 1;
	if (time >= smoothing)
	{
		text.setString("FPS: " + toString<float>(frames / time.asSeconds(), 3));
		time -= smoothing;
		frames = 0;
	}
}

// Render: apply transformation and draw
void StatMeter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(text);
}
