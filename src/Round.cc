#include "Round.hh"

#include <functional>
#include <algorithm>
#include <utility>
#include <cstdio>
#include <cassert>

#include "Utilities.hh"

Round::Round(unsigned int p) :
		players(p),
		success(true),
		duration(sf::Time::Zero),
		values(p * StatCount, 0)
{ }

Round& Round::operator+=(const Round& g)
{
	if (this == &g)
		return *this;
	assert(this->players == g.players);

	this->duration += g.duration;
	this->success = (this->success && g.success);

	if (g.values.size() > this->values.size())
		this->values.resize(g.values.size(), 0);

	for (std::vector<unsigned int>::size_type i = 0; i < g.values.size(); i++)
		this->values[i] += g.values[i];

	return *this;
}

std::vector<unsigned int> Round::rankPlayers(WinCondition w) const
{
	std::vector<unsigned int> retVal(players, 0);

	if (players == 1)	// Single Player
	{
		retVal[0] = (unsigned int)success;
		return retVal;
	}

	// Multi Player, needs some higher maths
	std::vector<std::pair<float, unsigned int>> temp(players);
	std::function<float(unsigned int)> f;
	switch (w) // Higher value is better
	{
	case LeastDamageTaken:
		f = [this] (unsigned int p) { return (float)(-values[p * StatCount + DamageTaken]); };
		break;
	case MostDamageGiven:
		f = [this] (unsigned int p) { return (float)(values[p * StatCount + DamageGiven]); };
		break;
	case MostKills:
		f = [this] (unsigned int p) { return (float)(values[p * StatCount + Kills]); };
		break;
	case BestKDRatio:
		f = [this] (unsigned int p) { return (float)(values[p * StatCount + Kills] / (float)(1 + values[p * StatCount + Deaths])); };
		break;
	case BestKDDiff:
		f = [this] (unsigned int p) { return (float)(values[p * StatCount + Kills] - values[p * StatCount + Deaths]); };
		break;
	case LeastDeaths:
		f = [this] (unsigned int p) { return (float)(-values[p * StatCount + Deaths]); };
		break;
	case MostMoneyGained:
		f = [this] (unsigned int p) { return (float)(values[p * StatCount + MoneyGained]); };
		break;
	case LeastMoneySpent:
		f = [this] (unsigned int p) { return (float)(-values[p * StatCount + MoneySpent]); };
		break;
	case BestBalance:
		f = [this] (unsigned int p) { return (float)(values[p * StatCount + MoneyGained] - values[p * StatCount + MoneySpent]); };
			break;
	case MostItemsUsed:
		f = [this] (unsigned int p) { return (float)(values[p * StatCount + ItemsUsed]); };
		break;
	case LeastItemsUsed:
		f = [this] (unsigned int p) { return (float)(-values[p * StatCount + ItemsUsed]); };
		break;
	default:
		f = [this] (unsigned int p) { return (float)(values[p * StatCount + Kills]); };
		break;
	}

	for (std::vector<unsigned int>::size_type i = 0; i < players; i++)
		temp[i] = std::make_pair(f(i), i);

	std::sort(temp.begin(), temp.end(), [] (const std::pair<float, unsigned int>& a, const std::pair<float, unsigned int>& b) { return (a.first < b.first); });

	unsigned int i = 0;
	for (auto iter = temp.rbegin(); iter != temp.rend(); iter++)
		retVal[iter->second] = ++i;

	return retVal;
}

void Round::setDuration(const sf::Time& d)
{
	duration = d;
}

const sf::Time& Round::getDuration() const
{
	return duration;
}

std::string Round::getScoreString(unsigned int p) const
{
	unsigned int k = values[p * StatCount + Kills];
	unsigned int d = values[p * StatCount + DamageGiven];
	int m = values[p * StatCount + MoneyGained] - values[p * StatCount + MoneySpent];
	return"K: " + toString(k) + " / D: " + toString(d) + " / $: " + toString(m);
}

// Value registration
unsigned int Round::getValue(Round::Stats t, unsigned int p) const
{
	return values[p * StatCount + t];
}

void Round::addValue(Round::Stats t, unsigned int p, unsigned int v)
{
	values[p * StatCount + t] = v;
}

bool Round::outcome() const
{
	return success;
}

void Round::fail()
{
	success = false;
}
