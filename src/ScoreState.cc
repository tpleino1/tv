#include "ScoreState.hh"

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include "Utilities.hh"
#include "KeyBindings.hh"

namespace
{
	enum Elements
	{
		Name 	= 0,
		Total 	= 1,
		Result 	= 2,
		RoundLevel 	= 3
	};
}

const unsigned int ScoreState::column = 5;

//Constructor
ScoreState::ScoreState(StateStack& s, State::Resources r) :
		State(s, r),
		background(),
		title(),
		labelMap(),
		buttonMap(),
		group((*(r.keyBinds))[0])
{
	// Config values
	const sf::Vector2f 	cfg_screenSize 		= fromString<sf::Vector2f>(r.config->GetValue("Video", 	"size"));
	const sf::Vector2f 	cfg_titlePos 		= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"titlePos"));
	const sf::Vector2f 	cfg_menuPos 		= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"menuPos"));
	const sf::Vector2f 	cfg_leftMargin 		= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"leftMarginScore"));
	const sf::Vector2f 	cfg_midMargin 		= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"midMarginScore"));
	const sf::Vector2f 	cfg_margin 			= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"margin"));

	const unsigned int 	cfg_charSize 		= fromString<unsigned int>(r.config->GetValue("Menu", 	"charSize"));
	const sf::Font& 	cfg_font 			= (r.fonts)->get("default");
	const sf::Color& 	cfg_color 			= (r.colors)->get("default");

	// Scoring related
	Session* 						g 						= (r.game);
	const Session::Settings* 		o 						= &(g->getSettings());
	const std::vector<Player>& 		cfg_players 			= g->getPlayers();
	const unsigned int  			cfg_playerCount 		= cfg_players.size();
	const unsigned int				cfg_roundCount 			= g->getCurrentRoundNumber();
	const Round 					cfg_sessionScores 		= g->getSessionAsRound();
	const std::vector<unsigned int> cfg_results 			= cfg_sessionScores.rankPlayers(Round::MostKills);
	const bool 						cfg_gameOver			= g->isGameOver();
	const bool 						cfg_outcome 			= cfg_sessionScores.outcome();
	const bool 						cfg_campaign 			= o->getValue("campaign");
	const bool 						cfg_soloCampaign 		= (cfg_playerCount == 1) || (cfg_campaign);
	std::vector<const Round*> 		cfg_rounds(cfg_roundCount);
	for (unsigned int i = 0; i < cfg_roundCount; i++)
		cfg_rounds[i] = &(g->getRound(i));

	// Label & button positions
	std::map<unsigned int, sf::Vector2f> cfg_elemPos({
			{ Name,				sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ ((float)cfg_roundCount + ((cfg_roundCount > 1) ? 1.f : 0.f) + (cfg_gameOver ? 0.f : 1.f) + 4.5f) * cfg_margin },
			{ RoundLevel, 		sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ ((float)cfg_roundCount + ((cfg_roundCount > 1) ? 1.f : 0.f) + (cfg_gameOver ? 0.f : 1.f) + 3.f) * cfg_margin },
			{ Total, 			sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ ((cfg_gameOver ? 0.f : 1.f) + 3.5f) * cfg_margin },
			{ Result, 			sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ ((cfg_gameOver ? 0.f : 1.f) + 2.f) * cfg_margin }
		});
	const sf::Vector2f 	cfg_buttonPos 	= sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos;

	// Background
	background.setTexture((r.textures)->get("background"));

	// Title
	title = sf::Text(cfg_gameOver ? "GAME OVER" : (cfg_soloCampaign ? "LEVEL OVER" : "ROUND OVER"), cfg_font, cfg_charSize);
	title.setColor(cfg_color);
	changeAnchor(title, Anchor::BottomRight);
	title.setPosition(sf::Vector2f(cfg_screenSize.x, 0.f) + cfg_titlePos);

	// "Static" Labels
	labelMap[Name] = sf::Text("PLAYER", cfg_font, cfg_charSize);
	changeAnchor(labelMap[Name], Anchor::BottomLeft);
	labelMap[Name].setPosition(cfg_elemPos[Name]);

	for (unsigned int i = 0; i < cfg_roundCount; i++)
	{
		labelMap[RoundLevel + i] = sf::Text((cfg_soloCampaign ? "LEVEL " : "ROUND ") + toString(i+1) + " (" + toString(cfg_rounds[i]->getDuration()) + ")", cfg_font, cfg_charSize);
		changeAnchor(labelMap[RoundLevel + i], Anchor::BottomLeft);
		labelMap[RoundLevel + i].setPosition(cfg_elemPos[RoundLevel] - (float)i * cfg_margin);
	}

	if (cfg_roundCount > 1)
	{
		labelMap[Total] = sf::Text("TOTAL (" + toString(cfg_sessionScores.getDuration()) + ")", cfg_font, cfg_charSize);
		changeAnchor(labelMap[Total], Anchor::BottomLeft);
		labelMap[Total].setPosition(cfg_elemPos[Total]);
	}

	labelMap[Result] = sf::Text(cfg_soloCampaign ? "OUTCOME" : "STANDINGS", cfg_font, cfg_charSize);
	changeAnchor(labelMap[Result], Anchor::BottomLeft);
	labelMap[Result].setPosition(cfg_elemPos[Result]);

	// Name labels
	for (std::vector<Player>::size_type i = 0; i < cfg_players.size(); i++)
	{
		unsigned int col = (i+1) << column;
		sf::Vector2f posMargin = cfg_leftMargin + (float)i * cfg_midMargin;

		labelMap[Name + col] = sf::Text(cfg_players[i].getName(), cfg_font, cfg_charSize);
		changeAnchor(labelMap[Name + col], Anchor::Bottom);
		labelMap[Name + col].setPosition(cfg_elemPos[Name] + posMargin);
	}

	// Round label
	for (unsigned int i = 0; i < cfg_roundCount; i++)
	{
		for (std::vector<Player>::size_type j = 0; j < cfg_players.size(); j++)
		{
			unsigned int col = (j+1) << column;
			sf::Vector2f posMargin = cfg_leftMargin + (float)j * cfg_midMargin - (float)i * cfg_margin;

			labelMap[RoundLevel + i + col] = sf::Text(cfg_rounds[i]->getScoreString(j), cfg_font, cfg_charSize);
			changeAnchor(labelMap[RoundLevel + i + col], Anchor::Bottom);
			labelMap[RoundLevel + i + col].setPosition(cfg_elemPos[RoundLevel] + posMargin);
		}
	}

	// Total labels
	if (cfg_roundCount > 1)
	{
		for (std::vector<Player>::size_type i = 0; i < cfg_players.size(); i++)
		{
			unsigned int col = (i+1) << column;
			sf::Vector2f posMargin = cfg_leftMargin + (float)i * cfg_midMargin;

			labelMap[Total + col] = sf::Text(cfg_sessionScores.getScoreString(i), cfg_font, cfg_charSize);
			changeAnchor(labelMap[Total + col], Anchor::Bottom);
			labelMap[Total + col].setPosition(cfg_elemPos[Total] + posMargin);
		}
	}

	// Result labels
	for (std::vector<Player>::size_type i = 0; i < cfg_players.size(); i++)
	{
		unsigned int col = (i+1) << column;
		sf::Vector2f posMargin = cfg_leftMargin + (float)i * cfg_midMargin;
		std::string temp;

		if (cfg_soloCampaign)
			temp = cfg_outcome ? "SUCCESS" : "FAIL";
		else
		{
			temp = toString(cfg_results[i]);
			if (cfg_results[i] / 10 == 1)		// 11th, 12th, 13th ...
				temp += "th";
			else
			{
				switch (cfg_results[i] % 10)	// 1st, 2nd, 3rd, 4th, ..., 21st, 22nd, 23rd, 24th, ...
				{
				case 1: temp += "st"; break;
				case 2: temp += "nd"; break;
				case 3: temp += "rd"; break;
				default: temp += "th"; break;
				}
			}
		}
		labelMap[Result + col] = sf::Text(temp, cfg_font, cfg_charSize);
		changeAnchor(labelMap[Result + col], Anchor::Bottom);
		labelMap[Result + col].setPosition(cfg_elemPos[Result] + posMargin);
	}

	// Labels: Color
	for (auto& l : labelMap)
		l.second.setColor(cfg_color);

	// Button: Next Round / Next Level
	if (!cfg_gameOver)
	{
		buttonMap[0] = Button((cfg_playerCount == 1) ? "NEXT LEVEL" : "NEXT ROUND", cfg_font, *(r.colors));
		buttonMap[0].setCharacterSize(cfg_charSize);
		changeAnchor(buttonMap[0], Anchor::BottomLeft);
		buttonMap[0].setPosition(cfg_buttonPos + cfg_margin);
		group.add(buttonMap[0]);
		buttonMap[0].setCallback([this] () { (getResources().game)->nextRound(); deferedClear(); deferedPush(States::Shop); } );
	}

	// Button: Main Menu
	buttonMap[1] = Button("MAIN MENU", cfg_font, *(r.colors));
	buttonMap[1].setCharacterSize(cfg_charSize);
	changeAnchor(buttonMap[1], Anchor::BottomLeft);
	buttonMap[1].setPosition(cfg_buttonPos);
	group.add(buttonMap[1]);
	buttonMap[1].setCallback([this] () { deferedClear(); deferedPush(States::Menu); } );

	// Play theme
	(r.music)->play("menu");
}

// Destructor
ScoreState::~ScoreState()
{ }

// Render: BG, title, button
void ScoreState::draw()
{
	sf::RenderWindow& w = *(getResources().window);

	w.draw(background);
	w.draw(title);
	for (auto& l : labelMap)
		w.draw(l.second);
	for (auto& b : buttonMap)
		w.draw(b.second);
}

// Nothing to update
bool ScoreState::update(const sf::Time&)
{
	return false; // Stop updating here
}

// Escape as a special case, otherwise pass event to group
bool ScoreState::handleEvent(const sf::Event& e)
{
	if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::Escape))
		group.selectEnd(true);
	else
		group.handleEvent(e);
	return false; // Stop event handling here
}
