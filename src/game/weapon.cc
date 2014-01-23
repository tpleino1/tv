#include <SFML/System.hpp>
#include <map>

#include "weapon.hh"
#include "datatables.hh"
#include "tile.hh"
#include "level.hh"


Weapon::Weapon(
		Item::ItemID id,
		std::map<Item::ItemID, WeaponData>& table,
		State::Resources res,
		int dir,
        int playerID) :
	// initialization list
	Item(fromString<float>(res.config->GetValue("Map", "tileSize")), table[id].cost, id),
	blastRadius(table[id].blastRadius),
	direction(dir),
	power(table[id].power),
	fuse(table[id].fuse),
	eTime(0),
	blasted(false),
	moveRequest(false),
	bpType(table[id].bpType),
    playerID(playerID)
{
	// set texture and right textureRect depending on the weapon
	rect.setTexture(&(res.textures->get("items")));
	rect.setTextureRect(table[id].textureRect);
}


/* 	Update weapons elapsed time on the gamefield,
*	blast them if time has reached the fuse.
*	With grenades fuse is used with moving the grenade. */
void Weapon::update(float dt)
{
	eTime += dt;
	if (eTime > fuse)
	{
		if (ID == Item::Grenade || ID == Item::LargeGrenade)
		{
			moveRequest = true;
			eTime = 0;
		}
		else
			blasted = true;
	}
}


// Calculates blastpatterns for different blastpattern types. Other types come from 
// Level but flamethrowers Direct-type is calculated here.
std::vector<sf::Vector3i> Weapon::calculateBlastPattern(sf::Vector2i vec, Level* level)
{
	std::vector<sf::Vector3i> pattern;
	std::vector<sf::Vector2i> coords;
	switch (bpType)
	{
		case BlastPatternType::Round:
            coords = level->getRadius(vec.x, vec.y, blastRadius,  Level::RadiusType::Circle);
			break;

		case BlastPatternType::CrossType:
			coords = level->getRadius(vec.x, vec.y, blastRadius, Level::RadiusType::Cross);
            break;

        // First calculate right direction and then the actual pattern.
        case BlastPatternType::Direct:
        	int dx=0, dy=0;
        	switch (direction)
        	{	case 0: dy=-1; break;
        		case 1: dy=1; break;
        		case 2: dx=-1; break;
        		case 3: dx=1; break;
        	}
        	if (dy)
        	{
        		for (int j=vec.y+dy*blastRadius; abs(j-vec.y)>0; j-=dy)
        			if (j>=0 && j<level->getHeight())
        				for (int i=(vec.x-(abs(j-vec.y)/3)); i<vec.x+(abs(j-vec.y)/3)+1; i++)
        					if (i>=0 && i<level->getWidth())
                        		coords.push_back(sf::Vector2i(i, j));
        	}
        	else if (dx)
        	{
        		for (int i=vec.x+dx*blastRadius; abs(i-vec.x)>0; i-=dx)
        			if (i>=0 && i<level->getWidth())
        				for (int j=(vec.y-(abs(i-vec.x)/3)); j<vec.y+(abs(i-vec.x)/3)+1; j++)
        					if (j>=0 && j<level->getHeight())
                        		coords.push_back(sf::Vector2i(i, j));
        	}
            break;

	}
	// Add weapons power as third element to the coordinates
	for (auto& v : coords)
        pattern.push_back(sf::Vector3i(v.x, v.y, power));
	return pattern;
}


size_t Weapon::getBlastRadius() const
{
	return this->blastRadius;
}

int Weapon::getDirection() const
{
	return this->direction;
}


bool Weapon::isBlasted() const
{
	return this->blasted;
}






