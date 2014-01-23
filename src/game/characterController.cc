
#include "characterController.hh"
#include "GameState.hh"
#include "tile.hh"
#include <SFML/System/Vector3.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include "player.hh"
#include "character.hh"
#include "monster.hh"
#include "State.hh"
#include "treasure.hh"


using namespace std;

GameState * Monster::gameState = nullptr;

CharacterController::CharacterController (GameState* gameState, State::Resources * resources) :
		GameController(gameState),
		clock(0),
		spawnerClock(0),
		activeItem(Item::None),
		binds(*(resources->keyBinds)),
		cheats()
	{
		this->level = &gameState->getLevel();
		monsterSpawners = &(level->getSpawners());
		Monster::gameState = gameState;

		// Cheats
		cheats.push_back(Cheat("MAP", [gameState] ()
		{
			gameState->getLevel().revealMap();
		} ) );

		cheats.push_back(Cheat("HEAL", [this] ()
		{
			for (auto p : players)
				if (p->getHealth() > 0)
					p->health = 100;
		} ) );

		cheats.push_back(Cheat("WEAK", [this] ()
		{
			for (auto m : monsters)
				if (m->getHealth() > 0)
					m->health = 1;
		} ) );
	}



void CharacterController::update(float dt) {
	try {

		// check treasures
        checkForTreasures();
		checkForHazards();

		inputHandler();
		updateCharacterPosition(dt);

		clock++;

		updateMonsters();
		if (clock % 1500 == 0) updateSpawners();
		if (clock == 1000000) clock = 0;
	} catch (std::invalid_argument e) {
		std::cout << e.what() << std::endl;
	}
}
void CharacterController::invokeGameEvent(GameEvent event) {
	switch (event.eventType)  {

	case (EventType::Explosion): onExplosion(event); break;
	case (EventType::PlayerMoves): break;
	case (EventType::PlayerDigs): break;
	case (EventType::WeaponUsed): break;
	case (EventType::ItemPicked): break;
	case (EventType::WeaponMoved): break;
	case (EventType::HitToCharacter): onHitToCharacter(event); break;
	default: break;

	}
}

void CharacterController::onExplosion(GameEvent event) {
	vector<sf::Vector3i> coordVec= event.getCoordinates();
	int damage = 0;
	Character * charptr = nullptr;
	for (sf::Vector3i & coords : coordVec) {
		damage = coords.z;
		Tile & tile = gameState->getTile(sf::Vector2i(coords.x, coords.y));
		charptr = tile.getCharacter();
		if (charptr) {
			damageCharacter(event.dataInt, charptr, damage);
		}
	}
}


void CharacterController::onHitToCharacter(GameEvent event) {
	vector<sf::Vector3i> vec = event.getCoordinates();
	Tile & tile = gameState->getTile(sf::Vector2i(vec[0].x, vec[0].y));
	Character * character = tile.getCharacter();
	if (character)
		damageCharacter(event.dataInt, character, vec[0].z);
}


void CharacterController::handleEvent(const sf::Event & e){
	if (e.type == sf::Event::KeyPressed){
		for(unsigned int i = 0; i < players.size(); i++){
			if (binds[i+1].getAction(e.key.code) == KeyBindings::UseFirst)
				useItem(i);
			else if (binds[i+1].getAction(e.key.code) == KeyBindings::UseSecond)
				players[i]->cycleItem();
		}
		checkCheats(e.key.code);
	}
}


void CharacterController::setMonsterSpawns(const vector<sf::Vector2i> & spawns) {
	for (unsigned i = 0; i < spawns.size(); i++){
		Imp * monster = new Imp();
		monster->x = spawns[i].x;
		monster->y = spawns[i].y;
		monsters.push_back(monster);
		Tile & tile = gameState->getTile(sf::Vector2i(spawns[i].x, spawns[i].y));
		tile.setCharacter(monsters[i]);
	}

}

void CharacterController::setPlayerSpawns(const vector<sf::Vector2i> & spawns)
{
		if (spawns.size() != players.size()) throw runtime_error("player count != spawn point count");
		for (unsigned i = 0; i < spawns.size(); i++){
			players[i]->x = spawns[i].x;
			players[i]->y = spawns[i].y;
			Tile & tile = gameState->getTile(sf::Vector2i(spawns[i].x, spawns[i].y));
			tile.setCharacter(players[i]);
			gameState->callGameEvent(GameEvent(EventType::PlayerMoves,players[i]->x, players[i]->y, 0));
		}

}


/** PRIVATE **/

void CharacterController::updateCharacterPosition(float dt){
	for (auto * player : players) {
		if (!player->dead) updateFromAction(dt, *player);
	}
	for (auto * monster : monsters) {
		if (!monster->dead) updateFromAction(dt, *monster);
	}

}

void CharacterController::updateFromAction(float, Character & character){

	character.updateTexture();
	if (character.action == Character::Action::NoAction) return;
	//cout << "update from action: " << endl;

	int dx, dy;
	setDeltas(character.movementDirection, dx, dy);

	character.clock++;

	switch (character.action) {
		case(Character::Action::NoAction): break;
		case(Character::Action::MovesItem): {
			if (character.clock > character.slowness) {
				gameState->callGameEvent(GameEvent(EventType::WeaponMoved, character.x, character.y, character.movementDirection));
				character.action = Character::Action::NoAction;
				break;
			}
		}
		case(Character::Action::Dead): break;
		case(Character::Action::HitsCharacter): {
			if (character.clock > 15) {
				character.clock = 0;
				character.action = Character::Action::NoAction;
				GameEvent event(EventType::HitToCharacter, character.x+dx, character.y+dy, character.hitDamage);
				event.dataInt = character.hitDamage;
				gameState->callGameEvent(event);
			}
			break;
		}
		case(Character::Action::Digs): {
			if (character.clock > 2) {
				character.clock = 0;
				character.action = Character::Action::NoAction;
				if (character.type == Character::Type::Monster)
					gameState->callGameEvent(GameEvent(EventType::MonsterDigs, character.x+dx, character.y+dy, character.diggingStrength));
				if (character.type == Character::Type::Player)
					gameState->callGameEvent(GameEvent(EventType::PlayerDigs, character.x+dx, character.y+dy, character.diggingStrength));
			}
				break;
		}
		case(Character::Action::Moves): {
			if (character.action == Character::Action::Moves){
				if (character.clock > character.slowness) {
					moveCharacter(character, character.movementDirection);
					character.clock = 0;
					character.action = Character::Action::NoAction;
					//character.currentAxis = character.nextAxis;
					//character.nextAxis = Character::Axis::None;
				}
			}
			break;
		}
	}
}

void CharacterController::moveCharacter(Character & character, Direction direction) {

	int dx,dy;
	setDeltas(direction, dx,dy);
	if (direction != Direction::None)
		switchCoordinates(character, dx,dy);

}
void CharacterController::switchCoordinates(Character & character, int dx, int dy){


	Tile & oldTile = gameState->getTile(sf::Vector2i(character.x, character.y));
	Tile & newTile = gameState->getTile(sf::Vector2i(character.x+dx, character.y+dy));

	if ((newTile.hasExit()) && (character.type == Character::Type::Player))
		gameState->callGameEvent(GameEvent(EventType::PlayerOnExit, character.x+dx, character.y+dy, 0));


	if (newTile.getCharacter() && !newTile.getCharacter()->dead) return;

	if (character.type == Character::Type::Player)
		gameState->callGameEvent(GameEvent(EventType::PlayerMoves,character.x+dx, character.y+dy, 0));

	newTile.setCharacter(oldTile.getCharacter());
	oldTile.setCharacter(nullptr);

	character.changeCoordinates(dx, dy);
}


void CharacterController::inputHandler() {
	const std::map<KeyBindings::Action, Direction> trans({
			{ KeyBindings::MoveLeft, 	Direction::Left },
			{ KeyBindings::MoveRight, 	Direction::Right },
			{ KeyBindings::MoveUp, 		Direction::Up },
			{ KeyBindings::MoveDown, 	Direction::Down }
		});

	for (unsigned int i = 0; i < players.size(); i++)
		for (auto& e : trans)
			if (sf::Keyboard::isKeyPressed(binds[i+1].getKey(e.first)))
				onDirectionKey(i, e.second);
}

void CharacterController::onDirectionKey(int playerNumber, Direction direction){
	Player * player = getPlayerFromNumber(playerNumber);
	if (player->dead) return;
	setActionFromDirection(*player, direction);
}

void CharacterController::setActionFromDirection(Character & character, Direction direction){

	int dx = 0, dy = 0;
	setDeltas(direction,dx,dy);
	if (!gameState->getLevel().validCoordinate(sf::Vector2i(character.x+dx, character.y+dy))) return;


	Tile & tile = gameState->getTile(sf::Vector2i(character.x+dx, character.y+dy));
	Character * charInFront = tile.getCharacter();
	if (charInFront) {
		if (!charInFront->dead) {
			character.movementDirection = direction;
			if (character.type == Character::Type::Monster)
				if (charInFront->type == Character::Type::Player)
					character.action = Character::Action::HitsCharacter;
			if (character.type == Character::Type::Player) {
				if (charInFront->type == Character::Type::Monster)
					character.action = Character::Action::HitsCharacter;
				else if (charInFront->type == Character::Type::Player)
					if (gameState->getLevel().getMapType() != Level::MapType::Campaign)
						character.action = Character::Action::HitsCharacter;
			}
			return;
		}
	}

	if (tile.hasItem()) {
		character.movementDirection = direction;
		character.action = Character::Action::MovesItem;
		return;
	};

	switch (tile.getTerrainType()) {

		case (TerrainType::Solid): {
			character.movementDirection = direction;
			character.action = Character::Action::Digs;
			break;
		}
		case (TerrainType::Hazard):
		case (TerrainType::Empty): {
				character.movementDirection = direction;
				character.action = Character::Action::Moves;
				break;
		}
		case (TerrainType::Indestructible): break;
		case (TerrainType::Void): break;
		default: break;
	}

}


void CharacterController::useItem(int playerNumber){
	Player* player = getPlayerFromNumber(playerNumber);
	if (player->dead || level->getTile(sf::Vector2i(player->x, player->y)).getTerrainType() != TerrainType::Empty || level->getWeapon(sf::Vector2i(player->x, player->y)) || !player->useItem()) return;
	GameEvent event(EventType::WeaponUsed);
	event.ID = player->getCurrentItem();
	event.dataInt = playerNumber;
	event.addCoordinates(player->x, player->y, player->movementDirection);
	gameState->callGameEvent(event);
}

void CharacterController::damageCharacter(int sourcePlayerNumber, Character * target, unsigned int damage) {
	Player * source = getPlayerFromNumber(sourcePlayerNumber);

	if ((source) && (source != target)) source->increaseRoundStatus(Round::Stats::DamageGiven, damage);
	target->health = damage > target->health ? 0 : target->health - damage;

	if (target->health == 0) {
		if ((source) && (source != target))
			{
				source->increaseRoundStatus(Round::Stats::Kills, 1);

				//cout << "player has dealt damage: " << source->getRoundStatus(Round::Stats::DamageGiven) << endl;

			}

		target->health = 0;
		target->dead = true;
		Tile & tile = gameState->getTile(sf::Vector2i(target->x, target->y));
		tile.setCharacter(nullptr);

	}
}
void CharacterController::updateMonsters(){

	for (auto * monster  : monsters) {
		if (monster->action == Character::Action::NoAction) {
			if (!approachClosestPlayer(*monster))
				moveRandomly(*monster);
		}
	}
}


bool CharacterController::approachClosestPlayer(Monster & monster) {
	int distance = 0;
	float smallestDistance = numeric_limits<float>::max();
	vector<sf::Vector2i> path;
	vector<sf::Vector2i> closestPath;
	for (auto * player : players) {
		if (player->dead) continue;
		if (monster.approachMethod == Monster::ApproachMethod::Direct) {
			path = level->getPath(sf::Vector2i(monster.x, monster.y), sf::Vector2i(player->x, player->y), Level::PathType::Open, monster.hearingDistance);
			if (path.size() == 0) {
				path = level->getPath(sf::Vector2i(monster.x, monster.y), sf::Vector2i(player->x, player->y), Level::PathType::Destructible, monster.hearingDistance);
			}
		}

		if (monster.approachMethod == Monster::ApproachMethod::Path)
			path = level->getPath(sf::Vector2i(monster.x, monster.y), sf::Vector2i(player->x, player->y), Level::PathType::Open, monster.hearingDistance);

		distance = path.size();

		if (distance > 0) {
			if (distance < smallestDistance) {
				closestPath = path;
				smallestDistance = distance;
			}
		}
	}
	if (closestPath.size() > 1) {
		sf::Vector2i coords = closestPath[1];
		Direction direction = directionFromDeltas(coords.x-monster.x, coords.y-monster.y);
		setActionFromDirection(monster, direction);
		return true;
	}
	return false;
}


void CharacterController::moveRandomly(Monster & monster) {
	Direction direction = static_cast<Direction>(rand() % Direction::None);
	int dx, dy;
	setDeltas(direction, dx, dy);
	if (!gameState->getLevel().validCoordinate(sf::Vector2i(monster.x+dx, monster.y+dy))) return;
	Tile & tile = gameState->getTile(sf::Vector2i(monster.x+dx, monster.y+dy));

	if (tile.getTerrainType() == TerrainType::Empty) {
		monster.movementDirection = direction;

		if (tile.getWeapon()) monster.action = Character::Action::MovesItem;
		else monster.action = Character::Action::Moves;
	}
}

Monster * CharacterController::generateMonsterForSpawner(MonsterSpawner* spawner) {
	int diff = rand() % 100;
	int diffFactor = spawner->powerLevel;
	Monster * monster = nullptr;
	//cout << "diff at: " << diffFactor << endl;
	if (diff > 99-diffFactor) monster = new BadAss();
	else if (diff > 75-diffFactor) monster = new Demon();
	else if (diff > 60-diffFactor)  monster = new Wyrm();

	if (!monster) monster = new Imp();
	return monster;
}

void CharacterController::updateSpawners()
{

	spawnerClock++;
	if (spawnerClock == 1000000) spawnerClock = 0;

	MonsterSpawner * spawner;
	vector<MonsterSpawner*>::size_type size = monsterSpawners->size();
	for (unsigned a = 0; a < size; a++) {
		spawner =  (*monsterSpawners)[a];
		sf::Vector2i location = spawner->getLocation();

		if (spawner->monsterCount < 4) {
			Tile & tile = gameState->getTile(spawner->getLocation());
			if (!tile.getCharacter()) {

				Monster * monster = generateMonsterForSpawner(spawner);
				monster->x = location.x;
				monster->y = location.y;

				tile.setCharacter(monster);
				monsters.push_back(monster);
				spawner->monsterCount++;

			}
		}
	}
}




/**********************************************************/

Player * CharacterController::getPlayerFromNumber(unsigned int playerNumber){

	for (auto * player : players) {
		if (player->getId() == playerNumber) {
			return player;
		}
	}
	return nullptr;
	//throw std::runtime_error("No players found with given number!");
}


void CharacterController::setDeltas(Direction direction, int & dx, int & dy){
	switch (direction) {
		case (Direction::Up): dx = 0; dy = -1; break;
		case (Direction::Down): dx = 0; dy = 1; break;
		case (Direction::Left): dx = -1; dy = 0; break;
		case (Direction::Right): dx = 1; dy = 0; break;
		case (Direction::None): dx = 0; dy = 0; break;
	}
 }

Direction CharacterController::directionFromDeltas(int dx, int dy){
	if ((dx == 0) && (dy == -1)) return Direction::Up;
	if ((dx == 0) && (dy == 1)) return Direction::Down;
	if ((dx == -1) && (dy == 0)) return Direction::Left;
	if ((dx == 1) && (dy == 0)) return Direction::Right;
		return Direction::None;
	throw runtime_error("Incorrect deltas in characterController::directionFromDeltas");

}

void CharacterController::checkCheats(sf::Keyboard::Key key)
{
	for (auto& c : cheats)
		c.checkKey(key);
}

unsigned CharacterController::getMonsterCount() const
{
	unsigned count = 0;
	for (Monster* m : this->monsters)
	{
		if (m && !m->dead) count++;
	}
	return count;
}

// function to pick treasures up
void CharacterController::checkForTreasures()
{
    for (auto& player : players)
        if (player) 
        	if (level->getTreasure(sf::Vector2i(player->x, player->y)))
        	{
	        	unsigned money = level->pickTreasure(sf::Vector2i(player->x, player->y));
	        	gameState->callGameEvent(GameEvent(EventType::ItemPicked, player->x, player->y, money));
	        	player->money += money;
	        	player->increaseRoundStatus(Round::Stats::MoneyGained, money);
	        }
}

void CharacterController::checkForHazards()
{
    for (auto& player : players)
	{
        if (player) {
			Tile& tile = level->getTile(sf::Vector2i(player->x, player->y));
        	if (tile.getTerrainType() == TerrainType::Hazard)
       		{
				if (tile.getTerrain() == TerrainStorage::getTerrain("Lava"))
				{
					player->health -= 1;
				}
			}
        }
	}
	for (auto& monster : monsters)
	{
        if (monster) {
			Tile& tile = level->getTile(sf::Vector2i(monster->x, monster->y));
        	if (tile.getTerrainType() == TerrainType::Hazard)
       		{
				if (tile.getTerrain() == TerrainStorage::getTerrain("Lava"))
				{
					monster->health -= 1;
				}
			}
        }
	}
}
