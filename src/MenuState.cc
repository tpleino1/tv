#include "MenuState.hh"

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include "Utilities.hh"
#include "KeyBindings.hh"
#include "MusicPlayer.hh"
#include "datatables.hh"
//#include "player.hh"

class Player;

//Constructor
MenuState::MenuState(StateStack& s, State::Resources r) :
		State(s, r),
		title(),
		background(),
		buttons(),
		group((*(r.keyBinds))[0])
{
	// Config values
	const sf::Vector2f 	cfg_screenSize 	= fromString<sf::Vector2f>(r.config->GetValue("Video", "size"));
	const unsigned int 	cfg_charSize 	= fromString<unsigned int>(r.config->GetValue("Menu", "charSize"));
	const sf::Vector2f 	cfg_titlePos 	= fromString<sf::Vector2f>(r.config->GetValue("Menu", "titlePos"));
	const sf::Vector2f 	cfg_menuPos 	= fromString<sf::Vector2f>(r.config->GetValue("Menu", "menuPos"));
	const sf::Vector2f 	cfg_margin 		= fromString<sf::Vector2f>(r.config->GetValue("Menu", "margin"));
	const sf::Font& 	cfg_font 		= (r.fonts)->get("default");

	// G++-4.6 does no support array initialization with a {init-list}
	std::array<sf::Vector2f, 5> cfg_buttonPos;
	for (std::array<sf::Vector2f, 5>::size_type i = 0; i < cfg_buttonPos.size(); i++)
		cfg_buttonPos[cfg_buttonPos.size() - 1 - i] = sf::Vector2f(cfg_menuPos.x + i * cfg_margin.x, 	cfg_screenSize.y + cfg_menuPos.y + i * cfg_margin.y);

	// Background
	background.setTexture((r.textures)->get("background"));

	// Title
	title = sf::Text("MAIN MENU", cfg_font, cfg_charSize);
	title.setColor((r.colors)->get("default"));
	changeAnchor(title, Anchor::BottomRight);
	title.setPosition(sf::Vector2f(cfg_screenSize.x, 0.f) + cfg_titlePos);

	// Element creation
	buttons[0] = Button("SOLO CAMPAIGN", 	cfg_font, *(r.colors));
	buttons[1] = Button("QUICK DUEL", 		cfg_font, *(r.colors));
	buttons[2] = Button("CUSTOM GAME", 		cfg_font, *(r.colors));
	buttons[3] = Button("OPTIONS", 			cfg_font, *(r.colors));
	buttons[4] = Button("QUIT", 			cfg_font, *(r.colors));

	// Settings (auto did not work here, i required for cfg_buttonPos)
	for (std::array<Button, 5>::size_type i = 0; i < buttons.size(); i++)
	{
		buttons[i].setCharacterSize(cfg_charSize);
		changeAnchor(buttons[i], Anchor::BottomLeft);
		buttons[i].setPosition(cfg_buttonPos[i]);
		group.add(buttons[i]);
	}

	// Callbacks
	createCallbacks();

	// Play theme
	(r.music)->play("menu");
}

// Destructor
MenuState::~MenuState()
{ }

// Render: BG, Title, Buttons
void MenuState::draw()
{
	sf::RenderWindow& w = *(getResources().window);

	w.draw(background);
	w.draw(title);
	for(auto& b : buttons)
		w.draw(b);
}

// Nothing to update
bool MenuState::update(const sf::Time&)
{
	return false; // Stop updating here
}

// Escape as a special case, otherwise pass event to group
bool MenuState::handleEvent(const sf::Event& e)
{
	if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::Escape))
		group.selectEnd(true);
	else
		group.handleEvent(e);
	return false; // Stop event handling here
}

void MenuState::createCallbacks()
{
	buttons[0].setCallback([this] ()
	{
		const State::Resources 		r 	= getResources();
		Session* 					s 	= r.game;
		Session::Settings* 			o 	= &(s->getSettings());
		CSimpleIniA::TNamesDepend 	keys;

		(r.config)->GetAllKeys("SetupDefaults", keys);
		for (auto i = keys.begin(); i != keys.end(); ++i)
			o->setValue(i->pItem, fromString<sf::Vector3<unsigned int>>((r.config)->GetValue("SetupDefaults", i->pItem)).z);

		// Create players
		std::string name;
		sf::Color color;
		std::vector<std::pair<std::string, sf::Color>> nameColor;
		unsigned int p = o->getValue("players");
		for (unsigned int i = 1; i <= p; i++)
		{
			name = (r.config)->GetValue("Players", std::string("name" + toString(i)).c_str());
			color = fromString<sf::Color>((r.config)->GetValue("Players", std::string("color" + toString(i)).c_str()));
			nameColor.push_back(std::make_pair(name, color));
		}

		s->start(nameColor, r.textures, r.textureRects); // Start Game, create first round


		// Add all weapons times 20
		const std::map<Item::ItemID, WeaponData> items = initializeWeaponData(r);
		std::vector<Player>& players = s->getPlayers();
		for (unsigned int i = 0; i < p; i++)
			for (unsigned int j = 0; j < 10; j++)
				for (auto& k : items)
					if (!players[i].buyItem(k.first, 0))
						break;

		// Add some bogus info to display on results
		/*Round& l = s->getCurrentRound();
		l.addValue(Round::DamageGiven, 	0, 1);
		l.addValue(Round::DamageTaken, 	0, 2);
		l.addValue(Round::Kills, 		0, 4);
		l.addValue(Round::Deaths, 		0, 8);
		l.addValue(Round::MoneyGained, 	0, 16);
		l.addValue(Round::MoneySpent, 	0, 32);
		l.addValue(Round::ItemsUsed, 	0, 64);*/

		deferedClear();
		deferedPush(States::Game);
	});

	buttons[1].setCallback([this] ()
	{
		const State::Resources 		r 	= getResources();
		Session* 					s 	= r.game;
		Session::Settings* 			o 	= &(s->getSettings());
		CSimpleIniA::TNamesDepend 	keys;

		(r.config)->GetAllKeys("SetupDefaults", keys);
		for (auto i = keys.begin(); i != keys.end(); ++i)
			o->setValue(i->pItem, fromString<sf::Vector3<unsigned int>>((r.config)->GetValue("SetupDefaults", i->pItem)).y);

		// Create players
		std::string name;
		sf::Color color;
		std::vector<std::pair<std::string, sf::Color>> nameColor;
		unsigned int p = o->getValue("players");
		for (unsigned int i = 1; i <= p; i++)
		{
			name = (r.config)->GetValue("Players", std::string("name" + toString(i)).c_str());
			color = fromString<sf::Color>((r.config)->GetValue("Players", std::string("color" + toString(i)).c_str()));
			nameColor.push_back(std::make_pair(name, color));
		}

		s->start(nameColor, r.textures, r.textureRects); // Start Game, create first round

		// Add all weapons times 20
		const std::map<Item::ItemID, WeaponData> items = initializeWeaponData(r);
		std::vector<Player>& players = s->getPlayers();
		for (unsigned int i = 0; i < p; i++)
			for (unsigned int j = 0; j < 20; j++)
				for (auto& k : items)
					if (!players[i].buyItem(k.first, 0))
						break;

		// Add some bogus info to display on results
		/*Round& l = s->getCurrentRound();
		l.addValue(Round::DamageGiven, 	0, 1);
		l.addValue(Round::DamageTaken, 	0, 2);
		l.addValue(Round::Kills, 		0, 4);
		l.addValue(Round::Deaths, 		0, 8);
		l.addValue(Round::MoneyGained, 	0, 16);
		l.addValue(Round::MoneySpent, 	0, 32);
		l.addValue(Round::ItemsUsed, 	0, 64);*/

		deferedClear();
		deferedPush(States::Game);
	});

	buttons[2].setCallback([this] () { deferedClear(); deferedPush(States::Setup); });
	buttons[3].setCallback([this] () { deferedPush(States::Options); });
	buttons[4].setCallback([this] () { deferedClear(); });
}
