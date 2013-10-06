#pragma once
class Action;
class Enchant;
#include "Sound.h"
#include "Strings.h"
#include "Geometry.h"
#include "Constants.h"
#include "Graphics.h"
#include <string>

struct Dmg
{
    DAMAGE type;
    long flags;
    int value;
    Dmg(DAMAGE type, int value, long flags): type(type), flags(flags), value(value) {}
    Dmg(): type(DMG_FIRE), flags(DFLAG_NOFLAGS), value(0) {}
};
enum DefEntFlags
{
    ENT_IS_UNIT = 1,
    ENT_IS_PROJECTILE = 2,
    ENT_IS_SOLID = 4,
    ENT_IS_TALL = 8,
    ENT_IS_OPAQUE = 16,
    ENT_IS_WALL = 32,
    ENT_IS_OBELISK = 64,
    ENT_IS_DOOR = 128,
    ENT_IS_IMMORTAL = 256,
    ENT_IS_IMMOBILE = 512,
    ENT_IS_FLYING = 1024,
    ENT_IS_VISIBLE = 2048,
    ENT_IS_CHARMABLE = 4096,
    ENT_IS_UNTARGETABLE = 8192,
    ENT_IS_OMNIDIRECTIONAL = 16384,
    ENT_IS_HOMING_PROJECTILE = 32768
};
enum enumWeapontEnchants
{
    NO_CHANT,
    CHANT_FIRE_SPLASH,
    CHANT_SHOCK_SPARK,
    CHANT_POISON,
    CHANT_IMPACT,
    CHANT_SLOW,
    CHANT_STUN,
    CHANT_LEECH,
    CHANT_MANA_LEECH,
    CHANT_MANA_BURN,
    CHANT_ANCHORING
};

struct Weapon
{
    Action* action;
    BLOCK_QUALITY block_quality;
    int mana_per_ammo; //how much mana is needed per 1 ammo
    struct WeaponEnchant
    {
        enumWeapontEnchants type;
        int power;
        WeaponEnchant(enumWeapontEnchants type = NO_CHANT, int power = 0): type(type), power(power) {}
    } Enchants[4];

    Weapon(Action* action, BLOCK_QUALITY blocktype, int manacost, vector<WeaponEnchant> chants = vector<WeaponEnchant>());
};

class DefaultEntity
{
protected:
    int hp, mp, reghp, regmp;
    enumStrings name;
    Sound* snd_hurt;
    Sound* snd_die;
    enumMaterial Material;
    long flags;
    Dmg aura_damage, aura_radius_damage;
    std::vector<int> textures;
public:
    DefaultEntity(int hp, int mp, int reghp, int regmp, enumStrings name,
        Sound* snd_hurt, Sound* snd_die, enumMaterial Material, long flags,
        Dmg aura_damage = Dmg(), Dmg aura_radius_damage = Dmg());
    bool Flag(DefEntFlags flag) const { return ((flags & flag) > 0); }
    enumStrings Name() const { return name; }
    void SetTextures(std::vector<int> textures) { this->textures = textures; }
    friend class Entity;
    virtual ~DefaultEntity(){}
};
class Entity
{
protected:
    int hp, mp;
    long flags;
    vInt coor;
    Direction dir;
    DefaultEntity& prototype;
    int team;
    Entity *object_owner;
    vector<Entity *> subs;
    bool is_dead;

    virtual const DefaultEntity& GetPrototype() const {return prototype;}
    virtual void ApplyPenalties(const vInt& dest, long flags, int manacost, int cd_index, int cd_value);
public:
    virtual ~Entity(){}
    Entity(DefaultEntity& prototype, const vInt& coor, int team, Direction dir = NO_DIRECTION);
    virtual void Move(const vInt& dest, const Direction dir = NO_DIRECTION);
    virtual void GetDamage(Dmg damage);
    virtual void Turn(const Direction dir);
    virtual void StopCSpells(){}
    virtual void StartTurn();
    virtual void EndTurn();
    virtual bool CheckForValidity(const vInt& dest, const Entity* target, long flags, 
        bool IsVisible, int manacost, int range, int cd_index, bool is_lengthy) const;
    //virtual bool PerformAction(const vInt& dest, const Entity* target, long flags, bool IsVisible); 
    bool Flag(DefEntFlags flag) const { return prototype.Flag(flag); }
    vInt Coor() const { return coor; }
    Direction Dir() const { return dir; }
    int Team() const { return team; }
    Dmg AuraDamage() const { return GetPrototype().aura_damage; }
    Dmg AuraRadiusDamage() const { return GetPrototype().aura_radius_damage; }
    int MyTexture() const { 
        if (Flag(ENT_IS_OMNIDIRECTIONAL)) {
            return GetPrototype().textures[0];
        } else {
            return GetPrototype().textures[dir - 1];
        }
    }
    bool IsDead() const { return is_dead; }
    friend class Unit;
};

class DefaultUnit: public DefaultEntity
{
    int resist_fire, resist_shock, armor;
    int speed, charm_size;
    const vector<Action *> Actions;
    Weapon* weapon;
    int ammo;
public:
    DefaultUnit(DefaultEntity defent,
        int fireresist, int shockresist, int armor, int speed, int charmsize,
        vector<Action *>& actions_, Weapon* weapon, int ammo);
    friend class Unit;
    virtual ~DefaultUnit(){}
};
class Unit: public Entity
{
protected:
    int enchants[Counters::enchants];
    int cooldowns[Counters::cooldowns];
    int movepoints, ammo, ammo_manabuffer;
    bool is_in_blocking_state;
    int can_act;  //0 = cant, 1 = can do one quick, 2 = can do one full or 2 quick
    struct LAction
    {
        int time;
        vInt coor;
        const Action *action;
        void Nullify() { time = 0; coor = vInt(0,0); action = nullptr; }
        LAction(int time, vInt& coor, const Action *action): time(time), coor(coor), action(action) {}
        LAction(): time(0), coor(vInt(0, 0)), action(nullptr) {}
    } long_action; //currently under casting
    struct CSpell {
        Entity* target;
        //vInt target_coor;
        int metadata;
        ContinuousSpell* action;
        bool Is_Cast;
        void SetCasting(Entity* target, ContinuousSpell* action) {
            this->target = target;
            this->action = action;
            Is_Cast = true;
            metadata = 0;
        }
        void UnSet() {
            target = nullptr;
            action = nullptr;
            Is_Cast = false;
        }
    } cspell[Counters::continuous_spells];

    virtual void ApplyPenalties(const vInt& dest, long flags, int manacost, int cd_index, int cd_value);
    int FireResist(){return min(GetPrototype().resist_fire + (IsEnchanted(ENCHANT_PROTECTION_FIRE))? 50 : 0 , 100); }
    int ShockResist(){return min(GetPrototype().resist_shock + (IsEnchanted(ENCHANT_PROTECTION_SHOCK))? 50 : 0 , 100); }
    int VenomResist(){return min((IsEnchanted(ENCHANT_PROTECTION_FIRE))? 50 : 0 , 100); }
    int Armor(){return GetPrototype().armor; }
    virtual const DefaultUnit& GetPrototype() const {return dynamic_cast<const DefaultUnit&>(Entity::GetPrototype());}
public:
    virtual ~Unit(){}
    Unit(DefaultUnit& prototype, vInt& coor, int team, Direction dir = NO_DIRECTION);
    virtual bool CheckForValidity(const vInt& dest, const Entity* target, long flags, 
        bool IsVisible, int manacost, int range, int cd_index, bool is_lengthy) const;
    //virtual bool PerformAction(const vInt& dest, const Entity* target, long flags, bool IsVisible); 
    virtual void GetDamage(Dmg damage);
    virtual void CastEnchant(const Enchant& enchant);
    int Speed() {
        if (IsEnchanted(ENCHANT_STUN)) { return 0; }
        return max(GetPrototype().speed + 
            ((IsEnchanted(ENCHANT_HASTE))? 2 : 0 ) -
            ((IsEnchanted(ENCHANT_SLOW))? 2 : 0 ), 1);
    }
    int Speedtest(const enumEnchants id) {
        if (IsEnchanted(ENCHANT_STUN)) { return 0; }
        if (id == ENCHANT_HASTE) {
            return max(GetPrototype().speed + 2 -
                ((IsEnchanted(ENCHANT_SLOW))? 2 : 0 ), 1);
        } else {
            return max(GetPrototype().speed + 
                ((IsEnchanted(ENCHANT_HASTE))? 2 : 0 ) - 2, 1);
        }
    }
    virtual void StartTurn();
    virtual void EndTurn();
    virtual void Turn(const Direction dir);
    virtual void Move(const vInt& dest, const Direction dir = NO_DIRECTION);
    virtual void DrainMana(Entity *source);
    bool IsEnchanted(const enumEnchants id) const { return (enchants[id] > 0); }
    int& EnchantTime(const enumEnchants id) { return enchants[id]; }
    bool IsBusy() const { return long_action.time != 0; }
    LAction& PerformingAction() { return long_action; }
    CSpell& ContinuousAction(enumContinuousSpells id) { return cspell[id]; }
    bool ManaNotFull() { return mp != GetPrototype().mp; }
    int MovePoints() { return movepoints; }
    bool& BlockingState() { return is_in_blocking_state; }
    Action* Spellbar(int ID) { return GetPrototype().Actions[ID]; }
};

class DefaultProjectile: public DefaultEntity
{
protected:
    int speed;
    void (*Collide)(Entity*, Entity*); //projectile, obstacle
public:
    DefaultProjectile(DefaultEntity defent, int speed, void (*Collidefunc)(Entity*, Entity*));
    friend class Projectile;
    friend class CommonProjectile;
    virtual ~DefaultProjectile(){}
};
class Projectile: public Entity
{
protected:
    const DefaultProjectile& GetPrototype() const { return dynamic_cast<const DefaultProjectile&> (Entity::GetPrototype()); }
public:
    virtual int Speed() { return 0; }
    void Collision(Entity* obstacle) { GetPrototype().Collide(this, obstacle); } 
    Projectile(Entity entity);
};
class CommonProjectile: public Projectile
{
protected:
    vDbl ex_coor;
    Angle angle;
    int speed;
    const DefaultProjectile& GetPrototype() const { return Projectile::GetPrototype(); }
public:
    virtual int Speed() const { return speed; }
    CommonProjectile(DefaultProjectile& prototype, vInt& coor, int team, Entity* source, ::Angle angle); 
    Angle& Angle() { return angle; }
    vDbl& ExCoor() { return ex_coor; }
    virtual ~CommonProjectile(){}
};
class HomingProjectile: public Projectile
{
protected:
    Entity* target;

    const DefaultProjectile& GetPrototype() const {return dynamic_cast<const DefaultProjectile&>(Entity::GetPrototype());}
public:
    HomingProjectile(DefaultProjectile& prototype, vInt& coor, int team, Entity* source, Entity* target);
    virtual ~HomingProjectile(){}
};
class EnchantProjectile: public HomingProjectile
{
protected:
    enumEnchants enchant_id;
public:
    const DefaultProjectile& GetPrototype() const {return dynamic_cast<const DefaultProjectile&>(Entity::GetPrototype());}
    EnchantProjectile(HomingProjectile homing_base, enumEnchants enchant_id);
    virtual ~EnchantProjectile(){}
};

namespace Loader{
    using namespace ContainerDefs;
    void loadWeapons(WeaponContainer& container, ActionContainer& actions);
    void loadDefaultEntities(EntityContainer& container, ActionContainer& actions, 
        SoundContainer& sounds, WeaponContainer& weapons);
};
