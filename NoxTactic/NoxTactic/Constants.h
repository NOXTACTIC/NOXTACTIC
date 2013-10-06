#pragma once
#include "Geometry.h"
const int visibility_range = 8;
const int actions_per_set = 5, actionsets_count = 5;
const vInt picsize = vInt(50, 50);
const int mana_drain_per_turn = 10;
const vInt field_window_size = vInt(10,10);
namespace Counters
{
    const int gestures = 9;
    const int actions = 53;
    const int enchants = 15;  //must be 18
    const int cooldowns = 3;
    const int entities = 32;
    const int weapons = 11;
    const int sounds = 89;
    const int strings = 99;
    const int continuous_spells = 8;
    const int directions = 8;
};
const string common_extension = ".jpg";
namespace Paths 
{
    const string MoveArrow = "\\data\\pictures\\common\\Arrow";
    const string SelectionFrame = "\\data\\pictures\\common\\Selection" + common_extension;
    const string EntitiesPictures = "\\data\\pictures\\entities\\";
    const string ActionsIcons = "\\data\\pictures\\actions icons\\";
    const string ActionsActiveIcons = "\\data\\pictures\\actions icons\\active_";
};
class Action;
class Spell;
class Ability;
class ContinuousSpell;
class DefaultEntity;
class Entity;
class Gesture;
class Enchant;
class Map;
class Sound;
struct Weapon;
class Tile;
class Wall;

enum enumEntities
{
    NO_ENTITY,
    ENT_WIZ_YELLOW,
    ENT_WIZ_BLUE,
    ENT_WIZ_RED,
    ENT_CONJ_GREEN,
    ENT_CONJ_ORANGE,
    ENT_WAR_RED,
    ENT_WAR_BLUE,
    ENT_FIREBALL,
    ENT_FIREBALL_SMALL,
    ENT_ARROW,
    ENT_SHURIKEN,
    ENT_FON,
    ENT_FON_SMALL,
    ENT_ENCHANTBALL,
    ENT_FLAME,
    ENT_BLUEFLAME,
    ENT_DISPELLER_CENTER,
    ENT_DISPELLER_LIGHT,
    ENT_MAGIC_MISSILE,
    ENT_PIXIE_SWARM,
    ENT_FIST_SHADOW,
    ENT_METEOR_SHADOW,
    ENT_FIRERING_FLAME,
    ENT_MAGICWALL,
    ENT_OBELISK,
    ENT_OBELISK_PRIMITIVE,
    ENT_OBELISK_LOTD,
    ENT_TELEPORT_PENTAGRAM,
    ENT_TOXIC_CLOUD,
    ENT_DOOR_WOODEN,
    ENT_DOOR_JAIL
};
enum enumWeapons
{
    WEAP_FIRESTAFF,
    WEAP_FORCESTAFF,
    WEAP_FONSTAFF,
    WEAP_LONGSWORD,
    WEAP_HAMMER,
    WEAP_SHURIKEN,
    WEAP_CHAKRAM,
    WEAP_MACE,
    WEAP_FIRESWORD,
    WEAP_TRIPLEFIRE,
    WEAP_BOW
};

enum BLOCK_QUALITY
{
    NO_BLOCK,
    BLOCK_STAFF,
    BLOCK_LONGSWORD,
    BLOCK_SHIELD,
    BLOCK_QUICKSHIELD
};
enum enumMaterial
{
    NO_MATERIAL,
    MATERIAL_FLESH,
    MATERIAL_METAL,
    MATERIAL_WOOD,
    MATERIAL_STONE
};
enum DAMAGE
{
    DMG_FIRE,
    DMG_SHOCK,
    DMG_VENOM,
    DMG_PHYS,
    DMG_BLUDGEON,
    DMG_MAGIC
};
enum DAMAGE_FLAGS
{
    DFLAG_NOFLAGS,
    DFLAG_CONTINUAL,
    DFLAG_IGNORES_FF,
    DFLAG_IGNORES_ALL,
    DFLAG_IGNORE_SOUND
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

namespace ContainerDefs
{
    typedef std::vector<Action *> ActionContainer;
    typedef std::vector<DefaultEntity *> EntityContainer;
    typedef std::vector<void (*)(Entity*, const vInt&, const Action*)> FunctionContainer;
    typedef std::vector<void (*)(Entity*, Entity*, const Action*)> ContinuousFunctionContainer;
    typedef std::vector<Gesture*> GestureContainer;
    typedef std::vector<Enchant*> EnchantContainer;
    typedef std::vector<Map*> MapContainer;
    typedef std::vector<std::string> StringContainer;
    typedef std::vector<Sound*> SoundContainer;
    typedef std::vector<Weapon*> WeaponContainer;
    typedef std::vector<Wall*> WallContainer;
    typedef std::vector<Tile*> TileContainer;
};

enum Team
{
    NO_TEAM,
    TEAM_BLUE,
    TEAM_RED,
    TEAM_YELLOW,
    TEAM_GREEN,
    TEAM_WHITE,
    TEAM_BLACK
};

enum MouseFlags
{
    CLICK_SHIFT = 1,
    CLICK_CTRL = 2,
    CLICK_ALT = 4,
    CLICK_LEFT = 8,
    CLICK_RIGHT = 16
};
enum Keys
{
    KEY_NONE,
    KEY_ESC,
    KEY_SPELL1,
    KEY_SPELL2,
    KEY_SPELL3,
    KEY_SPELL4,
    KEY_SPELL5,
    KEY_ARROW_LEFT,
    KEY_ARROW_RIGHT,
    KEY_ARROW_UP,
    KEY_ARROW_DOWN,
    KEY_ACCEPT
};
enum Buttons
{
    NO_BUTTON,
    BUTTON_ENDTURN
};
enum CursorMode
{
    CURSORMODE_NO,
    CURSORMODE_SELECT,
    CURSORMODE_TARGETING,
    CURSORMODE_LTARGETING
};
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