#include "StateStack.hh"

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

// Constructor
StateStack::StateStack(State::Resources r) : res(r), states(), actions(), creators()
{
	states.reserve(8);	// To avoid later relocation
}

// Destruction
StateStack::~StateStack()
{ }

// Queue a push operation
void StateStack::push(States::ID id)
{
	actions.push(std::make_pair(Push, id));
}

// Queue a pop operation
void StateStack::pop()
{
	actions.push(std::make_pair(Pop, States::None));
}

// Queue a clear operation
void StateStack::clear()
{
	actions.push(std::make_pair(Clear, States::None));
}

// Update states on top
void StateStack::update(const sf::Time& dt)
{
	// Loop from end to front (top -> bottom)
	for(auto iter = states.rbegin(); iter != states.rend(); iter++)
		if (!(*iter)->update(dt))	// (*iter) == State::Pointer
			break;

	// Update may include state changes
	processQueue();
}

// Draw states on top
// Can be called to empty
void StateStack::draw()
{
	// Find the top-most full screen state
	std::vector<State::Pointer>::size_type i = 0;
	for(auto iter = states.rbegin(); iter != states.rend(); iter++)
	{
		i++;
		if ((*iter)->isFullScreen())	// (*iter) == State::Pointer
			break;
	}

	// Draw that and every state on top of that
	for (i = states.size() - i; i < states.size(); i++)
		states.at(i)->draw();
}

// Process event on the states on top
void StateStack::handleEvent(const sf::Event& e)
{
	// Loop from end to front (top -> bottom)
	for(auto iter = states.rbegin(); iter != states.rend(); iter++)
		if (!(*iter)->handleEvent(e))	// (*iter) == State::Pointer
			break;
	// Events may result in state changes
	processQueue();
}

// Used for application-wide exit condition
bool StateStack::isEmpty()
{
	return states.empty();
}

// Process queued stack operations (Push, Pop and Clear)
void StateStack::processQueue()
{
	while(!actions.empty())
	{
		auto op = actions.front(); // std::pair<Operation, State::Pointer>
		switch (op.first)
		{
		case Push:
			states.push_back(createState(op.second));
			break;

		case Pop:
			states.pop_back();
			break;

		case Clear:
			states.clear();
			break;

		default:
			break;
		}
		actions.pop();
	}
}

// Calls to corresponding creator (used as by processQueue)
State::Pointer StateStack::createState(States::ID id)
{
	auto c = creators.find(id);		// Find corresponding creator
	assert(c != creators.end());	// Should always be found

	return c->second(); // Call creator & return the returned State object
}
