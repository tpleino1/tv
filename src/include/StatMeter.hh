#ifndef MB_STATMETER_HH
#define MB_STATMETER_HH

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Text.hpp>

namespace sf
{
	class Color;
	class Font;
	class RenderTarget;
	class RenderStates;
}

// A utility class to keep track of, and to display performance statistics (namely FPS)
class StatMeter : public sf::Drawable, public sf::Transformable
{
	public:
		StatMeter();
		virtual ~StatMeter();

		// Initializer functions
		void setColor(const sf::Color& c);
		void setFont(const sf::Font& f);

		// Add a frame after dt seconds (dt is not constant like tick)
		void update(const sf::Time& dt);

	private:
		// Required by sf::Drawable
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		// FPS is calculated/updated in bursts with a length of smoothing to avoid "number racing"
		static const sf::Time smoothing;

		sf::Text 	text;
		sf::Time 	time;
		std::size_t	frames;
};

#endif // MB_STATMETER_HH
