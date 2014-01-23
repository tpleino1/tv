#include "Session.hh"

#include <cassert>
//#include "player.hh"
#include "Utilities.hh"
// Session::Settings Constructor
Session::Settings::Settings() : current(), options()
{ }

unsigned int Session::Settings::nextValue(const std::string& k)
{
	assert(options.find(k) != options.end());
	assert(current.find(k) != current.end());

	const unsigned int min = options[k].left;
	const unsigned int max = options[k].top;
	const unsigned int step = options[k].width;
	const unsigned int incr = options[k].height; // 1 = exponential, 0 = linear

	if (incr == 1)						// multiply
	{
		if (current[k] == 0)
			current[k] = 1;
		else
			current[k] *= step;
	}
	else 								// add
		current[k] += step;

	if (current[k] > max)
		current[k] = min;

	return current[k];
}

void Session::Settings::setValue(const std::string& k, unsigned int v)
{
	current[k] = v;
}

unsigned int Session::Settings::getValue(const std::string& k) const
{
	auto iter = current.find(k);
	assert(iter != current.end());

	return iter->second;
}

unsigned int Session::Settings::getValue(const std::string& k)
{
	auto iter = current.find(k);
	assert(iter != current.end());

	return iter->second;
}

void Session::Settings::setOptions(const std::string& k, sf::Rect<unsigned int> v)
{
	options[k] = v;
}

const sf::Rect<unsigned int>& Session::Settings::getOptions(const std::string& k) const
{
	auto iter = options.find(k);
	assert(iter != options.end());

	return iter->second;
}

sf::Rect<unsigned int>& Session::Settings::getOptions(const std::string& k)
{
	auto iter = options.find(k);
	assert(iter != options.end());

	return iter->second;
}

// Session Constructor
Session::Session() :
	valid(false),
	rules(),
	currentRound(-1),
	players(),
	rounds()
{ }

void Session::clear()
{
	currentRound = -1;
	players.clear();
	rounds.clear();
	valid = false;
}

// Start new game (i.e., create players)
void Session::start(const std::vector<std::pair<std::string, sf::Color>>& p, const TextureContainer * textureContainer, const TextureRectContainer * textureRectContainer)
{
	clear();
	const unsigned int money = rules.getValue("cash");
	for (unsigned int i = 0; i < rules.getValue("players"); i++)
		players.push_back(Player(i, p[i].second, p[i].first, money, textureContainer, textureRectContainer));

	nextRound();
	for (auto& p : players)
		p.increaseRoundStatus(Round::MoneyGained, money);
		//rounds.back().addValue(Round::MoneyGained, p.getId(), money);
	valid = true;
}

// Create a round and play it
void Session::nextRound()
{
	// Restore health
	for (auto& p : players)
		p.roundReset();
	rounds.push_back(Round(rules.getValue("players")));
	currentRound++;
}

// Game Over -screen access methods
const Round& Session::getRound(int r) const
{
	assert(currentRound >= 0);
	assert(r <= currentRound);
	return rounds[r < 0 ? currentRound : r];
}

const Round Session::getSessionAsRound() const
{
	assert(currentRound >= 0);
	Round g(rules.getValue("players"));
	for (auto& r : rounds)
		g += r;
	return g;
}

// Settings access
Session::Settings& Session::getSettings()
{
	return rules;
}

// Settings const access
const Session::Settings& Session::getSettings() const
{
	return rules;
}

Round& Session::getCurrentRound()
{
	assert(currentRound >= 0);
	return rounds.back();
}

const Round& Session::getCurrentRound() const
{
	assert(currentRound >= 0);
	return rounds.back();
}

std::vector<Player>& Session::getPlayers()
{
	return players;
}

const std::vector<Player>& Session::getPlayers() const
{
	return players;
}

void Session::setPlayers(const std::vector<Player>& p)
{
	players = p;
}

unsigned int Session::getCurrentRoundNumber() const
{
	return currentRound + 1;
}

// Test if the game is over
bool Session::isGameOver() const
{
	return (getCurrentRoundNumber() == rules.getValue("rounds") || !getCurrentRound().outcome());
}
