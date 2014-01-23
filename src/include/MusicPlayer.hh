#ifndef MB_MUSICPLAYER_HH
#define MB_MUSICPLAYER_HH

#include <string>
#include <map>

//#include <SFML/Audio/Music.hpp>

// A player for playing background music from a single
// streamable audio source at any one time.
//
// There seems to a problem with the Audio module of SFML (in a base class destructor).
// Thus this MusicPlayer implementation has not been tested thoroughly.
// Actual audio functionality is commented out.
class MusicPlayer
{
	public:
		// enum to reflect the internal state of MusicPlayer
		enum Status
		{
			Stopped,
			Paused,
			Playing
		};

		MusicPlayer();
		// Copy and assignment constructors are disabled since
		// there should be only one  MusicPlayer instance.
		MusicPlayer(const MusicPlayer& m) = delete;
		MusicPlayer& operator= (const MusicPlayer& m) = delete;
		~MusicPlayer();

		// Public access methods
		void addTrack(const std::string& t, const std::string& f);
		void play(const std::string& t);
		void setStatus(Status s);
		void setVolume(float v);

	private:
		//sf::Music 							player;
		std::map<std::string, std::string> 	files;
		std::string							lastPlayed;
		float	 							volume;
};

#endif // MB_MUSICPLAYER_HH
