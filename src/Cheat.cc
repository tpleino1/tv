#include "Cheat.hh"

#include "Utilities.hh"

Cheat::Cheat() :
	keys(),
	press(0),
	call()
{ }

Cheat::Cheat(const std::vector<sf::Keyboard::Key>& k, Callback c) :
	keys(k),
	press(0),
	call()
{
	setCallback(c);
}

Cheat::Cheat(const std::string& k, Callback c) :
	keys(),
	press(0),
	call()
{
	setCallback(c);
	setKeys(k);
}

void Cheat::setKeys(const std::vector<sf::Keyboard::Key>& k)
{
	keys = k;
}

void Cheat::setKeys(const std::string& k)
{
	keys.clear();
	keys.reserve(k.length());
	sf::Keyboard::Key t;
	for (auto& c : k)
	{
		t = fromString<sf::Keyboard::Key>(toString(c));
		if (t != sf::Keyboard::Unknown)
			keys.push_back(t);
	}
	press = 0;
}

void Cheat::setCallback(Callback c)
{
	call = std::move(c);
}

bool Cheat::checkKey(sf::Keyboard::Key k)
{
	if (k == keys[press])
	{
		if (++press == keys.size())
		{
			press = 0;
			if (call)
				call();
		}
		return true;
	}

	press = 0;
	return false;
}
