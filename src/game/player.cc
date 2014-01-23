
#include <iostream>

#include "level.hh"
#include <string>
#include "player.hh"
#include "Utilities.hh"

const unsigned int 	Player::maxCount = 20;

Player::Player(unsigned int id, const sf::Color& c, const std::string& name, unsigned int money,
		const TextureContainer * textureContainer, const TextureRectContainer * textureRectContainer) :
	Character(Character::Type::Player, 7, 100, 20, 3),
	id(id),
	color(c),
	name(name),
	money(money),
	items(),
	currentItem(Item::None),
	textureRectContainer(textureRectContainer),
	stepClock(1)

{
	switch(id) {
		case(0): {
			this->standingRect = textureRectContainer->get("playerOne");
			this->step1Rect = textureRectContainer->get("playerOneStep1");
			this->step2Rect = textureRectContainer->get("playerOneStep2");
			break;
		}
		case(1): {
			this->standingRect = textureRectContainer->get("playerTwo");
			this->step1Rect = textureRectContainer->get("playerTwoStep1");
			this->step2Rect = textureRectContainer->get("playerTwoStep2");
			break;
		}
		case(2): {
			this->standingRect = textureRectContainer->get("playerThree");
			this->step1Rect = textureRectContainer->get("playerThreeStep1");
			this->step2Rect = textureRectContainer->get("playerThreeStep2");
			break;
		}
		case(3): {
			this->standingRect = textureRectContainer->get("playerFour");
			this->step1Rect = textureRectContainer->get("playerFourStep1");
			this->step2Rect = textureRectContainer->get("playerFourStep2");
			break;
		}
	}

	const sf::Texture * texture = &(textureContainer->get("player"));

	image.setTexture(texture);
	image.setTextureRect(standingRect);
	roundReset();
}


void Player::updateTexture() {
	switch (this->action) {
		case(Character::Action::Dead): return;
		case(Character::Action::Digs): break;
		case(Character::Action::MovesItem): break;
		case(Character::Action::HitsCharacter): break;
		case(Character::Action::Moves): {
			if (stepClock < 15) {
				image.setTextureRect(step1Rect);
			} else {
				image.setTextureRect(step2Rect);
				if (stepClock > 30) stepClock = 0;
			}
			stepClock++;
			break;
		}
		case(Character::Action::NoAction): {
			stepClock += 15;
			image.setTextureRect(standingRect);
			break;
		}
	}
	switch (this->movementDirection) {
	case(Direction::Up): 	image.setRotation(0.f); 	break;
	case(Direction::Down): 	image.setRotation(180.f); break;
	case(Direction::Left): 	image.setRotation(270.f); break;
	case(Direction::Right): image.setRotation(90.f);  break;
	case(Direction::None): 	break;
	}
}

void Player::roundReset()
{
	unsigned len = static_cast<unsigned>(Round::Stats::StatCount);
	for (unsigned a = 0; a < len; a++) {
		setRoundStatus(static_cast<Round::Stats>(a), 0);
	}


	health = 100;
	currentItem = Item::None;
    dead = false;

	// Clear inexistent
	for (auto iter = items.begin(); iter != items.end();)
		if (iter->second == 0)
			iter = items.erase(iter);
		else
			iter++;

	if (items.size() > 0)
		currentItem = items.begin()->first;
}

bool Player::buyItem(Item::ItemID i, unsigned int c)
{
	// Has enough money?
	if (money < c)
		return false;

	auto iter = items.find(i);
	if (iter == items.end())	// New item
		items[i] = 1;
	else if (iter->second < maxCount)	// Old item, there's room
		iter->second += 1;
	else						// Old item, capped
		return false;

	setRoundStatus(Round::MoneySpent, c);
	money -= c;
	if (currentItem == Item::None)
		cycleItem();
	return true;
}

bool Player::sellItem(Item::ItemID i, unsigned int c)
{
	auto iter = items.find(i);

	if (iter == items.end()) // Does not have current item
		return false;

	switch (iter->second)
	{
	case 0:	// Does not have current item
		items.erase(iter);
		return false;
	case 1: // Only one -> erase
		items.erase(iter);
		money += c;
		increaseRoundStatus(Round::MoneyGained, c);
		if (currentItem == i)
			cycleItem();
		return true;
	default: // More than one -> decrease
		iter->second -= 1;
		money += c;
		increaseRoundStatus(Round::MoneyGained, c);
		return true;
	}

}

unsigned int Player::getItemCount(Item::ItemID i) const
{
	auto iter = items.find(i);
	return (iter != items.end()) ? iter->second : 0;
}

// Basic "getters"
unsigned int Player::getId() const
{
	return id;
}

const sf::Color& Player::getColor() const
{
	return color;
}

const std::string& Player::getName() const
{
	return name;
}

unsigned int Player::getMoney() const
{
	return money;
}

// Game functionality
Item::ItemID Player::cycleItem()
{
	Item::ItemID prev = currentItem;
	auto p = items.find(currentItem);

	auto i = p;
	if (i != items.end() && ++i != items.end())
		for (; i != items.end(); i++)
			if (i->second > 0)
			{
				currentItem = i->first;
				break;
			}

	if (currentItem != prev)
		return currentItem;

	for (i = items.begin(); i != p; i++)
		if (i->second > 0)
		{
			currentItem = i->first;
			break;
		}

	if (currentItem == prev && getItemCount(currentItem) == 0)
		currentItem = Item::None;

	return currentItem;
}

Item::ItemID Player::getCurrentItem() const
{
	return currentItem;
}

bool Player::useItem()
{
	if (currentItem == Item::None)
		return false;

	auto iter = items.find(currentItem);

	if (iter == items.end()) // Does not have current item
	{
		cycleItem();
		return false;
	}

	switch (iter->second)
	{
	case 0:	// Does not have current item
		items.erase(iter);
		cycleItem();
		return false;
	case 1: // Only one -> erase
		items.erase(iter);
		increaseRoundStatus(Round::ItemsUsed, 1);
		return true;
	default: // More than one -> decrease
		iter->second -= 1;
		increaseRoundStatus(Round::ItemsUsed, 1);
		return true;
	}

}

void Player::setRoundStatus(Round::Stats stat, unsigned qty) {
	roundStatus[stat] = qty;
}

void Player::increaseRoundStatus(Round::Stats stat, unsigned qty) {
	auto iter = roundStatus.find(stat);
	if (iter != roundStatus.end())
		iter->second += qty;
	else
		setRoundStatus(stat, qty);
}

unsigned int Player::getRoundStatus(Round::Stats stat) const
{
	auto iter = roundStatus.find(stat);
	return (iter != roundStatus.end()) ? iter->second : 0;
}






