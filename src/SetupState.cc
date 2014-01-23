#include "SetupState.hh"

#include <cassert>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include "Utilities.hh"
#include "KeyBindings.hh"
#include "Session.hh"

//Constructor
SetupState::SetupState(StateStack& s, State::Resources r) :
		State(s, r),
		background(),
		labelMap(),
		buttonMap(),
		group((*(r.keyBinds))[0])
{
	// Config values
	const sf::Vector2f 	cfg_screenSize 	= fromString<sf::Vector2f>(r.config->GetValue("Video", 	"size"));
	const unsigned int 	cfg_charSize 	= fromString<unsigned int>(r.config->GetValue("Menu", 	"charSize"));
	const sf::Vector2f 	cfg_titlePos 	= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"titlePos"));
	const sf::Vector2f 	cfg_menuPos 	= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"menuPos"));
	const sf::Vector2f 	cfg_leftMargin 	= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"leftMargin"));
	const sf::Vector2f 	cfg_margin 		= fromString<sf::Vector2f>(r.config->GetValue("Menu", 	"margin"));
	const sf::Font& 	cfg_font 		= (r.fonts)->get("default");

	// Label positions
	std::map<Elements, sf::Vector2f> cfg_labelPos({
			{ Players,		sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 11.f * cfg_margin + 1.f * cfg_leftMargin },
			{ GameMode,	 	sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 10.f * cfg_margin + 1.f * cfg_leftMargin },
			{ Map, 		 	sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 9.f * cfg_margin + 1.f * cfg_leftMargin },
			{ Monsters, 	sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 8.f * cfg_margin + 1.f * cfg_leftMargin },
			{ Money, 		sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 7.f * cfg_margin + 1.f * cfg_leftMargin },
			{ Treasures, 	sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 6.f * cfg_margin + 1.f * cfg_leftMargin },
			{ Fog, 			sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 5.f * cfg_margin + 1.f * cfg_leftMargin },
			{ Rounds, 		sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 4.f * cfg_margin + 1.f * cfg_leftMargin },
			{ Length, 		sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 3.f * cfg_margin + 1.f * cfg_leftMargin }
		});

	// Button positions
	std::map<Elements, sf::Vector2f> cfg_buttonPos({
			{ Players,		sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 11.f * cfg_margin + 0.f * cfg_leftMargin },
			{ GameMode,	 	sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 10.f * cfg_margin + 0.f * cfg_leftMargin },
			{ Map, 		 	sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 9.f * cfg_margin + 0.f * cfg_leftMargin },
			{ Monsters, 	sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 8.f * cfg_margin + 0.f * cfg_leftMargin },
			{ Money, 		sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 7.f * cfg_margin + 0.f * cfg_leftMargin },
			{ Treasures, 	sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 6.f * cfg_margin + 0.f * cfg_leftMargin },
			{ Fog, 			sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 5.f * cfg_margin + 0.f * cfg_leftMargin },
			{ Rounds, 		sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 4.f * cfg_margin + 0.f * cfg_leftMargin },
			{ Length, 		sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 3.f * cfg_margin + 0.f * cfg_leftMargin },
			{ Continue, 	sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 1.f * cfg_margin + 0.f * cfg_leftMargin },
			{ Cancel, 		sf::Vector2f(0.f, cfg_screenSize.y) + cfg_menuPos + 0.f * cfg_margin + 0.f * cfg_leftMargin }
		});

	// Background
	background.setTexture((r.textures)->get("background"));

	(r.game)->clear();
	// Reset settings
	Session::Settings* o = &((r.game)->getSettings());
	CSimpleIniA::TNamesDepend keys;

	(r.config)->GetAllKeys("SetupDefaults", keys);
	for (auto i = keys.begin(); i != keys.end(); ++i)
		o->setValue(i->pItem, fromString<sf::Vector3<unsigned int>>((r.config)->GetValue("SetupDefaults", i->pItem)).x);

	// Labels: Creation
	labelMap[Title] 	= sf::Text("CUSTOMIZE", 												cfg_font, cfg_charSize);
	labelMap[Players] 	= sf::Text(toString(o->getValue("players")), 						cfg_font, cfg_charSize);
	labelMap[GameMode] 	= sf::Text(o->getValue("campaign") ? "CAMPAIGN" : "DEATHMATCH", 	cfg_font, cfg_charSize);
	labelMap[Map] 		= sf::Text("RANDOM", 													cfg_font, cfg_charSize);
	labelMap[Monsters] 	= sf::Text(toString(o->getValue("monsters")), 					cfg_font, cfg_charSize);
	labelMap[Money] 	= sf::Text(toString(o->getValue("cash")), 						cfg_font, cfg_charSize);
	labelMap[Treasures] = sf::Text(toString(o->getValue("treasures")), 					cfg_font, cfg_charSize);
	labelMap[Fog] 		= sf::Text(o->getValue("fog") ? "ON" : "OFF",						cfg_font, cfg_charSize);
	labelMap[Rounds] 	= sf::Text(toString(o->getValue("rounds")), 						cfg_font, cfg_charSize);
	labelMap[Length] 	= sf::Text(toString(sf::seconds(o->getValue("length"))), 			cfg_font, cfg_charSize);

	// Labels: Settings (anchor, position, color)
	for (auto& l : cfg_labelPos)
	{
		changeAnchor(labelMap[l.first], Anchor::BottomRight);
		labelMap[l.first].setPosition(l.second);
		labelMap[l.first].setColor((r.colors)->get("default"));
	}
	// Title as a special case
	changeAnchor(labelMap[Title], Anchor::BottomRight);
	labelMap[Title].setPosition(sf::Vector2f(cfg_screenSize.x, 0.f) + cfg_titlePos);
	labelMap[Title].setColor((r.colors)->get("default"));

	// Buttons: Creation
	buttonMap[Players] 		= Button("PLAYERS", 		cfg_font, *(r.colors));
	buttonMap[GameMode]		= Button("GAME MODE", 		cfg_font, *(r.colors));
	buttonMap[Map] 			= Button("MAP", 			cfg_font, *(r.colors));
	buttonMap[Monsters] 	= Button("MONSTERS", 		cfg_font, *(r.colors));
	buttonMap[Money] 		= Button("START MONEY", 	cfg_font, *(r.colors));
	buttonMap[Treasures] 	= Button("TREASURES", 		cfg_font, *(r.colors));
	buttonMap[Fog] 			= Button("FOG", 			cfg_font, *(r.colors));
	buttonMap[Rounds] 		= Button("ROUNDS", 			cfg_font, *(r.colors));
	buttonMap[Length] 		= Button("ROUND LENGTH", 	cfg_font, *(r.colors));
	buttonMap[Continue] 	= Button("CONTINUE", 		cfg_font, *(r.colors));
	buttonMap[Cancel] 		= Button("CANCEL", 			cfg_font, *(r.colors));

	// Buttons: Settings (text size, anchor, position, logical bind)
	for (auto& b : cfg_buttonPos)
	{
		buttonMap[b.first].setCharacterSize(cfg_charSize);
		changeAnchor(buttonMap[b.first], Anchor::BottomLeft);
		buttonMap[b.first].setPosition(b.second);
		group.add(buttonMap[b.first]);
	}
	// Only random maps are supported for now
	buttonMap[Map].setStatus(Button::Disabled);

	// Buttons: Callbacks
	createCallbacks();

	// Play background music
	(r.music)->play("menu");
}

// Destructor
SetupState::~SetupState()
{ }

// Render: BG, labels, buttons
void SetupState::draw()
{
	sf::RenderWindow& w = *(getResources().window);

	w.draw(background);
	for(auto& b : buttonMap)
		w.draw(b.second);
	for(auto& l : labelMap)
		w.draw(l.second);
}

// Nothing to update
bool SetupState::update(const sf::Time&)
{
	return false; // Stop updating here
}

// Escape as a special case, otherwise pass event to group
bool SetupState::handleEvent(const sf::Event& e)
{
	if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::Escape))
		group.selectEnd(true);
	else
		group.handleEvent(e);
	return false; // Stop event handling here
}

void SetupState::createCallbacks()
{
	buttonMap[Players].setCallback(		std::bind(&SetupState::changeSetting, this, "players", 		&labelMap[Players], 	[] (unsigned int v) { return toString(v); } ) );
	buttonMap[GameMode].setCallback(	std::bind(&SetupState::changeSetting, this, "campaign", 	&labelMap[GameMode], 	[] (unsigned int v) { return v ? "CAMPAIGN" : "DEATHMATCH"; } ) );
	buttonMap[Monsters].setCallback(	std::bind(&SetupState::changeSetting, this, "monsters", 	&labelMap[Monsters], 	[] (unsigned int v) { return toString(v); } ) );
	buttonMap[Money].setCallback(		std::bind(&SetupState::changeSetting, this, "cash", 		&labelMap[Money], 		[] (unsigned int v) { return toString(v); } ) );
	buttonMap[Treasures].setCallback(	std::bind(&SetupState::changeSetting, this, "treasures", 	&labelMap[Treasures], 	[] (unsigned int v) { return toString(v); } ) );
	buttonMap[Fog].setCallback(			std::bind(&SetupState::changeSetting, this, "fog", 			&labelMap[Fog], 		[] (unsigned int v) { return v ? "ON" : "OFF"; } ) );
	buttonMap[Rounds].setCallback(		std::bind(&SetupState::changeSetting, this, "rounds", 		&labelMap[Rounds], 		[] (unsigned int v) { return toString(v); } ) );
	buttonMap[Length].setCallback(		std::bind(&SetupState::changeSetting, this, "length", 		&labelMap[Length], 		[] (unsigned int v) { return toString(sf::seconds(v)); } ) );

	buttonMap[Continue].setCallback([this] ()
	{
		std::string name;
		sf::Color color;
		std::vector<std::pair<std::string, sf::Color>> nameColor;
		unsigned int p = (getResources().game)->getSettings().getValue("players");
		for (unsigned int i = 1; i <= p; i++)
		{
			name = (getResources().config)->GetValue("Players", std::string("name" + toString(i)).c_str());
			color = fromString<sf::Color>((getResources().config)->GetValue("Players", std::string("color" + toString(i)).c_str()));
			nameColor.push_back(std::make_pair(name, color));
		}

		(getResources().game)->start(nameColor, this->getResources().textures, this->getResources().textureRects);
		deferedClear();
		deferedPush(States::Shop);
	});

	buttonMap[Cancel].setCallback([this] ()
	{
		deferedClear();
		deferedPush(States::Menu);
	});

}


void SetupState::changeSetting(const std::string& s, sf::Text* t, std::function<std::string(unsigned int)> f)
{
	Session::Settings& r = (getResources().game)->getSettings();

	auto val = r.nextValue(s);

	changeAnchor(*t, Anchor::BottomLeft);
	t->setString(f(val));
	changeAnchor(*t, Anchor::BottomRight);
}

