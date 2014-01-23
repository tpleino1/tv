#ifndef MB_STATESTACK_HH
#define MB_STATESTACK_HH

#include <vector>		// "stack"
#include <queue>		// queue
#include <map>			// map
#include <utility>		// pair & make_pair
#include <cassert>		// assert
#include <functional> 	// function

#include "StateId.hh"	// States::ID
#include "State.hh"

namespace sf
{
	class Time;
	class Event;
}

// A stack-like ADT for storing different States
class StateStack
{
	public:
		// Available operations
		enum Operation
		{
			Push,
			Pop,
			Clear
		};

		explicit StateStack(State::Resources r);
		// Copy and assignment constructors are disabled since
		// there should be only one StateStack instance.
		StateStack(const StateStack& s) = delete;
		StateStack& operator= (const StateStack& s) = delete;
		~StateStack();

		// Template function to add a creator for a state (see below)
		template <typename T>
		void addCreator(States::ID id);

		// Asynchronous public access methods
		void push(States::ID id);
		void pop();
		void clear();

		// Application loop functions
		void update(const sf::Time& dt);
		void draw();
		void handleEvent(const sf::Event& e);
		bool isEmpty();

	private:
		void processQueue();
		State::Pointer createState(States::ID);

		State::Resources res;
		std::vector<State::Pointer> states;
		std::queue<std::pair<Operation, States::ID>> actions;
		std::map<States::ID, std::function<State::Pointer()>> creators;
};


// Template function to add a creator for a state (see below)
// Creator functions may be invoked in createState (by processQueue, initially due to a push request)
template <typename T>
void StateStack::addCreator(States::ID id)
{
	// Should be inserted only once
	assert(creators.find(id) == creators.end());

	// Insert a lambda function that returns new <T>State
	creators.insert(std::make_pair(id, [this] () { return State::Pointer(new T(*this, res)); }));
	//creators[id] = [this] () { return State::Pointer(new T(*this, res)); };
}

#endif // MB_STATESTACK_HH
