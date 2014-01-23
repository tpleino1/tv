#ifndef MB_OBJECT_HH
#define MB_OBJECT_HH

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>


class Object
{
	

	public:

		// Constructor initializes rect with given tilesize and sets its origin to center of the rect
		Object(float tileSize);

		virtual ~Object() {}

		// Not used since the World class handles the drawing
		void draw(sf::RenderTarget& rt);

		// Sets the rect right place on the gamefield
        void setPosition(sf::Vector2f vec);

        // Returns reference to the rect, used for drawing
        const sf::RectangleShape& getRect() const;

    protected:

    	// Rect to be drawed on the gamefield
		sf::RectangleShape rect;
};

#endif
