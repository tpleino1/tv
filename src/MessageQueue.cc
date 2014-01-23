#include "MessageQueue.hh"

#include <iostream>
#include <string>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

// Static consts
const sf::Time MessageQueue::defaultDuration = sf::seconds(2.0);
const bool MessageQueue::copyToStdOut = false;

// Parameterless constructor
MessageQueue::MessageQueue() :
		msgs(),
		font(nullptr),
		color(sf::Color::White),
		charSize(20u),
		anchor(Anchor::TopLeft),
		margin(sf::Vector2f(0.f, 20.f)),
		fadeTime(sf::seconds(0.5))
{ }

// Constructor
MessageQueue::MessageQueue(const sf::Font& f,
	const sf::Color& c,
	unsigned int s,
	Anchor::ID a,
	const sf::Vector2f& m,
	const sf::Time& t) :
		msgs(),
		font(&f),
		color(c),
		charSize(s),
		anchor(a),
		margin(m),
		fadeTime(t)
{ }

/*
// Destructor
MessageQueue::~MessageQueue()
{ }
*/

// Basic "setter"
void MessageQueue::setFont(const sf::Font& f)
{
	font = &f;
}

// Basic "setter"
void MessageQueue::setColor(const sf::Color& c)
{
	color = c;
}

// Basic "setter"
void MessageQueue::setCharSize(unsigned int s)
{
	charSize = s;
}

// Basic "setter"
void MessageQueue::setAnchor(Anchor::ID a)
{
	anchor = a;
}

// Basic "setter"
void MessageQueue::setMargin(const sf::Vector2f& m)
{
	margin = m;
}

// Basic "setter"
void MessageQueue::setFadeTime(const sf::Time& t)
{
	fadeTime = t;
}

// Use default text settings to create a new Text and add that
void MessageQueue::addString(const std::string& s, const sf::Time& d)
{
	sf::Text t(s, *font, charSize);
	t.setColor(color);
	addText(t, d);
}

// Add Text (includes font, size, color)
void MessageQueue::addText(const sf::Text& t, const sf::Time& d)
{
	// Negative duration uses default duration
	// Zero-duration may be used to show messages only in std::cout
	if (d != sf::Time::Zero)
	{
		// Find new position
		sf::Vector2f nextPos(0.f, 0.f);
		if (!msgs.empty())
			nextPos = msgs.back().first.getPosition() + margin +
					sf::Vector2f(0.f, (margin.y < 0 ? -1 : 1) * msgs.back().first.getLocalBounds().height);

		// Add to queue and apply position
		msgs.push_back(std::make_pair(sf::Text(t), sf::Time(d > sf::Time::Zero ? d : defaultDuration)));
		sf::Text& newText = msgs.back().first;
		changeAnchor<sf::Text>(newText, anchor, Anchor::TopLeft);
		newText.setPosition(nextPos);
	}

	// Copy messages to std::cout
	if (copyToStdOut)
	{
		const std::string& s = t.getString();
		std::cout << s << std::endl;
	}
}

// Clear messages in queue
void MessageQueue::clear()
{
	msgs.clear();
}

// Process queue with a time step of dt
// (i.e., decrease duration, fade, remove from queue)
void MessageQueue::update(const sf::Time& dt)
{
	sf::Vector2f nextPos(0.f, 0.f);
	for (auto iter = msgs.begin(); iter != msgs.end(); )
		if (iter->second > sf::Time::Zero) 	// Decrease duration & move if necessary
		{
			if (iter->second < fadeTime)	// Fade
			{
				sf::Color c = iter->first.getColor();
				c.a = 255 * (iter->second.asSeconds() / fadeTime.asSeconds());
				iter->first.setColor(c);
			}
			iter->second -= dt;
			iter->first.setPosition(nextPos);
			nextPos += margin + sf::Vector2f(0.f, (margin.y < 0 ? -1 : 1) * iter->first.getLocalBounds().height);
			iter++;
		}
		else // Remove from list
			iter = msgs.erase(iter);
}

// Render
void MessageQueue::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	for (auto& m : msgs)
		target.draw(m.first, states);
}
