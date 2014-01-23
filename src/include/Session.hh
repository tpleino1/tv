#ifndef MB_SESSION_HH
#define MB_SESSION_HH

#include <vector>
#include <utility>
#include <string>
#include <map>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>

#include "Round.hh"
#include "player.hh"

// A utility class to convey information about a single
// game session between setup, shop, game and score states.
class Session
{
	public:

		// Settings used in this session, as determined in setup
		class Settings
		{
			public:
				Settings();

				unsigned int nextValue(const std::string& k);
				void setValue(const std::string& k, unsigned int v);
				unsigned int getValue(const std::string& k);
				unsigned int getValue(const std::string& k) const;
				void setOptions(const std::string& k, sf::Rect<unsigned int> v);
				sf::Rect<unsigned int>& getOptions(const std::string& k);
				const sf::Rect<unsigned int>& getOptions(const std::string& k) const;
			private:
				std::map<std::string, unsigned int> 			current;
				std::map<std::string, sf::Rect<unsigned int>> 	options;
		};

		// Constructor
		Session();

		void clear();
		void start(const std::vector<std::pair<std::string, sf::Color>>& p, const TextureContainer * textureContainer, const TextureRectContainer * textureRectContainer);		// Start new game (by Setup)
		void nextRound();	// Create a round and play it (by Shop)
		bool isGameOver() const;

		// Settings access methods
		Settings& getSettings();
		const Settings& getSettings() const;

		// Shop & GameState access methods
		unsigned int getCurrentRoundNumber() const;
		Round& getCurrentRound();
		const Round& getCurrentRound() const;
		void setPlayers(const std::vector<Player>&);
		std::vector<Player>& getPlayers();
		const std::vector<Player>& getPlayers() const;

		// Game Over -screen access methods
		const Round& getRound(int r = -1) const;
		const Round getSessionAsRound() const;

	private:

		bool							valid;
		Settings						rules;
		int								currentRound;
		std::vector<Player>				players;
		std::vector<Round>				rounds;

};

#endif // MB_SESSION_HH
