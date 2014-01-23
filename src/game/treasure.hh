#ifndef MB_TREASURE_HH
#define MB_TREASURE_HH

#include <SFML/Graphics/Rect.hpp>

class Treasure
{

	public:
		enum Type
		{
			BlueGem = 0,
			YellowGem,
			PurpleGem,
			GreenGem,
			RedGem
		};

		Treasure(Type t);
		~Treasure() {}

		size_t getValue() const;
		sf::IntRect& getTextureRect();

	private:
		size_t value;
		sf::IntRect textureRect;
		Type type;

};

#endif
