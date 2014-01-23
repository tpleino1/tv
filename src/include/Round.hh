#ifndef MB_ROUND_HH
#define MB_ROUND_HH

#include <vector>
#include <string>

#include <SFML/System/Time.hpp>

// A class to represent a single Round or level in both MP and SP
// Basically a container for statistics
class Round
{
	public:
		enum WinCondition
		{
			MostDamageGiven,
			LeastDamageTaken,
			MostKills,
			BestKDRatio,
			BestKDDiff,
			LeastDeaths,
			MostMoneyGained,
			LeastMoneySpent,
			BestBalance,
			MostItemsUsed,
			LeastItemsUsed
		};

		enum Stats
		{
			DamageGiven = 0,
			DamageTaken,
			Kills,
			Deaths,
			MoneyGained,
			MoneySpent,
			ItemsUsed,
			StatCount
		};

		explicit Round(unsigned int p);

		Round& operator+=(const Round& g);
		std::vector<unsigned int> rankPlayers(WinCondition w = MostKills) const;

		void setDuration(const sf::Time& d);
		const sf::Time& getDuration() const;

		// Value registration
		unsigned int getValue(Stats t = Kills, unsigned int p = 0) const;
		void addValue(Stats t = Kills, unsigned int p = 0, unsigned int v = 1);

		// Single Player outcome
		bool outcome() const;
		void fail();

		std::string getScoreString(unsigned int p) const;

	private:
		unsigned int 				players;
		bool						success;
		sf::Time					duration;

		std::vector<unsigned int> 	values;
};

#endif // MB_ROUND_HH
