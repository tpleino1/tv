#ifndef MB_SOUNDPLAYER_HH
#define MB_SOUNDPLAYER_HH

#include <list>
#include <string>

//#include <SFML/Audio/Sound.hpp>
//#include <SFML/Audio/SoundBuffer.hpp>

#include "ResourceContainer.hh"

// A player for playing sound effects music from multiple
// buffered audio sources at any one time.
//
// There seems to a problem with the Audio module of SFML
// Thus this SoundPlayer implementation has not been tested thoroughly.
class SoundPlayer
{
	public:
		// enum to reflect the global status of the player
		enum Status
		{
			Stopped = 1,
			Paused	= 1 << 1,
			Playing = 1 << 2
		};

		SoundPlayer();
		// Copy and assignment constructors are disabled since
		// there should be only one instance.
		SoundPlayer(const SoundPlayer& s) = delete;
		SoundPlayer& operator= (const SoundPlayer& s) = delete;
		~SoundPlayer();

		// Access methods
		void addSound(const std::string& s, const std::string& f);
		void play(const std::string& s);
		void setGlobalStatus(unsigned int s);
		void removeSounds(unsigned int s);
		void setVolume(float v);

	private:
		//typedef ResourceContainer<sf::SoundBuffer, 	std::string> 	SoundContainer;

		//SoundContainer			buffer;
		//std::list<sf::Sound> 	sounds;
		float 					volume;
};

#endif // MB_SOUNDPLAYER_HH
