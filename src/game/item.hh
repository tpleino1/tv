#ifndef MB_ITEM_HH
#define MB_ITEM_HH

#include "object.hh"



class Item : public Object
{
	public:

		// Id to separate items from each other
		enum ItemID
		{
			None 		= -1,
			SmallBomb 	= 0,
			LargeBomb,
			Dynamite,
			Grenade,
			LargeGrenade,
			CrucifixBomb,
			LargeCrucifixBomb,
            Flamethrower,
            Mine,
            NuclearBomb
		};

		Item(float tileSize, size_t cost, ItemID id)
		: Object(tileSize), cost(cost), ID(id) {}

		virtual ~Item() {}

		// basic getters to acces the private members
		size_t getCost() const;
		ItemID getID() const;
		
	protected:
		size_t cost;
		ItemID ID;
};


#endif
