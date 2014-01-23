#ifndef MB_GAMEEVENT_HH
#define MB_GAMEEVENT_HH

#include "coordinates.hh"
#include <SFML/System/Vector3.hpp>
#include <vector>

enum EventType
{
	PlayerMoves,
	PlayerDigs,
	MonsterDigs,
	WeaponUsed,
    WeaponMoved,
	Explosion,
	ItemPicked,
	HitToCharacter,
	TerrainChanged,
	PlayerOnExit
};

enum Direction 
{
	Up,
	Down,
	Left,
	Right,
	None
};


struct GameEvent
{
public:

	const EventType eventType;

	int ID;
	int dataInt;


	GameEvent(EventType eType) : eventType(eType), ID(0), dataInt(0) {}

	GameEvent(EventType eType, int x, int y, int value) : eventType(eType), ID(0), dataInt(0)
	{
		addCoordinates(x,y,value);
	}

	GameEvent(EventType eType, std::vector<sf::Vector3i> coordinates) : eventType(eType), coords(coordinates) {}

	void addCoordinates(int x, int y, int value) {
		sf::Vector3i newCoordinates(x,y, value);
		coords.push_back(newCoordinates);
	}

	void addCoordinates(sf::Vector3i& vec)
	{
		coords.push_back(vec);
	}

	const std::vector<sf::Vector3i>& getCoordinates(){
		return this->coords;
	}

private:
	std::vector<sf::Vector3i> coords;
};


#endif
