#ifndef MB_CLOCKHUD_HH
#define MB_CLOCKHUD_HH

#include <map>

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>

#include "ResourceId.hh"	// ___Container
#include "SimpleIni.hh"		// CSimpleIniA
#include "World.hh"
#include "State.hh"

namespace sf
{
	class RenderTarget;
	class RenderStates;
}

// A HUD-element to display the clock
class ClockHUD : public sf::Drawable, public sf::Transformable
{
	public:
		ClockHUD(const sf::Time& t, const World& w, const State::Resources r);

		// Tick down the clock, and change HUD to reflect changes in the world
		void update(const sf::Time& dt);
		// Get or set remaining time
		const sf::Time& getTime() const;
		void setTime(const sf::Time& t);

		void setCounting(bool c);
		bool isCounting() const;		

	private:
		// Required by drawable
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        // Calculate next color as time advances
        void colorPulse(const sf::Time& dt);

		// Static consts related to color pulsing
        static const float twoPi;
        static const sf::Time cycleTime;
        static const sf::Time warningTime;

		bool 			countTime;
        sf::Time 		hourglass;	// Time remaining
		sf::Time 		pulseTime;	// Pulse timer

		const World*	world;
		unsigned int 	shownTime;
        unsigned int 	shownTreasures;	// To be removed
        unsigned int 	shownMonsters;	// To be removed

		// enum for elements the HUD comprises of
 		enum Elements
		{
			ClockBG,
			TreasuresBG,
			MonstersBG,
			TreasuresIcon,
			MonstersIcon,
			ClockText,
			TreasuresText,
			MonstersText
		};

		// Element storage
		std::map<Elements, sf::RectangleShape> 	rectMap;
		std::map<Elements, sf::Sprite> 			spriteMap;
		std::map<Elements, sf::Text> 			labelMap;
		sf::Color 								primaryColor;
		sf::Color 								secondaryColor;

};

#endif // MB_CLOCKHUD_HH
