#ifndef MB_BUTTONGROUP_HH
#define MB_BUTTONGROUP_HH

#include <vector>

#include "Button.hh"
#include "KeyBindings.hh"

namespace sf
{
	class Event;
}

// Does not actually own anything, just a logical bind
// for sequentially arranged Buttons
class ButtonGroup
{
	public:
		// Constructors
		ButtonGroup();
		explicit ButtonGroup(const KeyBindings& b);

		// Needed when parameterless constructor is used
		void setKeyBindings(const KeyBindings& b);

		// Logically bind a button (added to the end of the chain)
        void add(Button& b);

		// Checks whether a registered button has been pressed and acts accordingly
        void handleEvent(const sf::Event& e);

		// One selection function offered as an "escape"
		void selectEnd(bool dir = true);

    private:
    	// Selection related functions
        bool hasSelection() const;
        void select(std::size_t i);
        void selectNext(bool dir = true);

        std::vector<Button*> 	buttons;	// logically bound buttons
        int 					selected;	// index, int is useful in hasSelection()
        const KeyBindings* 		binds;		// used KeyBindings
};

#endif // MB_BUTTONGROUP_HH
