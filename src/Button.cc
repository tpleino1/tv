#include "Button.hh"

#include <memory>	// move

#include <SFML/Graphics/Color.hpp>

Button::Button() : Text(), colors(nullptr)
{ }

Button::Button(const std::string& string, const sf::Font& font, const ColorContainer& c) :
		Text(string, font), selected(false), active(false), toggled(false),
		stat(Status::Enabled), primaryCallback(), secondaryCallback(), colors(&c)
{
	changeColor();
}

/*
Button(const Button& b)
{ }
Button& operator= (const Button& b)
{ }
Button::~Button()
{ }
*/

// Basic "getter"
bool Button::isSelected() const
{
	return selected;
}

// Basic "setter" with "update"
void Button::select()
{
	selected = true;
	changeColor();
}

// Basic "setter" with "update"
void Button::deselect()
{
	selected = false;
	changeColor();
}

// Basic "getter"
bool Button::isActive() const
{
	return active;
}

// Activate button
void Button::activate(bool p)
{
	// Toggled button should change its color and deactivate if already activ
	if (toggled)
	{
		if (active)
			deactivate();
		else
		{
			active = true;
			changeColor();
		}
	}
	else
		active = true;

	// Call to callback if set
	if (p && primaryCallback)
		primaryCallback();
	else if (!p && secondaryCallback)
		secondaryCallback();

	// Non-toggled buttons deactivate immediately
	if (!toggled)
		deactivate();
}

// Deactivate button
void Button::deactivate()
{
	active = false;
	if (toggled)
		changeColor();
}

// Basic "getter"
bool Button::isToggle() const
{
	return toggled;
}

// Basic "setter"
void Button::setToggle(bool t)
{
	toggled = t;
}

// Basic "getter"
Button::Status Button::getStatus() const
{
	return stat;
}

// Basic "setter" with "update"
void Button::setStatus(Status s)
{
	stat = s;
	changeColor();
}

// Move is needed since lambdas disappear if not saved properly
void Button::setCallback(Callback cb, bool p)
{
	if (p)
		primaryCallback = std::move(cb);
	else
		secondaryCallback = std::move(cb);
}

// Basic "setter"
void Button::setColors(const ColorContainer& c)
{
	colors = &c;
}

// Buttons need not handle events,
// this is here just for possible future expansion
void Button::handleEvent(const sf::Event&)
{
	// pass
}

// Use color to correspond to current state of the button
void Button::changeColor()
{
	sf::Color c = sf::Color(0, 0, 0, 0);	// Transparent

	switch(stat)
	{
	case Status::Enabled:	// Different colors
		if (active && selected)
			c = colors->get("buttonActiveSelected");
		else if (active)
			c = colors->get("buttonActive");
		else if (selected)
			c = colors->get("buttonSelected");
		else
			c = colors->get("buttonEnabled");
		break;

	case Status::Disabled:	// Partly transparent
		c = colors->get("buttonDisabled");
		break;

	default:	// Hidden
		break;
	}

	setColor(c);
}
