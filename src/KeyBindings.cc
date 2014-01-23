#include "KeyBindings.hh"

#include <cassert>

// Reserved keys that may not be used in customizable binds
const std::set<sf::Keyboard::Key> KeyBindings::reservedKeys =
{
	sf::Keyboard::Escape,	// Pause
	sf::Keyboard::F12		// Take screenshot
};

// Constructor
KeyBindings::KeyBindings(unsigned int p) :
		binds()
{
	usePredefined(p);
}

// Use a hard-coded set
void KeyBindings::usePredefined(unsigned int p)
{
	binds.clear(); // May be called on top of an existing set

	switch(p)
	{
	case 1:
		binds[sf::Keyboard::Left] 		= MoveLeft;
		binds[sf::Keyboard::Right] 		= MoveRight;
		binds[sf::Keyboard::Up] 		= MoveUp;
		binds[sf::Keyboard::Down] 		= MoveDown;
		binds[sf::Keyboard::RControl] 	= UseFirst;
		binds[sf::Keyboard::RShift] 	= UseSecond;
		break;

	case 2:
		binds[sf::Keyboard::A] 			= MoveLeft;
		binds[sf::Keyboard::D] 			= MoveRight;
		binds[sf::Keyboard::W] 			= MoveUp;
		binds[sf::Keyboard::S] 			= MoveDown;
		binds[sf::Keyboard::LControl] 	= UseFirst;
		binds[sf::Keyboard::LShift] 	= UseSecond;
		break;

	case 3:
		binds[sf::Keyboard::J] 			= MoveLeft;
		binds[sf::Keyboard::L] 			= MoveRight;
		binds[sf::Keyboard::I] 			= MoveUp;
		binds[sf::Keyboard::K] 			= MoveDown;
		binds[sf::Keyboard::B] 			= UseFirst;
		binds[sf::Keyboard::N] 			= UseSecond;
		break;

	case 4:
		binds[sf::Keyboard::Numpad3]	= MoveLeft;
		binds[sf::Keyboard::Numpad1] 	= MoveRight;
		binds[sf::Keyboard::Numpad5] 	= MoveUp;
		binds[sf::Keyboard::Numpad2] 	= MoveDown;
		binds[sf::Keyboard::Multiply] 	= UseFirst;
		binds[sf::Keyboard::Subtract] 	= UseSecond;
		break;

	default: // includes Menu
		binds[sf::Keyboard::Left] 		= MoveLeft;
		binds[sf::Keyboard::Right] 		= MoveRight;
		binds[sf::Keyboard::Up] 		= MoveUp;
		binds[sf::Keyboard::Down] 		= MoveDown;
		binds[sf::Keyboard::Return] 	= UseFirst;
		binds[sf::Keyboard::Space] 		= UseSecond;
		break;
	}
}

// Frees a key (in case of "cross binding")
void KeyBindings::clearKey(sf::Keyboard::Key k)
{
	binds.erase(k);
}

// Pairs Key and Action together
void KeyBindings::setKey(Action a, sf::Keyboard::Key k)
{
	assert(a != None);
	assert(reservedKeys.find(k) == reservedKeys.end());

	// Allow only one key per action
	for (auto iter = binds.begin(); iter != binds.end(); )
	{
		if (iter->second == a)
			iter = binds.erase(iter); // C++11: iterator map::erase(const_iterator position);
		else
			iter++;
	}

	binds[k] = a; // Overwrite previous bind (if any)
}

// "Inverse retrieval", used in Options
sf::Keyboard::Key KeyBindings::getKey(Action a) const
{
	assert(a != None);

	for (auto iter = binds.begin(); iter != binds.end(); iter++)
		if (iter->second == a)
			return iter->first;

	return sf::Keyboard::Unknown;
}

// Default retrieval: Corresponding Action or None
KeyBindings::Action KeyBindings::getAction(sf::Keyboard::Key k) const
{
	auto b = binds.find(k);
	return ((b != binds.end()) ? b->second : None);
}
