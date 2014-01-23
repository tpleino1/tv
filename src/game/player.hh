#ifndef MB_PLAYER_HH
#define MB_PLAYER_HH


#include <string>
#include <map>

#include <SFML/Graphics.hpp>

#include "ResourceId.hh"
#include "character.hh"
#include "coordinates.hh"
#include "item.hh"
#include "Round.hh"

class Player : public Character {

public:


	Player(unsigned int id, const sf::Color& c, const std::string& name, unsigned int money,
			const TextureContainer * textureContainer, const TextureRectContainer * textureRectContainer);
	~Player() {}



	friend class CharacterController;

	void roundReset(); // Ran in the beginning of each round, after shop

	// Basic "getters"
	unsigned int 		getId() const;
	const sf::Color& 	getColor() const;
	const std::string& 	getName() const;
	unsigned int 		getMoney() const;

	// Game functionality
	Item::ItemID 	cycleItem();
	Item::ItemID 	getCurrentItem() const;
	bool 			useItem();

	// Game and Shop functionality
	unsigned int getItemCount(Item::ItemID i) const;

	// Pure shop functionality
	bool buyItem(Item::ItemID i, unsigned int c = 0);
	bool sellItem(Item::ItemID i, unsigned int c = 0);
	//void setTextures(const TextureContainer * textureContainer, const TextureRectContainer * textureRectContainer);

	//virtual void updateTexture();
	void updateTexture();

	unsigned int getRoundStatus(Round::Stats stat) const;
	void increaseRoundStatus(Round::Stats stat, unsigned qty);
	void setRoundStatus(Round::Stats stat, unsigned qty);

private:

	static const unsigned int 				maxCount;

	unsigned int 							id;
	sf::Color 								color;
	std::string 							name;
	unsigned int							money;
	std::map<Item::ItemID, unsigned int> 	items;			// or vector?
	Item::ItemID							currentItem;	// -1 for unarmed

	std::map<Round::Stats, int> 			roundStatus;

	const TextureRectContainer* 			textureRectContainer;
	sf::IntRect 							standingRect;
	sf::IntRect 							step1Rect;
	sf::IntRect 							step2Rect;

	int 									stepClock;
};




#endif
