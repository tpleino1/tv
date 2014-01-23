
#ifndef MB_TILE_HH
#define MB_TILE_HH

#include "terrain.hh"
#include "terrainStorage.hh"
#include "item.hh"
#include "character.hh"
#include "weapon.hh"

#include "effect.hh"
#include "treasure.hh"
#include "monsterSpawner.hh"

class Level;

class Tile 
{

	private:

		//The terrain this tile is "made of"
		const Terrain* terrain;

		//The character in this tile
		Character* character;

		//The weapon in this tile
		std::unique_ptr<Weapon> weapon;

		//The effect in this tile
		std::unique_ptr<Effect> effect;

		Treasure* treasure;

		MonsterSpawner* spawner;

		//The amount of damage inflicted on this tile
		int damageValue;

		//Is this tile revealed (i.e. drawn on the screen)
		bool revealed;

		//Is the exit in this tile (campaign only)
		bool exit;

	public:

		friend class Level;

		//Tile(TerrainType t) : terrain(Terrain(t)) {
		Tile(const Terrain* t = nullptr, bool rev = false) : terrain(t), revealed(rev)
		{
			this->character = nullptr;
			this->treasure = nullptr;
			this->weapon = nullptr;
			this->effect = nullptr;
			this->spawner = nullptr;
			this->damageValue = 0;
			this->exit = false;
		}

		~Tile() { }
	
		//Tile(Terrain t, Item i, Effect e) : terrain(t), item(i), effect(e) { damageValue = 0; }

		TerrainType getTerrainType() const
		{
			if (this->terrain == nullptr) return TerrainType::Void;
			else return this->terrain->getTerrainType();
		}

		const Terrain* getTerrain() const
		{
			return this->terrain;
		}

		void setTerrain(const Terrain* t)
		{
			this->terrain = t;
		}
		
		bool hasItem() const
		{
			if (weapon)
			{
				return true;
			}
			return false;
		}

		// Return pointer to the weapon
		Weapon* getWeapon() const
		{
			return weapon.get();
		}

		// Place weapon into this tile 
		void setWeapon(std::unique_ptr<Weapon> ptr)
		{
			weapon = std::move(ptr);
		}

		// Releases unique_ptr in order to move the weapon to another tile
		Weapon* releaseWeapon()
		{
			return weapon.release();
		}

		Character* getCharacter() const
		{
			return this->character;
		}

		void setCharacter(Character* p)
		{
			this->character= p;
		}

		Treasure* getTreasure() const
		{
			return this->treasure;
		}

		void setTreasure(Treasure* t)
		{
			this->treasure = t;
		}

		MonsterSpawner* getMonsterSpawner() const
		{
			return this->spawner;
		}

		int getDamage() const
		{
			return this->damageValue;
		}

		void resetDamage()
		{
			this->damageValue = 0;
		}

		// Return pointer to the effect
		Effect* getEffect() const
		{
			return effect.get();

		}

		// Place effect into this tile
		void setEffect(std::unique_ptr<Effect> ptr) 
		{
			effect = std::move(ptr);
		}

		void damage(int val) 
		{
			if (val > 0)
			{
				this->damageValue += val;
			}
		}

		bool hasExit() const
		{
			return this->exit;
		}

		bool isVisible() const
		{
			return this->revealed;
		}

		void reveal()
		{
			this->revealed = true;
		}

		void obscure()
		{
			this->revealed = false;
		}
        
        // check if weapon can be moved to this tile    
        bool isValidForWeapon() const
        {
            if (weapon || terrain == nullptr || terrain->getTerrainType() != Empty || character)
			{
                return false;   
			}
            return true;
        }

};

#endif
