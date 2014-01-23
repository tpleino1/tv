#ifndef MB_KEYBINDINGS_H
#define MB_KEYBINDINGS_H

#include <map>
#include <set>

#include <SFML/Window/Keyboard.hpp>

// A utility class to determine controls
// That is, what key maps to what action
// (actions map only to one key)
class KeyBindings
{
	public:

		// enum for Actions, used in both the game and in menus
		enum Action
		{
			None 		= -1,
			MoveLeft 	= 0,
			MoveRight,
			MoveUp,
			MoveDown,
			UseFirst,	// Fire / MenuKey
			UseSecond	// Cycle inventory
		};

		// Reserved keys that may not be used in customizable key sets
		static const std::set<sf::Keyboard::Key> reservedKeys;

		// Just a short hand
		typedef std::map<unsigned int, KeyBindings> Container;

		// Constructor
		explicit KeyBindings(unsigned int p = 0);

		// Use one of the default, hard-coded sets
		void usePredefined(unsigned int p);

		// Public access methods
		void clearKey(sf::Keyboard::Key k);
		void setKey(Action a, sf::Keyboard::Key k);
		sf::Keyboard::Key getKey(Action a) const;
		Action getAction(sf::Keyboard::Key k) const;

	private:
		// Most commonly we want an action based on a key
		std::map<sf::Keyboard::Key, Action> binds;
};

#endif // MB_KEYBINDINGS_H
