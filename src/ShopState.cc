#include "ShopState.hh"

#include <functional> // function, bind

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Color.hpp>

#include "Session.hh"
#include "Round.hh"
#include "Utilities.hh"
#include "KeyBindings.hh"
#include "player.hh"


namespace // Used for positioning elements
{
	enum Elements
		{
			NameReady 	= 0,	// CCCR RRRR, C = Column, R = Row
			MoneyCancel = 1,	// Name/Money for labels, Ready/Cancel for buttons
			ShopItem 	= 2
		};
}

const unsigned int 	ShopState::column 		= 5;
const float 		ShopState::buyFactor	= 1.0f;
const float 		ShopState::sellFactor	= 0.5f;

//Ctor
ShopState::ShopState(StateStack& s, State::Resources r) :
		State(s, r),
		background(),
		title(),
		labelMap(),
		buttonMap(),
		groups(),
		players((r.game)->getPlayers()),
		moneySpent(players.size(), 0),
		moneyGained(players.size(), 0),
		items(initializeWeaponData(r))//,
		//weaponController(this)
{
	// Config values
	const sf::Vector2f 							cfg_screenSize 	= fromString<sf::Vector2f>(r.config->GetValue("Video", 	"size"));
	const sf::Vector2f 							cfg_titlePos 	= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"titlePos"));
	const sf::Vector2f 							cfg_menuPos 	= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"menuPos"));
	const sf::Vector2f 							cfg_leftMargin 	= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"leftMarginShop"));
	const sf::Vector2f 							cfg_midMargin 	= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"midMarginShop"));
	const sf::Vector2f 							cfg_margin 		= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"margin"));

	const unsigned int 							cfg_charSize 	= fromString<unsigned int>(r.config->GetValue("Menu", 	"charSize"));
	const sf::Font& 							cfg_font 		= (r.fonts)->get("default");
	const sf::Color& 							cfg_color 		= (r.colors)->get("default");

	const unsigned int 							cfg_itemCount 	= items.size();

	const sf::Vector2f							cfg_namePos		= sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ ((float)cfg_itemCount + 4.5f) * cfg_margin;
	const sf::Vector2f							cfg_moneyPos	= sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ ((float)cfg_itemCount + 3.5f) * cfg_margin;
	const sf::Vector2f 							cfg_itemPos 	= sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ ((float)cfg_itemCount + 2.f) * cfg_margin;
	const sf::Vector2f							cfg_readyPos	= sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ 1.f * cfg_margin;
	const sf::Vector2f 							cfg_cancelPos 	= sf::Vector2f(0.f, cfg_screenSize.y) 	+ cfg_menuPos 	+ 0.f * cfg_margin;

	// Background
	background.setTexture((r.textures)->get("background"));


	// Title
	title = sf::Text("SHOP", cfg_font, cfg_charSize);
	changeAnchor(title, Anchor::BottomRight);
	title.setPosition(sf::Vector2f(cfg_screenSize.x, 0.f) + cfg_titlePos);
	title.setColor(cfg_color);


	// Labels that are always present (left column: Name, Money, Items)
	labelMap[NameReady] 	= sf::Text("PLAYER", cfg_font, cfg_charSize);
	changeAnchor(labelMap[NameReady], Anchor::BottomLeft);
	labelMap[NameReady].setPosition(cfg_namePos);

	labelMap[MoneyCancel] 	= sf::Text("MONEY", cfg_font, cfg_charSize);
	changeAnchor(labelMap[MoneyCancel], Anchor::BottomLeft);
	labelMap[MoneyCancel].setPosition(cfg_moneyPos);

	unsigned int i = 0;
	for (auto& e : items)
	{
		labelMap[ShopItem + i] 	= sf::Text(e.second.name + " ($" + toString(e.second.cost) + ")", cfg_font, cfg_charSize);
		changeAnchor(labelMap[ShopItem + i], Anchor::BottomLeft);
		labelMap[ShopItem + i].setPosition(cfg_itemPos - (float)i * cfg_margin);
		i++;
	}


	// Labels and Buttons for each player
	for (unsigned int i = 0; i < players.size(); i++)
	{
		// column & positional offset for this player
		unsigned int col = (i + 1) << column;
		sf::Vector2f posMargin = cfg_leftMargin + (float)i * cfg_midMargin;

		// Name label
		labelMap[NameReady + col] = sf::Text(players[i].getName(), cfg_font, cfg_charSize);
		changeAnchor(labelMap[NameReady + col], Anchor::Bottom);
		labelMap[NameReady + col].setPosition(cfg_namePos + posMargin);

		// Money label
		labelMap[MoneyCancel + col] = sf::Text(toString(players[i].getMoney()), cfg_font, cfg_charSize);
		changeAnchor(labelMap[MoneyCancel + col], Anchor::Bottom);
		labelMap[MoneyCancel + col].setPosition(cfg_moneyPos + posMargin);

		// ButtonGroup
		groups.push_back(ButtonGroup((*(r.keyBinds))[i+1]));

		// Ready button
		buttonMap[NameReady + col] = Button("READY", cfg_font, *(r.colors));
		buttonMap[NameReady + col].setCharacterSize(cfg_charSize);
		changeAnchor(buttonMap[NameReady + col], Anchor::Bottom);
		buttonMap[NameReady + col].setPosition(cfg_readyPos + posMargin);
		buttonMap[NameReady + col].setToggle(true);
		buttonMap[NameReady + col].setCallback(std::bind(&ShopState::leaveState, this, i, true));

		// Cancel button
		buttonMap[MoneyCancel + col] = Button("CANCEL", cfg_font, *(r.colors));
		buttonMap[MoneyCancel + col].setCharacterSize(cfg_charSize);
		changeAnchor(buttonMap[MoneyCancel + col], Anchor::Bottom);
		buttonMap[MoneyCancel + col].setPosition(cfg_cancelPos + posMargin);
		buttonMap[MoneyCancel + col].setToggle(true);
		buttonMap[MoneyCancel + col].setCallback(std::bind(&ShopState::leaveState, this, i, false));

		// Item buttons
		unsigned int j = 0;
		for (auto& e : items)
		{
			buttonMap[ShopItem + j + col] 	= Button(toString(players[i].getItemCount(e.first)), cfg_font, *(r.colors));
			buttonMap[ShopItem + j + col].setCharacterSize(cfg_charSize);
			changeAnchor(buttonMap[ShopItem + j + col], Anchor::Bottom);
			buttonMap[ShopItem + j + col].setPosition(cfg_itemPos - (float)j * cfg_margin + posMargin);
			groups.back().add(buttonMap[ShopItem + j + col]);
			buttonMap[ShopItem + j + col].setCallback(std::bind(&ShopState::shopItem, this, i, j, e.first, true), true);		// Primary: Buy
			buttonMap[ShopItem + j + col].setCallback(std::bind(&ShopState::shopItem, this, i, j, e.first, false), false);	// Secondary: Sell
			j++;
		}

		// Add Ready & Cancel to ButtonGroup
		groups.back().add(buttonMap[NameReady + col]);
		groups.back().add(buttonMap[MoneyCancel + col]);
	}

	for (auto& l : labelMap)
		l.second.setColor(cfg_color);

	// Set background theme
	(r.music)->play("menu");
}


// Destructor
ShopState::~ShopState()
{ }

// Render: BG, labels, buttons
void ShopState::draw()
{
	sf::RenderWindow& w = *(getResources().window);

	w.draw(background);
	w.draw(title);
	for(auto& l : labelMap)
		w.draw(l.second);
	for(auto& b : buttonMap)
		w.draw(b.second);
}

// Nothing to update
bool ShopState::update(const sf::Time&)
{
	return false; // Stop updating here
}

// Pass events to players' ButtonGroups
bool ShopState::handleEvent(const sf::Event& e)
{
	for (auto& g : groups)
		g.handleEvent(e);
	return false; // Stop event handling here
}

// A generic function used when leaving the shop to be used as a callback
// (consent of the all players required)
void ShopState::leaveState(unsigned int player, bool toGame)
{
	// Required: unanimous players
	if (players.size() > 1)
	{
		// Deactivate the other possibility (Ready and Cancel are mutually exclusive)
		buttonMap[(toGame ? MoneyCancel : NameReady) + ((player + 1) << column)].deactivate();

		// Check whether all agree the course of action
		bool allActive = true;
		for (unsigned int i = 0; i < players.size(); i++)
		{
			if (!buttonMap[(toGame ? NameReady : MoneyCancel) + ((i + 1) << column)].isActive())
			{
				allActive = false;
				break;
			}
		}

		if (!allActive)	// We have not come to an understanding
			return;
	}

	deferedClear();
	if (toGame)
	{
		(getResources().game)->setPlayers(players);
		/*Round& r = (getResources().game)->getCurrentRound();
		for (unsigned int i = 0; i < players.size(); i++)
		{
			r.addValue(Round::MoneySpent, i, moneySpent[i]);
			r.addValue(Round::MoneyGained, i, moneyGained[i]);
		}*/

		deferedPush(States::Game);
	}
	else
		deferedPush(States::Menu);
}

// General shop functionality to be used as a callback (after std::binding)
void ShopState::shopItem(unsigned int player, unsigned int row, Item::ItemID item, bool buy)
{
	const unsigned int moneyBefore = players[player].getMoney();

	// Unsuccessful
	if (buy && !players[player].buyItem(item, (unsigned int)(buyFactor * items[item].cost)))
		return;
	else if (!buy && !players[player].sellItem(item, (unsigned int)(sellFactor * items[item].cost)))
		return;

	const unsigned int moneyAfter = players[player].getMoney();

	// Register changes to round
	if (buy)
		moneySpent[player] += moneyBefore - moneyAfter;
	else
		moneyGained[player] += moneyAfter - moneyBefore;

	// Update labels
	sf::Text& 	moneyLabel = labelMap[MoneyCancel + ((player + 1) << column)];
	Button& 	itemButton = buttonMap[ShopItem + row + ((player + 1) << column)];

	changeAnchor(moneyLabel, Anchor::BottomLeft, Anchor::Bottom);
	changeAnchor(itemButton, Anchor::BottomLeft, Anchor::Bottom);

	moneyLabel.setString(toString(players[player].getMoney()));
	itemButton.setString(toString(players[player].getItemCount(item)));

	changeAnchor(moneyLabel, Anchor::Bottom, Anchor::BottomLeft);
	changeAnchor(itemButton, Anchor::Bottom, Anchor::BottomLeft);

	// The player is not finished with shopping after all
	buttonMap[NameReady + ((player + 1) << column)].deactivate();
	buttonMap[MoneyCancel + ((player + 1) << column)].deactivate();
}
