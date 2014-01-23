#include "ButtonGroup.hh"

#include <cassert>

#include <SFML/Window/Event.hpp>


// Parameterless constructor for completeness, do not use before setting keyBindings
ButtonGroup::ButtonGroup() :
		buttons(),
		selected(-1),
		binds(nullptr)
{ }

// Complete constructor
ButtonGroup::ButtonGroup(const KeyBindings& b) :
		buttons(),
		selected(-1),
		binds(&b)
{ }

// Basic "setter"
void ButtonGroup::setKeyBindings(const KeyBindings& b)
{
	binds = &b;
}

// Add a button end of the vector, and select if all other buttons are not selectable
void ButtonGroup::add(Button& b)
{
	buttons.push_back(&b);
	if (!hasSelection() && b.getStatus() == Button::Enabled)
		select(buttons.size() - 1);
}

// Checks whether a registered button has been pressed and acts accordingly
void ButtonGroup::handleEvent(const sf::Event& e)
{
	assert(binds != nullptr);

	// If one were to pass events to buttons, this could be done like so:
	// if (hasSelection() && buttons[selected]->isActive())
	// 	buttons[selected]->handleEvent(e);

	// As a list we need to able to move in both directions
	// and to activate the selected button
	if (e.type == sf::Event::KeyPressed)
	{
		switch (binds->getAction(e.key.code))
		{
		case KeyBindings::MoveUp:
			selectNext(false);
			break;
		case KeyBindings::MoveDown:
			selectNext(true);
			break;
		case KeyBindings::UseFirst:
			if (hasSelection())
				buttons[selected]->activate(true);
			break;
		case KeyBindings::UseSecond:	// Allow UseSecond only for non-toggled buttons
			if (hasSelection() && !buttons[selected]->isToggle())
				buttons[selected]->activate(false);
			break;
		default:
			break;
		}
	}
}

// If there's no selectable buttons, selected remains -1
bool ButtonGroup::hasSelection() const
{
	return selected >= 0;
}

// Select i:th button if selectable
void ButtonGroup::select(std::size_t i)
{
	assert(i < buttons.size()); // Out-of-bounds

	if (buttons[i]->getStatus() == Button::Enabled)
	{
		if (hasSelection())
			buttons[selected]->deselect();

		buttons[i]->select();
		selected = i;
	}

}

// Select next selectable button in the direction
// set by dir (true = larger idx), wrapping if necessary
void ButtonGroup::selectNext(bool dir)
{
	if (!hasSelection())
		return;

	std::size_t i = selected;
	do
		i = (i + buttons.size() + (dir ? 1 : -1)) % buttons.size();
	while (!buttons[i]->getStatus() == Button::Enabled);

	select(i);
}

// Publicly offered "escape" selection
void ButtonGroup::selectEnd(bool dir)
{
	int prev = selected;
	int i;

	if (dir)
	{
		for (i = buttons.size(); i > prev; i--)
		{
			select(i - 1);
			if (selected != prev)
				break;
		}
	}
	else
	{
		for (i = 0; i < prev; i++)
		{
			select(i);
			if (selected != prev)
				break;
		}
	}
}

