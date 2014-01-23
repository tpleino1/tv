#include "GameState.hh"

#include <vector>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include "player.hh"
#include "Round.hh"

const sf::Time GameState::freezeTime = sf::seconds(2.0f);

//Constructor
GameState::GameState(StateStack& s, State::Resources r) :
		State(s, r),
		phase(Start),
		hourglass(sf::Time::Zero),
		weaponController(this),
		environmentController(this),
		characterController(this, &r),
		world(this, fromString<float>((r.config)->GetValue("Map", "tilesize"))),
		hasFoundExit(false),
		hud(),
		clk(sf::seconds((r.game)->getSettings().getValue("length")), world, r),
		msgs((r.fonts)->get("default"), (r.colors)->get("default"), 20)
{
	// Config Values
	const sf::Vector2f 	cfg_screenSize	= fromString<sf::Vector2f>((r.config)->GetValue(	"Video", 	"size"));
	const sf::Vector2f 	cfg_margin 		= fromString<sf::Vector2f>((r.config)->GetValue(	"HUD", 		"outerMargin"));
	const float 		cfg_height 		= fromString<float>((r.config)->GetValue(			"HUD", 		"height"));
	const float 		cfg_playerWidth = fromString<float>((r.config)->GetValue(			"HUD", 		"playerWidth"));
	const float 		cfg_clockWidth 	= fromString<float>((r.config)->GetValue(			"HUD", 		"clockWidth"));
	const sf::Vector2f 	cfg_messagePos 	= fromString<sf::Vector2f>((r.config)->GetValue(	"HUD", 		"msgPos"));
	const sf::Vector2u 	cfg_levelSize 	= fromString<sf::Vector2u>((r.config)->GetValue(	"Map", 		"Size"));

	// HUD
	std::vector<Player>& players = (r.game)->getPlayers();
	for (auto& p : players)
	{
		hud.push_back(PlayerHUD(p, r));
	}

	clk.setPosition(cfg_margin);
	for (std::vector<PlayerHUD>::size_type i = 0; i < hud.size(); i++)
		hud[i].setPosition(cfg_margin + sf::Vector2f(cfg_clockWidth, 0.f) +
			sf::Vector2f(cfg_margin.x, 0.f) + (float)i * (sf::Vector2f(cfg_margin.x, 0.f) +
			sf::Vector2f(cfg_playerWidth, 0.f)));

	msgs.setPosition(sf::Vector2f(0.5f * cfg_screenSize.x, 2.f * cfg_margin.y + cfg_height) + cfg_messagePos);


	unsigned int diff = 2 * (r.game)->getSettings().getValue("monsters");
	if ((r.game)->getSettings().getValue("campaign"))
		diff *= (2 + r.game->getCurrentRoundNumber());

	// Level Creation
	this->world.generateLevel(
		cfg_levelSize.x,
		cfg_levelSize.y,
		players.size(),
		((bool)(r.game)->getSettings().getValue("campaign") ? Level::MapType::Campaign : Level::MapType::Deathmatch),
		!(bool)(r.game)->getSettings().getValue("fog"),
		diff,
		(r.game)->getSettings().getValue("treasures")
		);

	//const sf::Vector2f cfg_wSize(16.f * 70, 16.f * 35);
	changeAnchor(world, Anchor::Center);
	world.setPosition(0.5f * cfg_screenSize + 0.5f * sf::Vector2f(0.f, 2.f * cfg_margin.y + cfg_height));

	characterController.setPlayers(players);
	characterController.setPlayerSpawns(this->world.getLevel().getPlayerSpawnLocations());
	characterController.setMonsterSpawns(this->world.getLevel().getMonsterSpawnLocations());

	// Theme
	(r.music)->play("game");

	clk.update(sf::Time::Zero);
	msgs.addString(std::string("GET READY!"), sf::seconds(2.f));
}

// Destructor
GameState::~GameState()
{ }


// To be changed
Tile& GameState::getTile(Coordinate coords)
{
	return this->world.getLevel().getTile(sf::Vector2i(coords.x, coords.y));
}


Tile& GameState::getTile(sf::Vector2i coords)
{
	return this->world.getLevel().getTile(coords);
}

// To be changed
Level& GameState::getLevel()
{
	return this->world.getLevel();
}

// To be changed
void GameState::callGameEvent(GameEvent gameEvent)
{
 	characterController.invokeGameEvent(gameEvent);
 	weaponController.invokeGameEvent(gameEvent);
 	environmentController.invokeGameEvent(gameEvent);
	world.invokeGameEvent(gameEvent);

	if (gameEvent.eventType == EventType::PlayerOnExit)
		hasFoundExit = true;
}

// Render
void GameState::draw()
{
	sf::RenderWindow& w = *(getResources().window);

	// Draw World
	w.draw(world);

	// Draw HUD
	w.draw(clk);
	for (auto& h : hud)
		w.draw(h);

	// Draw Messages
	w.draw(msgs);
}

// Update world with dt = tick, handle real-time input
bool GameState::update(const sf::Time& dt)
{
	msgs.update(dt);	// Update messages

	switch (phase)
	{
	case Start:		// pre-game
		hourglass += dt;
		if (hourglass > freezeTime)
		{
			hourglass = sf::Time::Zero;
			phase = OnGoing;
			msgs.addString(std::string("GO!"), sf::seconds(2.f));
		}
		return false;
	case Finish:	// post-game
		hourglass += dt;
		if (hourglass > freezeTime)
		{
			deferedClear();
			deferedPush(States::Score);
		}
		break;
	default:		// game
		characterController.update(dt.asMilliseconds()); // Update characters
		environmentController.update(dt.asMilliseconds());
		if (isRoundOver())
		{
			hourglass = sf::Time::Zero;
			msgs.addString(std::string("ROUND OVER!"), sf::seconds(2.f));
			phase = Finish;
		}
		break;
	}

	// Update World
	weaponController.update(dt.asSeconds());

	// Update HUD
	for (auto& h : hud)
		h.update(dt);
	clk.update(dt);

	return false;	// Stop updating in this state
}

// Handle one-time events (i.e., real-time input is not done here)
bool GameState::handleEvent(const sf::Event& e)
{
	if (phase == OnGoing)
		characterController.handleEvent(e);

	// Pause when focus is lost
	if (e.type == sf::Event::LostFocus)
		deferedPush(States::Pause);
	// A Key was pressed
	else if (e.type == sf::Event::KeyPressed)
	{
		if (e.key.code == sf::Keyboard::Escape)
			deferedPush(States::Pause);
	}
	return false;	// Stop event handling in this state
}

// Check whether the round is over
bool GameState::isRoundOver()
{
	State::Resources 			r 				= getResources();
	Session* 					s 				= r.game;
	const Session::Settings* 	o 				= &(s->getSettings());

	const bool 					campaign 		= o->getValue("campaign");
	const unsigned int 			treasuresTotal 	= o->getValue("treasures");
	const unsigned int 			treasuresLeft 	= world.getTreasureCount();

	const sf::Time 				timeTotal 		= sf::seconds(o->getValue("length"));
	const sf::Time&	 			timeLeft 		= clk.getTime();

	const std::vector<Player>& 	players 		= (r.game)->getPlayers();
	const unsigned int 			playersTotal 	= players.size();
	unsigned int 				playersLeft 	= 0;
	for (auto& p : players)
		if (p.getHealth() > 0)
			playersLeft++;

	short over = 0;

	// Campaign
	if (campaign)
	{
		if (playersLeft == 0 || timeLeft == sf::Time::Zero)	// Failure
			over = -1;
		else if (hasFoundExit)	// Success
			over = 1;
	}
	else // Deathmatch
	{
		if (playersTotal > 1)
		{
			if (playersLeft == 1 || timeLeft == sf::Time::Zero)
				over = 1;
		}
		else
		{
			if (playersLeft == 0 || timeLeft == sf::Time::Zero)	// Fail
				over = -1;
			else if (treasuresTotal > 0 && treasuresLeft == 0)	// Success
				over = 1;
		}
	}

	if (over != 0)
	{
		if (over < 0)
			s->getCurrentRound().fail();
		s->getCurrentRound().setDuration(timeTotal - timeLeft);
		for (auto& p : players)	// Add stats
			for(unsigned int i = Round::DamageGiven; i < (unsigned int)Round::StatCount; i++)
				s->getCurrentRound().addValue((Round::Stats)i, p.getId(), p.getRoundStatus((Round::Stats)i));

		clk.setCounting(false);
		return true;
	}

	return false;
}

// FOR DEBUGGING
void GameState::showMessage(const std::string& message)
{
	msgs.addString(message, sf::seconds(0));
}

unsigned GameState::getMonsterCount() const
{
	return this->characterController.getMonsterCount();
}
