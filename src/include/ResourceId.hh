#ifndef MB_RESOURCEID_HH
#define MB_RESOURCEID_HH

#include <string>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
//#include <SFML/Audio/SoundBuffer.hpp>

#include "ResourceContainer.hh"

typedef ResourceContainer<sf::Texture, 		std::string> 	TextureContainer;
typedef ResourceContainer<sf::IntRect, 		std::string> 	TextureRectContainer;
typedef ResourceContainer<sf::Font, 		std::string> 	FontContainer;
typedef ResourceContainer<sf::Color, 		std::string> 	ColorContainer;
//typedef ResourceContainer<sf::SoundBuffer, 	std::string> 	SoundContainer;

#endif // MB_RESOURCEID_HH
