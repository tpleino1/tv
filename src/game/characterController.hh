/*
 * playerlogic.hh
 *
 *  Created on: 7.11.2013
 *      Author: mkostia
 */

#ifndef MB_CHARACTERCONTROLLER_HH
#define MB_CHARACTERCONTROLLER_HH

#include <vector>
#include <iostream>
#include <string>
#include <list>

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include "weapon.hh"
#include "item.hh"
#include "gameController.hh"
#include "monster.hh"
#include "player.hh"
#include "State.hh"
#include "Cheat.hh"

class Character;

class GameState;
class MonsterSpawner;
class CharacterController : public GameController
{

public:

	CharacterController (GameState* gameState, State::Resources * resources);

	~CharacterController() {
		for (auto monster : monsters){
			delete monster;
		}
	}

	void setPlayers(std::vector<Player> & players) {
		for (auto & player : players) {
			this->players.push_back(&player);
		}

	}

	void handleEvent(const sf::Event & e);
	void setPlayerSpawns(const std::vector<sf::Vector2i> & spawns);
	void setMonsterSpawns(const std::vector<sf::Vector2i> & spawns);
	void inputHandler();
	void update(float dt);
	void onExplosion(GameEvent  event);
	void onItemPicked(GameEvent  event);
	void invokeGameEvent(GameEvent  event);
	unsigned getMonsterCount() const;

private:

	void setActionFromDirection(Character & character, Direction direction);
	void updateCharacterPosition(float dt);
	Player * getPlayerFromNumber(unsigned int playerNumber);
	void onDirectionKey(int playerNumber, Direction direction);
	void switchCoordinates(Character & character, int dx, int dy);
	void moveCharacter(Character & character, Direction direction);
	void damageCharacter(int sourcePlayerNumber, Character * character, unsigned int damage);
	Direction directionFromDeltas(int dx, int dy);
	void moveRandomly(Monster & monster);
	void updateFromAction(float dt, Character & character);
	void setDeltas(Direction d, int & dx, int & dy);
	void onHitToCharacter(GameEvent event);
	bool approachClosestPlayer(Monster & monster);
	void approachClosestPlayer_directRoute(Monster & monster);
	void approachTarget(Monster & attacker, Player & target);
	void updateMonsters();
	Monster * generateMonsterForSpawner(MonsterSpawner* spawner);
	void updateSpawners();
	void checkCheats(sf::Keyboard::Key);
	void useItem(int playerNumber);
    void checkForTreasures();
	void checkForHazards();

	int clock;
	int spawnerClock;

	Item::ItemID activeItem;

	std::vector<Player*> players;
	std::vector<Monster*> monsters;
	KeyBindings::Container binds;


	const std::vector<MonsterSpawner*> * monsterSpawners;

	Level * level;
	std::list<Cheat> cheats;


};


#endif
