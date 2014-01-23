#include "Utilities.hh"

#include <map>
#include <cstdio> // sscanf for std::istream& operator>>(std::istream& is, sf::Time& t)
#include <cstdlib> // for rand()

// KeyBindings are load from file in two different classes, and
// saved in one class
void saveKeyBindings(const KeyBindings::Container& b, CSimpleIniA& c)
{
	const std::map<KeyBindings::Action, std::string> act({
			{ KeyBindings::MoveUp,		"MoveUp" },
			{ KeyBindings::MoveDown,	"MoveDown" },
			{ KeyBindings::MoveLeft,	"MoveLeft" },
			{ KeyBindings::MoveRight,	"MoveRight" },
			{ KeyBindings::UseFirst,	"UseFirst" },
			{ KeyBindings::UseSecond,	"UseSecond" },
		});

	for (auto& k : b)
		for (auto& a : act)
			c.SetValue( "Controls", std::string(a.second + toString(k.first)).c_str(), toString( k.second.getKey(a.first) ).c_str() );
}

void loadKeyBindings(KeyBindings::Container& b, const CSimpleIniA& c)
{
	const std::map<KeyBindings::Action, std::string> act({
			{ KeyBindings::MoveUp,		"MoveUp" },
			{ KeyBindings::MoveDown,	"MoveDown" },
			{ KeyBindings::MoveLeft,	"MoveLeft" },
			{ KeyBindings::MoveRight,	"MoveRight" },
			{ KeyBindings::UseFirst,	"UseFirst" },
			{ KeyBindings::UseSecond,	"UseSecond" },
		});

	for (auto& k : b)
		for (auto& a : act)
			k.second.setKey( a.first, fromString<sf::Keyboard::Key>( c.GetValue( "Controls", std::string(a.second + toString(k.first)).c_str() ) ) );
}

// IO-manipulation with sf::Color
std::ostream& operator<<(std::ostream& os, const sf::Color& c)
{
	os << (unsigned short)c.r << ' ' << (unsigned short)c.g << ' ' << (unsigned short)c.b << ' ' << (unsigned short)c.a;
	return os;
}

// IO-manipulation with sf::Color
std::istream& operator>>(std::istream& is, sf::Color& c)
{
	unsigned short temp;
	is >> temp;
		c.r = temp;
	is >> temp;
		c.g = temp;
	is >> temp;
		c.b = temp;
	is >> temp;
		c.a = temp;
	return is;
}

// IO-manipulation with sf::Time
std::ostream& operator<<(std::ostream& os, const sf::Time& t)
{
	bool sign = t < sf::Time::Zero;
	float s = (sign ? -1 : 1) * t.asSeconds();
	unsigned int m = (unsigned int)s / 60;
	s -= m * 60;
	os << (sign ? "-" : "") << m << ':' << ((s < 10) ? "0" : "");
	if (s > 1) 	// print partial seconds only when t <= 1 sec
		os << (unsigned int)s;
	else
		os << s;
	return os;
}

// IO-manipulation with sf::Time
std::istream& operator>>(std::istream& is, sf::Time& t)
{
	std::string temp;
	is >> temp;
	int m = 0;
	float s = 0;

	sscanf(temp.c_str(), "%d:%f", &m, &s);

	t = sf::seconds(m * 60 + (m < 0 ? -1 : 1) * s);
	return is;
}

// Random number generation with limits
float floatRNG(float l, float u)
{
	return l + (u - l) * (float)rand() / (float)RAND_MAX;
}

int intRNG(int l, int u)
{
	return rand() % (u - l) + l;
}

// IO-manipulation with sf::Keyboard::Key
std::ostream& operator<<(std::ostream& os, const sf::Keyboard::Key& k)
{
	switch (k) // also a conversion map could be used
	{
	case sf::Keyboard::A: 			os << "A"; 			break;
	case sf::Keyboard::B: 			os << "B"; 			break;
	case sf::Keyboard::C: 			os << "C"; 			break;
	case sf::Keyboard::D: 			os << "D"; 			break;
	case sf::Keyboard::E: 			os << "E"; 			break;
	case sf::Keyboard::F: 			os << "F"; 			break;
	case sf::Keyboard::G: 			os << "G"; 			break;
	case sf::Keyboard::H: 			os << "H"; 			break;
	case sf::Keyboard::I: 			os << "I"; 			break;
	case sf::Keyboard::J: 			os << "J"; 			break;
	case sf::Keyboard::K: 			os << "K"; 			break;
	case sf::Keyboard::L: 			os << "L"; 			break;
	case sf::Keyboard::M: 			os << "M"; 			break;
	case sf::Keyboard::N: 			os << "N"; 			break;
	case sf::Keyboard::O: 			os << "O"; 			break;
	case sf::Keyboard::P: 			os << "P"; 			break;
	case sf::Keyboard::Q: 			os << "Q"; 			break;
	case sf::Keyboard::R: 			os << "R"; 			break;
	case sf::Keyboard::S: 			os << "S"; 			break;
	case sf::Keyboard::T: 			os << "T"; 			break;
	case sf::Keyboard::U: 			os << "U"; 			break;
	case sf::Keyboard::V: 			os << "V"; 			break;
	case sf::Keyboard::W: 			os << "W"; 			break;
	case sf::Keyboard::X: 			os << "X"; 			break;
	case sf::Keyboard::Y: 			os << "Y"; 			break;
	case sf::Keyboard::Z: 			os << "Z"; 			break;
	case sf::Keyboard::Num0: 		os << "Num0"; 		break;
	case sf::Keyboard::Num1: 		os << "Num1"; 		break;
	case sf::Keyboard::Num2: 		os << "Num2"; 		break;
	case sf::Keyboard::Num3: 		os << "Num3"; 		break;
	case sf::Keyboard::Num4: 		os << "Num4"; 		break;
	case sf::Keyboard::Num5: 		os << "Num5"; 		break;
	case sf::Keyboard::Num6: 		os << "Num6"; 		break;
	case sf::Keyboard::Num7: 		os << "Num7"; 		break;
	case sf::Keyboard::Num8: 		os << "Num8"; 		break;
	case sf::Keyboard::Num9: 		os << "Num9"; 		break;
	case sf::Keyboard::Escape: 		os << "Escape"; 	break;
	case sf::Keyboard::LControl: 	os << "LControl"; 	break;
	case sf::Keyboard::LShift: 		os << "LShift"; 	break;
	case sf::Keyboard::LAlt: 		os << "LAlt"; 		break;
	case sf::Keyboard::LSystem: 	os << "LSystem"; 	break;
	case sf::Keyboard::RControl: 	os << "RControl"; 	break;
	case sf::Keyboard::RShift: 		os << "RShift"; 	break;
	case sf::Keyboard::RAlt: 		os << "RAlt"; 		break;
	case sf::Keyboard::RSystem: 	os << "RSystem"; 	break;
	case sf::Keyboard::Menu: 		os << "Menu"; 		break;
	case sf::Keyboard::LBracket: 	os << "LBracket"; 	break;
	case sf::Keyboard::RBracket: 	os << "RBracket"; 	break;
	case sf::Keyboard::SemiColon: 	os << "SemiColon"; 	break;
	case sf::Keyboard::Comma: 		os << "Comma"; 		break;
	case sf::Keyboard::Period: 		os << "Period"; 	break;
	case sf::Keyboard::Quote: 		os << "Quote"; 		break;
	case sf::Keyboard::Slash: 		os << "Slash"; 		break;
	case sf::Keyboard::BackSlash: 	os << "BackSlash"; 	break;
	case sf::Keyboard::Tilde: 		os << "Tilde"; 		break;
	case sf::Keyboard::Equal: 		os << "Equal"; 		break;
	case sf::Keyboard::Dash: 		os << "Dash"; 		break;
	case sf::Keyboard::Space: 		os << "Space"; 		break;
	case sf::Keyboard::Return: 		os << "Return"; 	break;
	case sf::Keyboard::BackSpace: 	os << "BackSpace"; 	break;
	case sf::Keyboard::Tab: 		os << "Tab"; 		break;
	case sf::Keyboard::PageUp: 		os << "PageUp"; 	break;
	case sf::Keyboard::PageDown: 	os << "PageDown"; 	break;
	case sf::Keyboard::End: 		os << "End"; 		break;
	case sf::Keyboard::Home: 		os << "Home"; 		break;
	case sf::Keyboard::Insert: 		os << "Insert"; 	break;
	case sf::Keyboard::Delete: 		os << "Delete"; 	break;
	case sf::Keyboard::Add: 		os << "Add"; 		break;
	case sf::Keyboard::Subtract: 	os << "Subtract"; 	break;
	case sf::Keyboard::Multiply: 	os << "Multiply"; 	break;
	case sf::Keyboard::Divide: 		os << "Divide"; 	break;
	case sf::Keyboard::Left: 		os << "Left"; 		break;
	case sf::Keyboard::Right: 		os << "Right"; 		break;
	case sf::Keyboard::Up: 			os << "Up"; 		break;
	case sf::Keyboard::Down: 		os << "Down"; 		break;
	case sf::Keyboard::Numpad0: 	os << "Numpad0"; 	break;
	case sf::Keyboard::Numpad1: 	os << "Numpad1"; 	break;
	case sf::Keyboard::Numpad2: 	os << "Numpad2"; 	break;
	case sf::Keyboard::Numpad3: 	os << "Numpad3"; 	break;
	case sf::Keyboard::Numpad4: 	os << "Numpad4"; 	break;
	case sf::Keyboard::Numpad5: 	os << "Numpad5"; 	break;
	case sf::Keyboard::Numpad6: 	os << "Numpad6"; 	break;
	case sf::Keyboard::Numpad7: 	os << "Numpad7"; 	break;
	case sf::Keyboard::Numpad8: 	os << "Numpad8"; 	break;
	case sf::Keyboard::Numpad9: 	os << "Numpad9"; 	break;
	case sf::Keyboard::F1: 			os << "F1"; 		break;
	case sf::Keyboard::F2: 			os << "F2"; 		break;
	case sf::Keyboard::F3: 			os << "F3"; 		break;
	case sf::Keyboard::F4: 			os << "F4"; 		break;
	case sf::Keyboard::F5: 			os << "F5"; 		break;
	case sf::Keyboard::F6: 			os << "F6"; 		break;
	case sf::Keyboard::F7: 			os << "F7"; 		break;
	case sf::Keyboard::F8: 			os << "F8"; 		break;
	case sf::Keyboard::F9: 			os << "F9"; 		break;
	case sf::Keyboard::F10: 		os << "F10"; 		break;
	case sf::Keyboard::F11: 		os << "F11"; 		break;
	case sf::Keyboard::F12: 		os << "F12"; 		break;
	case sf::Keyboard::F13: 		os << "F13"; 		break;
	case sf::Keyboard::F14: 		os << "F14"; 		break;
	case sf::Keyboard::F15: 		os << "F15"; 		break;
	case sf::Keyboard::Pause: 		os << "Pause"; 		break;
	default:						os << "Unknown"; 	break;
	}

	return os;
}

// IO-manipulation with sf::Keyboard::Key
std::istream& operator>>(std::istream& is, sf::Keyboard::Key& k)
{
	std::string temp;
	is >> temp;

	// Conversion map (switch case not possible)
	const std::map<std::string, sf::Keyboard::Key> c(
	{
		{"A", 			sf::Keyboard::A},
		{"B", 			sf::Keyboard::B},
		{"C", 			sf::Keyboard::C},
		{"D", 			sf::Keyboard::D},
		{"E", 			sf::Keyboard::E},
		{"F", 			sf::Keyboard::F},
		{"G", 			sf::Keyboard::G},
		{"H", 			sf::Keyboard::H},
		{"I", 			sf::Keyboard::I},
		{"J", 			sf::Keyboard::J},
		{"K", 			sf::Keyboard::K},
		{"L", 			sf::Keyboard::L},
		{"M", 			sf::Keyboard::M},
		{"N", 			sf::Keyboard::N},
		{"O", 			sf::Keyboard::O},
		{"P", 			sf::Keyboard::P},
		{"Q", 			sf::Keyboard::Q},
		{"R", 			sf::Keyboard::R},
		{"S", 			sf::Keyboard::S},
		{"T", 			sf::Keyboard::T},
		{"U", 			sf::Keyboard::U},
		{"V", 			sf::Keyboard::V},
		{"W", 			sf::Keyboard::W},
		{"X", 			sf::Keyboard::X},
		{"Y", 			sf::Keyboard::Y},
		{"Z", 			sf::Keyboard::Z},
		{"Num0", 		sf::Keyboard::Num0},
		{"Num1", 		sf::Keyboard::Num1},
		{"Num2", 		sf::Keyboard::Num2},
		{"Num3", 		sf::Keyboard::Num3},
		{"Num4", 		sf::Keyboard::Num4},
		{"Num5", 		sf::Keyboard::Num5},
		{"Num6", 		sf::Keyboard::Num6},
		{"Num7", 		sf::Keyboard::Num7},
		{"Num8", 		sf::Keyboard::Num8},
		{"Num9", 		sf::Keyboard::Num9},
		{"Escape", 		sf::Keyboard::Escape},
		{"LControl", 	sf::Keyboard::LControl},
		{"LShift", 		sf::Keyboard::LShift},
		{"LAlt", 		sf::Keyboard::LAlt},
		{"LSystem", 	sf::Keyboard::LSystem},
		{"RControl", 	sf::Keyboard::RControl},
		{"RShift", 		sf::Keyboard::RShift},
		{"RAlt", 		sf::Keyboard::RAlt},
		{"RSystem", 	sf::Keyboard::RSystem},
		{"Menu", 		sf::Keyboard::Menu},
		{"LBracket", 	sf::Keyboard::LBracket},
		{"RBracket", 	sf::Keyboard::RBracket},
		{"SemiColon", 	sf::Keyboard::SemiColon},
		{"Comma", 		sf::Keyboard::Comma},
		{"Period", 		sf::Keyboard::Period},
		{"Quote", 		sf::Keyboard::Quote},
		{"Slash", 		sf::Keyboard::Slash},
		{"BackSlash", 	sf::Keyboard::BackSlash},
		{"Tilde", 		sf::Keyboard::Tilde},
		{"Equal", 		sf::Keyboard::Equal},
		{"Dash", 		sf::Keyboard::Dash},
		{"Space", 		sf::Keyboard::Space},
		{"Return", 		sf::Keyboard::Return},
		{"BackSpace", 	sf::Keyboard::BackSpace},
		{"Tab", 		sf::Keyboard::Tab},
		{"PageUp", 		sf::Keyboard::PageUp},
		{"PageDown", 	sf::Keyboard::PageDown},
		{"End", 		sf::Keyboard::End},
		{"Home", 		sf::Keyboard::Home},
		{"Insert", 		sf::Keyboard::Insert},
		{"Delete", 		sf::Keyboard::Delete},
		{"Add", 		sf::Keyboard::Add},
		{"Subtract", 	sf::Keyboard::Subtract},
		{"Multiply", 	sf::Keyboard::Multiply},
		{"Divide", 		sf::Keyboard::Divide},
		{"Left", 		sf::Keyboard::Left},
		{"Right", 		sf::Keyboard::Right},
		{"Up", 			sf::Keyboard::Up},
		{"Down", 		sf::Keyboard::Down},
		{"Numpad0", 	sf::Keyboard::Numpad0},
		{"Numpad1", 	sf::Keyboard::Numpad1},
		{"Numpad2", 	sf::Keyboard::Numpad2},
		{"Numpad3", 	sf::Keyboard::Numpad3},
		{"Numpad4", 	sf::Keyboard::Numpad4},
		{"Numpad5", 	sf::Keyboard::Numpad5},
		{"Numpad6", 	sf::Keyboard::Numpad6},
		{"Numpad7", 	sf::Keyboard::Numpad7},
		{"Numpad8", 	sf::Keyboard::Numpad8},
		{"Numpad9", 	sf::Keyboard::Numpad9},
		{"F1", 			sf::Keyboard::F1},
		{"F2", 			sf::Keyboard::F2},
		{"F3", 			sf::Keyboard::F3},
		{"F4", 			sf::Keyboard::F4},
		{"F5", 			sf::Keyboard::F5},
		{"F6", 			sf::Keyboard::F6},
		{"F7", 			sf::Keyboard::F7},
		{"F8", 			sf::Keyboard::F8},
		{"F9", 			sf::Keyboard::F9},
		{"F10", 		sf::Keyboard::F10},
		{"F11", 		sf::Keyboard::F11},
		{"F12", 		sf::Keyboard::F12},
		{"F13", 		sf::Keyboard::F13},
		{"F14", 		sf::Keyboard::F14},
		{"F15", 		sf::Keyboard::F15},
		{"Pause", 		sf::Keyboard::Pause}
	});

	auto iter = c.find(temp);
	k = (iter != c.end()) ? iter->second : sf::Keyboard::Unknown;

	return is;
}
