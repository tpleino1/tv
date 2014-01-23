#ifndef MB_PLAYERHUD_HH
#define MB_PLAYERHUD_HH

#include <string>
#include <map>

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>

//#include "player.hh"
#include "State.hh"
#include "SimpleIni.hh"
#include "ResourceId.hh"
#include "datatables.hh"

class Player;

namespace sf
{
	class RenderTarget;
	class RenderStates;
}


// A HUD element that shows player health, money and current item
// To be changed
class PlayerHUD : public sf::Drawable, public sf::Transformable
{
	public:
		PlayerHUD(); // Ctor required by STL-containers
		// Actual constructor to be used
		PlayerHUD(const Player& p, const State::Resources r);
		//virtual ~PlayerHUD();

		// Reflect changes in player to the HUD
		void update(const sf::Time& dt);

	private:
		// Required by sf::Drawable
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		// Warnings
        void lowHealthPulse(const sf::Time& dt);
        void damageTakenPulse(const sf::Time& dt);

        // Related to color cycling
        static const float twoPi;
        static const sf::Time cycleTime;

		// To be changed
		const Player* player;
		unsigned int shownHealth;
		unsigned int shownMoney;
		unsigned int shownItemCount;
		Item::ItemID shownItem;

		// enum for individual elements that make up the HUD
		enum Elements
		{
			PortraitBG,
			HealthBG,
			ItemBG,
			MoneyBG,
			Portrait,
			ItemIcon,
			MoneyIcon,
			HealthBar,
			NameText,
			HealthText,
			ItemText,
			MoneyText
		};

		sf::Color									frameColor;
		std::map<Elements, sf::RectangleShape> 		rectMap;
		std::map<Elements, sf::Sprite> 				spriteMap;
		std::map<Elements, sf::Text> 				labelMap;
		sf::Time 									lowHealthTime;
		sf::Time									damageTakenTime;
		float 										healthFullWidth;
		std::map<Item::ItemID, sf::IntRect> 		itemRects;
};

#endif // MB_PLAYERHUD_HH
