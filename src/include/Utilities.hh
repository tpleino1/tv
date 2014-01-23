#ifndef MB_UTILITIES_HH
#define MB_UTILITIES_HH

#include <sstream>	// toString(T) & fromString
#include <iostream>
#include <string>
#include <iomanip> 	// toString(T, int)

#include <SFML/Graphics/Color.hpp>	// toString(sf::Color), fromString(sf::Color)
#include <SFML/Graphics/Rect.hpp>	// changeAnchor
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "KeyBindings.hh"
#include "SimpleIni.hh"

// Drawable element anchors
namespace Anchor
{
	enum ID
	{
		TopLeft 	= 9,	// 001 001
		Top 		= 10,	// 001 010
		TopRight 	= 12,	// 001 100
		Left 		= 17,	// 010 001
		Center 		= 18,	// 010 010
		Right 		= 20,	// 010 100
		BottomLeft 	= 33,	// 100 001
		Bottom 		= 34,	// 100 010
		BottomRight = 36	// 100 100
	};
}

// KeyBindings are load from file in two different classes, and
// saved in one class
void saveKeyBindings(const KeyBindings::Container& b, CSimpleIniA& c);
void loadKeyBindings(KeyBindings::Container& b, const CSimpleIniA& c);

// Lexical cast from T to std::string
template <typename T>
std::string toString(const T& v);

// Lexical cast from T to std::string with a precision of n (for real numbers)
template <typename T>
std::string toString(const T& v, int n);

// Lexical cast from std::string to T
template <typename T>
T fromString(const std::string& s);

// IO-manipulation with sf::Color
std::ostream& operator<<(std::ostream& os, const sf::Color& c);
std::istream& operator>>(std::istream& is, sf::Color& c);

// IO-manipulation with sf::Time
std::ostream& operator<<(std::ostream& os, const sf::Time& t);
std::istream& operator>>(std::istream& is, sf::Time& t);

// IO-manipulation with sf::Keyboard::Key
std::ostream& operator<<(std::ostream& os, const sf::Keyboard::Key& k);
std::istream& operator>>(std::istream& is, sf::Keyboard::Key& k);

// Random number generation with limits [l, u)
float floatRNG(float l = 0, float u = 1);
int intRNG(int l = 0, int u = 1);

// IO-manipulation with sf::Rect<T>
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const sf::Rect<T>& r);
template <typename T>
inline std::istream& operator>>(std::istream& is, sf::Rect<T>& r);

// IO-manipulation with sf::Vector2<T>
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& v);
template <typename T>
inline std::istream& operator>>(std::istream& is, sf::Vector2<T>& v);

// IO-manipulation with sf::Vector3<T>
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const sf::Vector3<T>& v);
template <typename T>
inline std::istream& operator>>(std::istream& is, sf::Vector3<T>& v);

template <typename T>
sf::Rect<T> nextRect(const sf::Rect<T>& rect, int x = 0, int y = 0);

// Change origin of a Transformable object
template <typename T>
void changeAnchor(T& obj, Anchor::ID to = Anchor::TopLeft, Anchor::ID from = Anchor::TopLeft);


// Lexical cast from T to std::string
template <typename T>
std::string toString(const T& v)
{
	std::ostringstream oss;
	oss << v;
	return oss.str();
}

// Lexical cast from T to std::string with a precision of n (for floating points)
template <typename T>
std::string toString(const T& v, int n)
{
	std::ostringstream oss;
	oss << std::setprecision(n) << v;
	return oss.str();
}

// Lexical cast from std::string to T
template <typename T>
T fromString(const std::string& s)
{
	T v;
	std::istringstream iss;
	iss.str(s);
	iss >> v;
	return v;
}

// IO-manipulation with sf::Rect<T>
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const sf::Rect<T>& r)
{
	os << r.left << ' ' << r.top << ' ' << r.width << ' ' << r.height;
	return os;
}

// IO-manipulation with sf::Rect<T>
template <typename T>
inline std::istream& operator>>(std::istream& is, sf::Rect<T>& r)
{
	is >> r.left >> r.top >> r.width >> r.height;
	return is;
}

// IO-manipulation with sf::Vector2<T>
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& v)
{
	os << v.x << ' ' << v.y;
	return os;
}

// IO-manipulation with sf::Vector2<T>
template <typename T>
inline std::istream& operator>>(std::istream& is, sf::Vector2<T>& v)
{
	is >> v.x >> v.y;
	return is;
}

// IO-manipulation with sf::Vector3<T>
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const sf::Vector3<T>& v)
{
	os << v.x << ' ' << v.y << ' ' << v.z;
	return os;
}

// IO-manipulation with sf::Vector3<T>
template <typename T>
inline std::istream& operator>>(std::istream& is, sf::Vector3<T>& v)
{
	is >> v.x >> v.y >> v.z;
	return is;
}

template <typename T>
sf::Rect<T> nextRect(const sf::Rect<T>& rect, int x = 0, int y = 0)
{
	return sf::Rect<T>(rect.left + x * rect.width, rect.top + y * rect.height, rect.width, rect.height);
}


// T must implement the following:
// 		sf::FloatRect 		getLocalBounds()					// Text, Sprite, Shape, ...
//		void 				setOrigin(const Vector2f& origin)	// Transformable
//		const Vector2f& 	getOrigin() const					// Transformable
template <typename T>
void changeAnchor(T& obj, Anchor::ID to = Anchor::TopLeft, Anchor::ID from = Anchor::TopLeft)
{
	if (to == from)	// nothing to be done
		return;

	sf::Vector2f o = obj.getOrigin();
	sf::FloatRect b = obj.getLocalBounds();

	float c;	// Coefficient

	// Anchor changed horizontally
	if ((to & 0x07) != (from & 0x07))
	{
		c = 0.f;
		// Find and subtract original X-offset
		/*if (from & 1)			// Left
			c -= 0.f;
		else*/ if (from & 1 << 1)	// Center
			c -= 0.5f;
		else if (from & 1 << 2)	// Right
			c -= 1.f;

		// Find and add new X-offset
		/*if (to & 1)				// Left
			c += 0.f;
		else*/ if (to & 1 << 1)	// Center
			c += 0.5f;
		else if (to & 1 << 2)	// Right
			c += 1.f;
		o.x = c * b.width;
	}

	// Anchor changed vertically
	if ((to & 0x38) != (from & 0x38))
	{
		c = 0.f;
		// Find and subtract original Y-offset
		/*if (from & 1 << 3)		// Top
			c -= 0.f;
		else*/ if (from & 1 << 4)	// Middle
			c -= 0.5f;
		else if (from & 1 << 5)	// Bottom
			c -= 1.f;

		// Find and add new Y-offset
		/*if (to & 1 << 3)		// Top
			c += 0.f;
		else*/ if (to & 1 << 4)	// Middle
			c += 0.5f;
		else if (to & 1 << 5)	// Bottom
			c += 1.f;
		o.y = c * b.height;
	}

	obj.setOrigin(o);
}

#endif // MB_UTILITIES_HH
