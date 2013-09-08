#pragma once
enum enumStrings;
#include <vector>
#include "Sound.h"
#include "Strings.h"
#include "Vector.h"
class Entity;
class Unit;
class Projectile;

enum ActionFlags
{
    AFLAG_CANT_TARGET_SELF = 1,
    AFLAG_MUST_TARGET_UNIT = 2,
    AFLAG_MUST_TARGET_VISIBLE = 4,
    AFLAG_CANT_BE_ANCHORED = 8,
    AFLAG_MUST_TARGET_EMPTY = 16,
    AFLAG_NEED_AMMO = 32,
    AFLAG_MUST_TURN_TO_TARGET = 64,
    AFLAG_IS_MELEE = 128,
    AFLAG_IS_AGGRESSIVE = 256,
    AFLAG_IS_TARGETLESS = 512,
    AFLAG_IS_SPELL = 1024,
    AFLAG_IS_ABILITY = 2048,
    AFLAG_IS_QUICK = 4096,
    AFLAG_RANGE_LIMITED = 8192,
    AFLAG_IS_CONTINUOUS = 16384,
    AFLAG_IS_ENCHANT = 32768
};

enum enumActions
{
    NO_ACTION,
    ACTION_FIREBALL,
    ACTION_ANCHOR,
    ACTION_BURN,
    ACTION_CHANNEL,
    ACTION_COUNTERSPELL,
    ACTION_DEATHRAY,
    ACTION_DISPELL_UNDEAD,
    ACTION_ENERGY_BOLT,
    ACTION_EARTHQUAKE,
    ACTION_LIGHTNING,
    ACTION_PROTECTION_FIRE,
    ACTION_PROTECTION_SHOCK,
    ACTION_PROTECTION_VENOM,
    ACTION_FORCEFIELD,
    ACTION_HEAL_LESSER,
    ACTION_HEAL_GREATER,
    ACTION_HASTE,
    ACTION_INVERSION,
    ACTION_MAGIC_MISSILE,
    ACTION_MANA_DRAIN,
    ACTION_PULL,
    ACTION_PUSH,
    ACTION_REFLECTIVE_SHIELD,
    ACTION_FIRE_RING,
    ACTION_SHOCK,
    ACTION_SLOW,
    ACTION_SWAP,
    ACTION_TELEPORT,
    ACTION_WALL,
    ACTION_FIST_OF_VENGEANCE,
    ACTION_VAMPIRISM,
    ACTION_STUN,
    ACTION_TOXIC_CLOUD,
    ACTION_FORCE_OF_NATURE,
    ACTION_PIXIES,
    ACTION_CHARM,
    ACTION_METEOR,
    ACTION_POISON,
    ACTION_OBLITERATION,
    ACTION_BERSERKER,
    ACTION_HARPOON,
    ACTION_WARCRY,
    ACTION_HAMMER,
    ACTION_LONGSWORD,
    ACTION_MACE,
    ACTION_BOW,
    ACTION_FORCE_STAFF,
    ACTION_FIRE_STAFF,
    ACTION_HELLFIRE_STAFF,
    ACTION_SHURIKEN,
    ACTION_FIRESWORD,
    ACTION_FON_STAFF
};
enum enumGestures
{
    G_,
    G_UN,
    G_KA,
    G_ET,
    G_RO,
    G_ZO,
    G_DO,
    G_CHA,
    G_IN
};
enum enumEnchants
{
    NO_ENCHANT,
    ENCHANT_SLOW,
    ENCHANT_HASTE,
    ENCHANT_FORCE_FIELD,
    ENCHANT_REFLECTIVE_SHIELD,
    ENCHANT_PROTECTION_FIRE,
    ENCHANT_PROTECTION_SHOCK,
    ENCHANT_PROTECTION_POISON,
    ENCHANT_STUN,
    ENCHANT_PIXIES,
    ENCHANT_ANCHOR,
    ENCHANT_VAMPIRISM,
    ENCHANT_POISON,
    ENCHANT_SHOCK,
    ENCHANT_NULL
};
enum enumCooldowns
{
    CD_WARCRY = 0,
    CD_BERSERKER,
    CD_HARPOON
};
enum enumContinuousSpells 
{
    CONT_LIGHTNING,
    CONT_ENERGY_BOLT,
    CONT_GREATER_HEAL,
    CONT_CHARM,
    CONT_DRAIN_MANA,
    CONT_OBLITERATION,
    CONT_CHANNEL_LIFE,
    CONT_LIGHTNING_STAFF
};

class Gesture
{
	const Sound& sound;
	const int powernumber;
public:
	Gesture(const Sound& sound, const int number);
	void CastMe(const int volume) const;
};
class Enchant
{
public:
    const enumStrings name;
	const Sound& sound_on;
	const Sound& sound_off;
    const int time_to_last, id;
    const bool does_wearoff_auto;
    Enchant(const enumStrings name, const Sound& snd_on, const Sound& snd_off, const int time, const int id, const bool wear_off_auto = true);
    int MyTexture() const { return 0; }
};

class Action
{
protected:
	int range, damage1, damage2, time_value, time_to_perform;
	Sound& sound;
    enumStrings name, msg_performed, msg_started, msg_stopped, msg_affected, msg_killed;
    int flags;
    int icon_texture_id;
    int active_icon_texture_id;
    void (*action)(Entity*, const vInt&, const Action*); //func to be called 
public:
    Action(void (*action)(Entity*, const vInt&, const Action*), 
            int range, int damage1, int damage2, int timevalue, int performtime,
            Sound& sound, enumStrings name, enumStrings msg_done, enumStrings msg_started, 
            enumStrings msg_stopped, enumStrings msg_affected, enumStrings msg_killed, int flags,
            int icon_texture_id, int active_icon_texture_id);
	virtual bool Perform(Entity* actor, const vInt& target) const;
    int Range() const {return range;}
    bool Flag(const ActionFlags flag) const { return ((flags & flag) == 1); }
    bool IsLengthy() const { return time_to_perform != 0; }
    int TimeToPerform() const { return time_to_perform; }
    int MyIcon() const { return icon_texture_id; }
    int MyActiveIcon() const { return active_icon_texture_id; }
};
class Spell: public Action
{
	enumGestures gesture[8];
	const int manacost;
public:
    Spell(Action action, int manacost, enumGestures g1, enumGestures g2, enumGestures g3, 
        enumGestures g4, enumGestures g5, enumGestures g6, enumGestures g7, enumGestures g8);
    virtual bool Perform(Entity* actor, const vInt& target) const;
    int Cost() const {return manacost;}
};
class ContinuousSpell: public Spell
{
    enumContinuousSpells id;
    void (*continuous_action)(Entity*, Entity*, const Action*); //func to be called 
public:
    ContinuousSpell(Spell spell, enumContinuousSpells id, void (*continuous_action)(Entity*, Entity*, const Action*));
    enumContinuousSpells ID() const { return id;}
    virtual bool Perform(Entity* actor, const vInt& target) const;
    virtual bool ApplyEffect(Entity* actor, Entity* target) const;
};
class EnchantSpell: public Spell
{
    enumEnchants id;
public:
    EnchantSpell(Spell spell, enumEnchants id);
    virtual bool Perform(Entity* actor, const vInt& target) const;
};
class Ability: public Action
{
    const int cooldown;
    enumCooldowns cd_index;
public:
    Ability(Action action, int cooldown, enumCooldowns cd_index);
    virtual bool Perform(Entity* actor, const vInt& target) const;
    int cooldown_index() const {return cd_index;}
    int cooldown_value() const {return cooldown;}
};

namespace Loader{
    typedef std::vector<Action *> ActionContainer;
    typedef std::vector<void (*)(Entity*, const vInt&, const Action*)> FunctionContainer;
    typedef std::vector<void (*)(Entity*, Entity*, const Action*)> ContinuousFunctionContainer;
    void loadActions(ActionContainer& container, SoundContainer& sounds, 
        FunctionContainer& functions, ContinuousFunctionContainer& continuous_functions,
        std::string AppPath, StringContainer strings);
    
    typedef std::vector<Gesture> GestureContainer;
    void loadGestures(GestureContainer& container, SoundContainer& sounds);
    
    typedef std::vector<Enchant> EnchantContainer;
    void loadEnchants(EnchantContainer& container, SoundContainer& sounds);
};

