#ifndef MB_CHEAT_HH
#define MB_CHEAT_HH

#include <vector>
#include <functional>
#include <string>

#include <SFML/Window/Keyboard.hpp>

// A utility class to facilitate cheating
class Cheat
{
	public:
        typedef std::function<void()> Callback;

		Cheat();
		Cheat(const std::vector<sf::Keyboard::Key>& k, Callback c);
		Cheat(const std::string& k, Callback c);

		void setKeys(const std::vector<sf::Keyboard::Key>& k);
		void setKeys(const std::string& k);
		void setCallback(Callback c);

		bool checkKey(sf::Keyboard::Key k);

	private:
		std::vector<sf::Keyboard::Key> 	keys;
		unsigned int 					press;
		Callback 						call;
};

#endif // MB_CHEAT_HH
