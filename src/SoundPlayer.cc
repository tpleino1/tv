#include "SoundPlayer.hh"

#include <cassert>

// Constructor
SoundPlayer::SoundPlayer() : /*buffer(), sounds(),*/ volume(100.f)
{ }

// Destructor
SoundPlayer::~SoundPlayer()
{ }

// Adds a sound effect into the buffer to be played later
void SoundPlayer::addSound(const std::string& /*s*/, const std::string& /*f*/)
{
	//buffer.load(s, f);
}

// Adds sound corresponding to given identifier to be played
// (multiple simultaneous sound effects alllowed)
void SoundPlayer::play(const std::string& /*s*/)
{
	/*
	sounds.push_back(sf::Sound());
	sf::Sound& e = sounds.back();
	e.setBuffer(buffer.get(s));
	e.setVolume(volume);
	e.play();
	*/
}

// A function to stop, pause or play all the sounds currently in the playlist
void SoundPlayer::setGlobalStatus(unsigned int /*s*/)
{
	/*
	for (auto& e : sounds)
	{
		if (s & Stopped)
			e.stop();
		else if (s & Paused)
			e.pause();
		else if (s & Playing)
			e.play();
	}
	*/
}

// Removes all sounds that are stopped, paused and/or currently playing from the playlist
void SoundPlayer::removeSounds(unsigned int /*s*/)
{
	/*
	sounds.remove_if([s] (const sf::Sound& e) -> bool
	{
		sf::Sound::Status t = e.getStatus();
		return (((s & Stopped) && (t == sf::Sound::Stopped)) ||
			((s & Paused) && (t == sf::Sound::Paused)) ||
			((s & Playing) && (t == sf::Sound::Playing)));
	});
	*/
}

// Basic "setter"
void SoundPlayer::setVolume(float v)
{
	assert(v >= 0.f && v <= 100.f);
	volume = v;
}
