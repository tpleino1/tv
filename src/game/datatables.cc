#include "datatables.hh"
#include "weapon.hh"
#include "Utilities.hh"



std::map<Item::ItemID, WeaponData> initializeWeaponData(State::Resources res)
{
	std::map<Item::ItemID, WeaponData> data;

	CSimpleIniA::TNamesDepend keys;
	res.config->GetAllKeys("Items", keys);
	std::string field;
	const char* field_ptr;

	for (auto i = keys.begin(); i != keys.end(); ++i)
	{
		// Ensure enabled weapon
		if (fromString<int>(res.config->GetValue("Items", i->pItem)) != 1)
			continue;

		field = "w_" + std::string(i->pItem);
		field_ptr = field.c_str();

		Item::ItemID id 		= 		(Item::ItemID)fromString<int>(	res.config->GetValue(field_ptr, "id"));
		data[id].name 			= 										res.config->GetValue(field_ptr, "name");
		data[id].cost 			= 					fromString<size_t>(	res.config->GetValue(field_ptr, "cost"));
		data[id].power 			= 					fromString<size_t>(	res.config->GetValue(field_ptr, "power"));
		data[id].blastRadius 	= 					fromString<int>(	res.config->GetValue(field_ptr, "radius"));
		data[id].fuse 			= 					fromString<float>(	res.config->GetValue(field_ptr, "fuse"));
		data[id].bpType 		= (BlastPatternType)fromString<int>(	res.config->GetValue(field_ptr, "blastType"));
		data[id].textureRect 	= 										res.textureRects->get(field);
	}

	return data;
}
