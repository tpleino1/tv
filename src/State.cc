#include "State.hh"

#include "StateStack.hh"

// Resources Constructor
State::Resources::Resources(sf::RenderWindow& w,
	const FontContainer& f,
	const TextureContainer& t,
	const TextureRectContainer& tr,
	const ColorContainer& c,
	CSimpleIniA& i,
	KeyBindings::Container& k,
	MusicPlayer& m,
	SoundPlayer& s,
	MessageQueue& o,
	Session& g) :
		window(&w),
		fonts(&f),
		textures(&t),
		textureRects(&tr),
		colors(&c),
		config(&i),
		keyBinds(&k),
		music(&m),
		sound(&s),
		overlay(&o),
		game(&g)
{ }

// State Constructor
State::State(StateStack& s, Resources r) :
		states(&s), res(r)
{ }

// Destructor
State::~State()
{ }

// To be used with bottom->top iteration of draw
bool State::isFullScreen() const
{
	return true;
}

// Add push operation to the queue
void State::deferedPush(States::ID id)
{
	states->push(id);
}

// Add pop operation to the queue
void State::deferedPop()
{
	states->pop();
}

// Add clear operation to the queue
void State::deferedClear()
{
	states->clear();
}

// Return a copy of the resources
State::Resources State::getResources() const
{
	return res;
}
