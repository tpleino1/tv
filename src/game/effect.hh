#ifndef MB_EFFECT_HH
#define MB_EFFECT_HH

#include "object.hh"
#include "State.hh"

class Effect : public Object
{

	public:

		// Constructor, 
		Effect(State::Resources res);
		
		// Updates the time elapsed by effect. Changes over to true if time has reached fuse.
		void update(float dt);

		// Makes the effect animate by moving the texturerect with certain interval.
		void moveTexture();

		// Returns true if effect is over (eTime>fuse). Default fuse is 0.5s.
		bool isOver() const;
		

	public:
		float eTime;
		float fuse;
		bool over;
		
		// Offset is used with moving the texturerect
		int offset;
};


#endif
