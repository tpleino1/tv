#include "PlayerHUD.hh"

#include <cassert>
#include <cmath>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include "Utilities.hh"
#include "player.hh"

const float PlayerHUD::twoPi = 6.283185;
const sf::Time PlayerHUD::cycleTime = sf::seconds(2.f);

// Parameterless Constructor : Do not use
PlayerHUD::PlayerHUD() :
	player(nullptr),
	shownHealth(0),
	shownMoney(0),
	shownItemCount(0),
	shownItem(Item::None),
	frameColor(sf::Color(0, 0, 0, 0)),
	rectMap(),
	labelMap(),
	lowHealthTime(sf::Time::Zero),
	damageTakenTime(cycleTime),
	healthFullWidth(0),
	itemRects()
{ }

// Actual Constructor
PlayerHUD::PlayerHUD(const Player& p, const State::Resources r) :
		player(&p),
		shownHealth(p.getHealth()),
		shownMoney(p.getMoney()),
		shownItemCount(p.getItemCount(p.getCurrentItem())),
		shownItem(p.getCurrentItem()),
		frameColor((r.colors)->get("hudBG")),
		rectMap(),
		spriteMap(),
		labelMap(),
		lowHealthTime(sf::Time::Zero),
		damageTakenTime(cycleTime),
		healthFullWidth(0),
		itemRects()
{
	const float 		cfg_height 		= fromString<float>(					(r.config)->GetValue(	"HUD", "height"));
	const float 		cfg_width 		= fromString<float>(					(r.config)->GetValue(	"HUD", "playerWidth"));
	const float 		cfg_padding 	= fromString<float>(					(r.config)->GetValue(	"HUD", "padding"));
	const float 		cfg_paddingText = fromString<float>(					(r.config)->GetValue(	"HUD", "paddingText"));
	const float 		cfg_margin 		= fromString<float>(					(r.config)->GetValue(	"HUD", "margin"));
	const float 		cfg_share 		= fromString<float>( 					(r.config)->GetValue(	"HUD", "itemShare"));
	const unsigned int 	cfg_charSize 	= fromString<unsigned int>(				(r.config)->GetValue(	"HUD", "charSmall"));
	const float 		cfg_charComp 	= cfg_charSize * fromString<float >(	(r.config)->GetValue(	"HUD", "charCompSmall"));
	const sf::Color 	cfg_textColor 	= (r.colors)->get("hudText");
	const sf::Font& 	cfg_font		= (r.fonts)->get("default");

	// Used in const-like manner yet the compiler does not like const
	std::map<Elements, sf::FloatRect> cfg_dim({
			{PortraitBG, 	sf::FloatRect(0.f, 						 								 																0.f, 	 															cfg_height, 																cfg_height ) },
			{HealthBG, 		sf::FloatRect((cfg_height + cfg_margin),   																								0.f, 		 														(cfg_width - cfg_margin - cfg_height),										(0.5f * (cfg_height - cfg_margin)) ) },
			{ItemBG,		sf::FloatRect((cfg_height + cfg_margin),   																								(0.5f * (cfg_height - cfg_margin) + cfg_margin), 					(cfg_share * (cfg_width - cfg_margin - cfg_height - cfg_margin)), 			(0.5f * (cfg_height - cfg_margin)) ) },
			{MoneyBG,		sf::FloatRect((cfg_height + cfg_margin + cfg_share * (cfg_width - cfg_height - cfg_margin - cfg_margin) + cfg_margin), 					(0.5f * (cfg_height - cfg_margin) + cfg_margin), 					((1.f - cfg_share) * (cfg_width - cfg_margin - cfg_height - cfg_margin)), 	(0.5f * (cfg_height - cfg_margin)) ) },
			{Portrait,		sf::FloatRect(cfg_padding, 							 								 													cfg_padding, 						 								(cfg_height - 2.f * cfg_padding), 											(cfg_height - 2.f * cfg_padding) ) },
			{ItemIcon,		sf::FloatRect((cfg_height + cfg_margin + cfg_padding),		 																			(0.5f * (cfg_height - cfg_margin) + cfg_margin + cfg_padding), 		0.f, 																		(0.5f * (cfg_height - cfg_margin) - 2.f * cfg_padding) ) },
			{MoneyIcon,		sf::FloatRect((cfg_height + cfg_margin + cfg_share * (cfg_width - cfg_height - cfg_margin - cfg_margin) + cfg_margin + cfg_padding),  	(0.5f * (cfg_height - cfg_margin) + cfg_margin + cfg_padding), 		0.f, 																		(0.5f * (cfg_height - cfg_margin) - 2.f * cfg_padding) ) },
			{HealthBar,		sf::FloatRect((cfg_height + cfg_margin + cfg_padding),		 																			cfg_padding, 				 										(cfg_width - cfg_margin - cfg_height - 2.f * cfg_padding), 					(0.5f * (cfg_height - cfg_margin) - 2.f * cfg_padding) ) },
			{NameText,		sf::FloatRect((cfg_height + cfg_margin + cfg_padding + cfg_paddingText),  																(0.25f * (cfg_height - cfg_margin)) + cfg_charComp, 				0.f, 																		0.f	) },
			{HealthText,	sf::FloatRect((cfg_width - cfg_padding - cfg_paddingText), 				 																(0.25f * (cfg_height - cfg_margin)) + cfg_charComp, 				0.f, 																		0.f	) },
			{ItemText,		sf::FloatRect((cfg_height + cfg_margin + cfg_share * (cfg_width - cfg_height - cfg_margin) - cfg_paddingText - 0.5f * cfg_margin), 		(0.75f * (cfg_height - cfg_margin) + cfg_margin) + cfg_charComp, 	0.f, 																		0.f	) },
			{MoneyText,		sf::FloatRect(cfg_width - cfg_paddingText, 					 																			(0.75f * (cfg_height - cfg_margin) + cfg_margin) + cfg_charComp, 	0.f, 																		0.f	) }
		});
	healthFullWidth = cfg_dim[HealthBar].width;

	// Rects: Creation
	rectMap[PortraitBG] = sf::RectangleShape();
	rectMap[HealthBG] 	= sf::RectangleShape();
	rectMap[ItemBG] 	= sf::RectangleShape();
	rectMap[MoneyBG] 	= sf::RectangleShape();
	rectMap[HealthBar] 	= sf::RectangleShape();

	// Rects: Color, Position, Size
	for (auto& e : rectMap)
	{
		e.second.setSize(sf::Vector2f(cfg_dim[e.first].width, cfg_dim[e.first].height));
		e.second.setFillColor(frameColor);
		e.second.setPosition(sf::Vector2f(cfg_dim[e.first].left, cfg_dim[e.first].top));
	}
	rectMap[HealthBar].setFillColor(p.getColor());

	// itemRects
	const std::map<Item::ItemID, WeaponData> items = initializeWeaponData(r);
	itemRects[Item::None] = r.textureRects->get("w_default");
	for (auto& i : items)
		itemRects[i.first] = i.second.textureRect;

	// Sprites
	spriteMap[ItemIcon] = sf::Sprite(r.textures->get("misc"), r.textureRects->get("hudBag"));
	spriteMap[MoneyIcon] = sf::Sprite(r.textures->get("misc"), r.textureRects->get("hudMoney"));
	spriteMap[Portrait] = sf::Sprite(r.textures->get("items"), itemRects[shownItem]);

	for (auto& e : spriteMap)
	{
		float scale = cfg_dim[e.first].height / (float)e.second.getTextureRect().height;
		e.second.setScale(scale, scale);
		e.second.setPosition(sf::Vector2f(cfg_dim[e.first].left, cfg_dim[e.first].top));
	}

	// Labels: Creation
	labelMap[NameText] 		= sf::Text(p.getName(),					cfg_font, 	cfg_charSize);
	labelMap[HealthText] 	= sf::Text(toString(shownHealth), 		cfg_font, 	cfg_charSize);
	labelMap[ItemText] 		= sf::Text(toString(shownItemCount), 	cfg_font, 	cfg_charSize);
	labelMap[MoneyText] 	= sf::Text(toString(shownMoney), 		cfg_font, 	cfg_charSize);

	// Labels: Anchors
	changeAnchor(labelMap[NameText], 	Anchor::Left);
	changeAnchor(labelMap[HealthText], 	Anchor::Right);
	changeAnchor(labelMap[ItemText], 	Anchor::Right);
	changeAnchor(labelMap[MoneyText], 	Anchor::Right);

	// Labels: Color & Position
	for (auto& l : labelMap)
	{
		l.second.setColor(cfg_textColor);
		l.second.setPosition(sf::Vector2f(cfg_dim[l.first].left, cfg_dim[l.first].top));
	}
}

/*
// Destructor
PlayerHUD::~PlayerHUD()
{ }
*/

void PlayerHUD::update(const sf::Time& dt)
{
	unsigned int temp;

	temp = player->getHealth();
	if (temp != shownHealth)
	{
		shownHealth = temp;
		changeAnchor(labelMap[HealthText], Anchor::Left, Anchor::Right);
		labelMap[HealthText].setString(((shownHealth == 0) ? "DEAD" : ((shownHealth == 100) ? "" : toString(shownHealth))));
		changeAnchor(labelMap[HealthText], Anchor::Right, Anchor::Left);

		// Change health bar width
		sf::Vector2f s = rectMap[HealthBar].getSize();
		s.x = healthFullWidth * (shownHealth / 100.f);
		rectMap[HealthBar].setSize(s);

		// Flash BG
		damageTakenTime = sf::Time::Zero;
	}

	temp = player->getMoney();
	if (temp != shownMoney)
	{
		shownMoney = temp;
		changeAnchor(labelMap[MoneyText], Anchor::Left, Anchor::Right);
		labelMap[MoneyText].setString(toString(shownMoney));
		changeAnchor(labelMap[MoneyText], Anchor::Right, Anchor::Left);
	}

	Item::ItemID tempItem = player->getCurrentItem();
	if (tempItem != shownItem)
	{
		shownItem = tempItem;
		spriteMap[Portrait].setTextureRect(itemRects[shownItem]);
	}

	temp = player->getItemCount(shownItem);
	if (temp != shownItemCount)
	{
		shownItemCount = temp;
		changeAnchor(labelMap[ItemText], Anchor::Left, Anchor::Right);
		labelMap[ItemText].setString(toString(shownItemCount));
		changeAnchor(labelMap[ItemText], Anchor::Right, Anchor::Left);
	}

	// Change the color of the healthBar if necessary
	if (shownHealth > 0 && shownHealth < 100)
		lowHealthPulse(dt);

	if (damageTakenTime < 0.25f * cycleTime)
		damageTakenPulse(dt);
}

// Color pulsing to warn when shownHealth is low
void PlayerHUD::lowHealthPulse(const sf::Time& dt)
{
	sf::Color c = rectMap[HealthBar].getFillColor();

	// Basically sine that fluctuates between current and maximum shownHealth scaled to (0, 255)
	// 255 - A * [1 - cos(2 pi / Tcyc * t)], where A = 255 * (1 - shownHealth / 100) / 2
	// Limit pulseTime to one cycle
	c.a = round(255.f * (1.f - (1.f - shownHealth / 100.f) / 2.f * (1.f - cos(twoPi / cycleTime.asSeconds() * lowHealthTime.asSeconds()))));

	lowHealthTime += dt;
	if (lowHealthTime > cycleTime)
		lowHealthTime -= cycleTime;

	rectMap[HealthBar].setFillColor(c);
}

void PlayerHUD::damageTakenPulse(const sf::Time& dt)
{
	const sf::Color& prim = player->getColor();

	std::vector<unsigned short> p({prim.r, prim.g, prim.b, prim.a});
	const std::vector<unsigned short> s({frameColor.r, frameColor.g, frameColor.b, frameColor.a});
	for (std::vector<unsigned short>::size_type i = 0; i < p.size(); i++)
		if (p[i] != s[i]) // Basically sine that fluctuates between primary and colors, for each channel
			p[i] += round((float)(s[i] - p[i]) * 0.5f * (1.f - cos(twoPi / cycleTime.asSeconds() * 2.f * damageTakenTime.asSeconds())));

	damageTakenTime += dt;

	const std::vector<Elements> bg({PortraitBG, HealthBG, ItemBG, MoneyBG});
	for (auto e : bg)
		rectMap[e].setFillColor(sf::Color(p[0], p[1], p[2], p[3]));

}

// Render
void PlayerHUD::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();	// Apply transformations

	for (auto& e : rectMap)				// Draw backgrounds
		target.draw(e.second, states);
	for (auto& e : spriteMap)			// Draw icons
		target.draw(e.second, states);
	for (auto& e : labelMap)			// Draw labels
		target.draw(e.second, states);
}
