#include "ClockHUD.hh"

#include <cmath>
#include <vector>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include "Utilities.hh"

// Pulsing-related consts
const float ClockHUD::twoPi = 6.283185;
const sf::Time ClockHUD::cycleTime = sf::seconds(2.f); // to be changed
const sf::Time ClockHUD::warningTime = sf::seconds(15.f); // to be changed

// Complete constructor
ClockHUD::ClockHUD(const sf::Time& t, const World& w, const State::Resources r) :
		countTime(true),
		hourglass(t),
		pulseTime(sf::Time::Zero),
		world(&w),
		shownTime((unsigned int)t.asSeconds()),
		shownTreasures(w.getTreasureValue()),
		shownMonsters(w.getMonsterCount()),
		rectMap(),
		spriteMap(),
		labelMap(),
		primaryColor((r.colors)->get("hudText")),
		secondaryColor((r.colors)->get("hudHighlight"))

{
	// Config values
	const float 		cfg_height 		= fromString<float>(					(r.config)->GetValue(	"HUD", "height"));
	const float 		cfg_width 		= fromString<float>(					(r.config)->GetValue(	"HUD", "clockWidth"));
	const float 		cfg_share 		= fromString<float>(					(r.config)->GetValue(	"HUD", "clockShare"));
	const float 		cfg_padding 	= fromString<float>(					(r.config)->GetValue(	"HUD", "paddingText"));
	const float 		cfg_margin 		= fromString<float>(					(r.config)->GetValue(	"HUD", "margin"));
	const unsigned int 	cfg_charSmall 	= fromString<unsigned int>(				(r.config)->GetValue(	"HUD", "charSmall"));
	const unsigned int 	cfg_charBig 	= fromString<unsigned int>(				(r.config)->GetValue(	"HUD", "charBig"));
	const float 		cfg_compSmall 	= cfg_charSmall * fromString<float>(	(r.config)->GetValue(	"HUD", "charCompSmall"));
	const float 		cfg_compBig 	= cfg_charBig * fromString<float>(		(r.config)->GetValue(	"HUD", "charCompBig"));
	const sf::Color 	cfg_frameColor	= (r.colors)->get("hudBG");
	const sf::Color 	cfg_textColor 	= (r.colors)->get("hudText");
	const sf::Font& 	cfg_font 		= (r.fonts)->get("default");

	// Used in const-like manner yet the compiler does not like const
	std::map<Elements, sf::FloatRect> cfg_dim({
			{ClockBG, 		sf::FloatRect(0.f, 																	0.f, 																cfg_share * (cfg_width - cfg_margin), 						cfg_height) },
			{MonstersBG, 	sf::FloatRect(cfg_share * (cfg_width - cfg_margin) + cfg_margin, 					0.f, 																(1 - cfg_share) * (cfg_width - cfg_margin),					(0.5 * (cfg_height - cfg_margin)) ) },
			{TreasuresBG,	sf::FloatRect(cfg_share * (cfg_width - cfg_margin) + cfg_margin, 					(0.5 * (cfg_height - cfg_margin) + cfg_margin), 					(1 - cfg_share) * (cfg_width - cfg_margin),					(0.5 * (cfg_height - cfg_margin)) ) },
			{MonstersIcon, 	sf::FloatRect(cfg_share * (cfg_width - cfg_margin) + cfg_margin + cfg_padding, 		cfg_padding, 														0.f,														(0.5 * (cfg_height - cfg_margin) - 2.f * cfg_padding) ) },
			{TreasuresIcon,	sf::FloatRect(cfg_share * (cfg_width - cfg_margin) + cfg_margin + cfg_padding, 		(0.5 * (cfg_height - cfg_margin) + cfg_margin + cfg_padding), 		0.f,														(0.5 * (cfg_height - cfg_margin) - 2.f * cfg_padding) ) },
			{ClockText,		sf::FloatRect(0.5 * cfg_share * (cfg_width - cfg_margin), 							(0.5 * cfg_height + cfg_compBig), 									0.f, 														0.f	) },
			{MonstersText,	sf::FloatRect(cfg_width - cfg_padding, 												(0.25 * (cfg_height - cfg_margin) + cfg_compSmall), 				0.f,														0.f	) },
			{TreasuresText,	sf::FloatRect(cfg_width - cfg_padding,												(0.75 * (cfg_height - cfg_margin) + cfg_margin + cfg_compSmall),	0.f,														0.f	) }
		});

	// Rects: Creation
	rectMap[ClockBG] 		= sf::RectangleShape();
	rectMap[TreasuresBG] 	= sf::RectangleShape();
	rectMap[MonstersBG] 	= sf::RectangleShape();

	// Rects: Settings
	for (auto& e : rectMap)
	{
		e.second.setSize(sf::Vector2f(cfg_dim[e.first].width, cfg_dim[e.first].height));
		e.second.setPosition(sf::Vector2f(cfg_dim[e.first].left, cfg_dim[e.first].top));
		e.second.setFillColor(cfg_frameColor);
	}

	spriteMap[MonstersIcon] = sf::Sprite(r.textures->get("misc"), r.textureRects->get("hudMonster"));
	spriteMap[TreasuresIcon] = sf::Sprite(r.textures->get("misc"), r.textureRects->get("hudMoney"));

	for (auto& e : spriteMap)
	{
		float scale = cfg_dim[e.first].height / (float)e.second.getTextureRect().height;
		e.second.setScale(scale, scale);
		e.second.setPosition(sf::Vector2f(cfg_dim[e.first].left, cfg_dim[e.first].top));
	}

	// Labels: Creation
	labelMap[ClockText] 	= sf::Text(toString(hourglass),			cfg_font, cfg_charBig);
	labelMap[TreasuresText] = sf::Text(toString(shownTreasures), 	cfg_font, cfg_charSmall);
	labelMap[MonstersText] 	= sf::Text(toString(shownMonsters), 	cfg_font, cfg_charSmall);

	// Labels: Anchor
	changeAnchor(labelMap[ClockText], 		Anchor::Center);
	changeAnchor(labelMap[TreasuresText], 	Anchor::Right);
	changeAnchor(labelMap[MonstersText], 	Anchor::Right);

	// Labels: Color, Position
	for (auto& e : labelMap)
	{
		e.second.setPosition(sf::Vector2f(cfg_dim[e.first].left, cfg_dim[e.first].top));
		e.second.setColor(cfg_textColor);
	}
}

//
void ClockHUD::update(const sf::Time& dt)
{
	// Set time
	hourglass = (hourglass < dt) ? sf::Time::Zero : hourglass - dt;

	unsigned int temp; // Temp value to test for changes

	temp = world->getTreasureValue(); // Load from World
	if (temp != shownTreasures)
	{
		shownTreasures = temp;
		changeAnchor(labelMap[TreasuresText], 	Anchor::Left, Anchor::Right);
		labelMap[TreasuresText].setString(toString(shownTreasures));
		changeAnchor(labelMap[TreasuresText], 	Anchor::Right, 	Anchor::Left);
	}

	temp = world->getMonsterCount(); // Load from World
	if (temp != shownMonsters)
	{
		shownMonsters = temp;
		changeAnchor(labelMap[MonstersText], 	Anchor::Left, Anchor::Right);
		labelMap[MonstersText].setString(toString(shownMonsters));
		changeAnchor(labelMap[MonstersText], 	Anchor::Right, 	Anchor::Left);
	}
	
	if (countTime)
	{
		temp = (unsigned int)hourglass.asSeconds();
		if (temp != shownTime)
		{
			shownTime = temp;
			changeAnchor(labelMap[ClockText], 		Anchor::Left, Anchor::Center);
			labelMap[ClockText].setString(toString(sf::seconds(shownTime)));
			changeAnchor(labelMap[ClockText], 		Anchor::Center, Anchor::Left);
		}
	}

	// Pulse if time running out
	if (hourglass < warningTime)
		colorPulse(dt);
}

// Change clock text periodically if time is about to run out
void ClockHUD::colorPulse(const sf::Time& dt)
{
	std::vector<unsigned short> p({primaryColor.r, primaryColor.g, primaryColor.b, primaryColor.a});
	const std::vector<unsigned short> s({secondaryColor.r, secondaryColor.g, secondaryColor.b, secondaryColor.a});
	for (std::vector<unsigned short>::size_type i = 0; i < p.size(); i++)
		if (p[i] != s[i]) // Basically sine that fluctuates between primary and colors, for each channel
			p[i] += round((float)(s[i] - p[i]) * 0.5f * (1.f - cos(twoPi / cycleTime.asSeconds() * pulseTime.asSeconds())));
	labelMap[ClockText].setColor(sf::Color(p[0], p[1], p[2], p[3]));

	pulseTime = sf::seconds(fmod(pulseTime.asSeconds() + dt.asSeconds(), cycleTime.asSeconds()));
}

// Rendering function: apply transformations, draw backgrounds and labels in this order
void ClockHUD::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform(); // Apply transformations

	for (auto& e : rectMap)				// Draw backgrounds
		target.draw(e.second, states);
	for (auto& e : spriteMap)			// Draw icons
		target.draw(e.second, states);
	for (auto& e : labelMap)			// Draw labels
		target.draw(e.second, states);
}

// Basic "getter" for the remaining time
// Usable for end-condition
const sf::Time& ClockHUD::getTime() const
{
	return hourglass;
}

void ClockHUD::setTime(const sf::Time& t)
{
	hourglass = t;
}

bool ClockHUD::isCounting() const
{
	return countTime;
}

void ClockHUD::setCounting(bool c)
{
	countTime = c;
}
