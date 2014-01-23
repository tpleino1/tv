#include "PauseState.hh"

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include "Utilities.hh"
#include "KeyBindings.hh"
#include "MusicPlayer.hh"

//Constructor
PauseState::PauseState(StateStack& s, State::Resources r) :
		State(s, r),
		title(),
		background(),
		buttons(),
		group((*(r.keyBinds))[0])
{
	// Config values
	const sf::Vector2f 	cfg_screenSize 	= fromString<sf::Vector2f>(r.config->GetValue("Video", "size"));
	const unsigned int 	cfg_charSize 	= fromString<unsigned int>(r.config->GetValue("Menu", "charSize"));
	const sf::Vector2f 	cfg_titlePos	= fromString<sf::Vector2f>(r.config->GetValue("Menu", "titlePos"));
	const sf::Vector2f 	cfg_menuPos 	= fromString<sf::Vector2f>(r.config->GetValue("Menu", "menuPos"));
	const sf::Vector2f 	cfg_margin 		= fromString<sf::Vector2f>(r.config->GetValue("Menu", "margin"));
	const sf::Font& 	cfg_font 		= (r.fonts)->get("default");

	// G++-4.6 does no support array initialization with a {init-list}
	std::array<sf::Vector2f, 3> cfg_buttonPos;
	for (std::array<sf::Vector2f, 3>::size_type i = 0; i < cfg_buttonPos.size(); i++)
		cfg_buttonPos[cfg_buttonPos.size() - 1 - i] = sf::Vector2f(cfg_menuPos.x + i * cfg_margin.x, 	cfg_screenSize.y + cfg_menuPos.y + i * cfg_margin.y);

	// Background
	background.setTexture((r.textures)->get("background"));
	background.setColor(sf::Color(255, 255, 255, 128));

	// Title
	title = sf::Text("GAME PAUSED", cfg_font, cfg_charSize);
	title.setColor((r.colors)->get("default"));
	changeAnchor(title, Anchor::BottomRight);
	title.setPosition(sf::Vector2f(cfg_screenSize.x, 0.f) + cfg_titlePos);

	// Element creation
	buttons[0] = Button("RESUME", cfg_font, *(r.colors));
	buttons[1] = Button("MAIN MENU", cfg_font, *(r.colors));
	buttons[2] = Button("QUIT", cfg_font, *(r.colors));

	// Settings (auto issued a warning here)
	for (std::array<Button, 3>::size_type i = 0; i < buttons.size(); i++)
	{
		buttons[i].setCharacterSize(cfg_charSize);
		changeAnchor(buttons[i], Anchor::BottomLeft);
		buttons[i].setPosition(cfg_buttonPos[i]);
		group.add(buttons[i]);
	}

	// Callbacks
	buttons[0].setCallback([this] () { deferedPop(); });
	buttons[1].setCallback([this] () { deferedClear(); deferedPush(States::Menu); });
	buttons[2].setCallback([this] () { deferedClear(); });

	// Pause theme
	(r.music)->setStatus(MusicPlayer::Paused);
}

// Destructor: Resume theme
PauseState::~PauseState()
{
	(getResources().music)->setStatus(MusicPlayer::Playing);
}

// PauseState is somewhat transparent -> need also to draw the state below
bool PauseState::isFullScreen() const
{
	return false;
}

// Render: BG, Title, Buttons
void PauseState::draw()
{
	sf::RenderWindow& w = *(getResources().window);

	w.draw(background);
	w.draw(title);
	for(auto& b : buttons)
		w.draw(b);
}

// Nothing to update
bool PauseState::update(const sf::Time&)
{
	return false; // Stop updating here
}

// Escape as a special case, otherwise pass event to group
bool PauseState::handleEvent(const sf::Event& e)
{
	if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::Escape))
		group.selectEnd(true);
	else
		group.handleEvent(e);
	return false; // Stop event handling here
}
