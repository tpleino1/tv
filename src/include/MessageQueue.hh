#ifndef MB_MESSAGEQUEUE_HH
#define MB_MESSAGEQUEUE_HH

#include <list>
#include <utility> // pair
#include <string>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include "Utilities.hh"

namespace sf
{
	class RenderTarget;
	class RenderStates;
}

// A utility class for displaying on-screen messages
class MessageQueue : public sf::Drawable, public sf::Transformable
{
	public:
		MessageQueue();
		explicit MessageQueue(const sf::Font& f,
			const sf::Color& c = sf::Color::White,
			unsigned int s = 20u,
			Anchor::ID a = Anchor::Center,
			const sf::Vector2f& m = sf::Vector2f(0.f, 20.f),
			const sf::Time& t = sf::seconds(0.5));
		//virtual ~MessageQueue();

		// Constructor helpers
		void setFont(const sf::Font& f);
		void setColor(const sf::Color&);
		void setCharSize(unsigned int s);
		void setAnchor(Anchor::ID a);
		void setMargin(const sf::Vector2f& m);
		void setFadeTime(const sf::Time& t);

		// Access methods
		void addString(const std::string& s, const sf::Time& d);
		void addText(const sf::Text& t, const sf::Time& d);
		void clear();

		// Process queue with a time step
		void update(const sf::Time& dt);

	private:
		// Required by sf::Drawable
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		static const sf::Time 	defaultDuration;
		static const bool 		copyToStdOut;

		std::list<std::pair<sf::Text, sf::Time>> 	msgs;
		const sf::Font* 							font;
		sf::Color 									color;
		unsigned int 								charSize;
		Anchor::ID 									anchor;
		sf::Vector2f 								margin;
		sf::Time 									fadeTime;
};

#endif // MB_MESSAGEQUEUE_HH
