#ifndef MB_BUTTON_HH
#define MB_BUTTON_HH

#include <functional>	// function
#include <string>

#include <SFML/Graphics/Text.hpp>	// Base class

#include "ResourceId.hh"	// ColorContainer

namespace sf
{
	class Event;
}

// A class to represent "clickable" text (i.e., a link) in menu-like states
// (that are bound together with a ButtonGroup)
class Button : public sf::Text
{
	public:
		// enum type to tell the state of a button
		enum Status
		{
			Enabled,	// Selectable and visible
			Disabled,	// Unselectable, but visible
			Hidden		// Unselectable, invisible
		};

        typedef std::function<void()> Callback;	// Just a shorthand, basically

		// Parameterless constructor for STL-containers
		Button();
		// Actual constructor that should always be used
		Button(const std::string& string, const sf::Font& font, const ColorContainer& c);
		//Button(const Button& b) = delete;
		//Button& operator= (const Button& b) = delete;
		//virtual ~Button();

		// Selection related functions
		bool 	isSelected() const;
        void 	select();
        void 	deselect();

		// Activation related functions
        bool 	isActive() const;
        void 	activate(bool p = true);
        void 	deactivate();

		// Toggling related functions
		bool 	isToggle() const;
		void	setToggle(bool t);

		// Status related functions
		Status 	getStatus() const;
		void	setStatus(Status s);

		void 	setCallback(Callback cb, bool p = true);
		void 	setColors(const ColorContainer& c);
		void 	handleEvent(const sf::Event& e); // For future expansion

    private:
    	// Button changes its color in response to its state
		void 	changeColor();

        bool 					selected;
        bool 					active;
		bool 					toggled;
		Status					stat;
		Callback 				primaryCallback;
		Callback 				secondaryCallback;
		const ColorContainer* 	colors;		// We use 5 different colors here
};

#endif // MB_BUTTON_HH
