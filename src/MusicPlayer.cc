#include "MusicPlayer.hh"

#include <cassert>
#include <stdexcept>

// Constructor
MusicPlayer::MusicPlayer() : /*player(),*/ files(), lastPlayed(""), volume(100.f)
{ }

// Destructor
MusicPlayer::~MusicPlayer()
{ }

// Add a new file to be played later
void MusicPlayer::addTrack(const std::string& t, const std::string& f)
{
	auto iter = files.find(t);
	assert(iter == files.end());

	files[t] = f;
}

// Play a previously added track
void MusicPlayer::play(const std::string& t)
{
	if (t != lastPlayed)
	{
		auto iter = files.find(t);
		assert(iter != files.end());

		/*
		if (!player.openFromFile(iter->second))
			throw std::runtime_error("Could not open " + iter->second + ".");

		player.setVolume(volume);
		player.setLoop(true);
		*/

		lastPlayed = t;
	}
	//player.play();
}

// Function that allows playing, pausing and stopping
void MusicPlayer::setStatus(Status /*s*/)
{
	/*
	switch (s)
	{
	case Playing:
		player.play();
		break;
	case Paused:
		player.pause();
		break;
	default:
		player.stop();
		break;
	}
	*/
}

// Basic "setter"
void MusicPlayer::setVolume(float v)
{
	assert(v >= 0.f && v <= 100.f);
	volume = v;
}
