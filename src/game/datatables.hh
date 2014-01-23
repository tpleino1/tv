#ifndef MB_DATATABLES_HH
#define MB_DATATABLES_HH

#include "ResourceId.hh"
#include "State.hh"
#include "item.hh"

#include <SFML/Graphics/Rect.hpp>

#include <map>


enum BlastPatternType
{
		Round,
		CrossType,
        Direct
};

struct WeaponData
{
	std::string			name;
	size_t 				cost;
	size_t 				power;
	int 				blastRadius;
	float 				fuse;
	BlastPatternType 	bpType;
	sf::IntRect			textureRect;
};


std::map<Item::ItemID, WeaponData> initializeWeaponData(State::Resources);


#endif //MB_DATATABLES_HH
