#ifndef MB_SHOPSTATE_HH
#define MB_SHOPSTATE_HH

#include <map>
#include <vector>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "State.hh"
#include "Button.hh"
#include "ButtonGroup.hh"
#include "datatables.hh"

//#include "weaponController.hh"

namespace sf
{
	class Time;
	class Event;
}

class StateStack;

class ShopState : public State
{
	public:
		ShopState(StateStack &s, State::Resources r);
		ShopState(const ShopState& s) = delete;
		ShopState& operator= (const ShopState& s) = delete;
		virtual ~ShopState();

		virtual void draw();
		virtual bool update(const sf::Time& dt);
		virtual bool handleEvent(const sf::Event& e);

	private:
		static const unsigned int 	column;
		static const float 			buyFactor;
		static const float 			sellFactor;

		// Shop callbacks before std::binding
		void leaveState(unsigned int player, bool toGame);
		void shopItem(unsigned int player, unsigned int row, Item::ItemID item, bool buy = true);

		sf::Sprite 							background;
		sf::Text 							title;
		std::map<unsigned int, sf::Text> 	labelMap;
		std::map<unsigned int, Button> 		buttonMap;
		std::vector<ButtonGroup>			groups;
		std::vector<Player> 				players;
		std::vector<unsigned int>			moneySpent;
		std::vector<unsigned int>			moneyGained;
		std::map<Item::ItemID, WeaponData> 	items;
};

#endif // MB_SHOPSTATE_HH
