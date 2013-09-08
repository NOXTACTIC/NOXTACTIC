class Action;
class Enchant;
#include "Entity.h"
#include "Action.h"
#include "Constants.h"
#include "Sound.h"
#include "Vector.h"
#include <math.h>
#include <string>
#pragma warning (disable:4351)
DefaultEntity::DefaultEntity(int hp, int mp, int reghp, int regmp, enumStrings name,
    Sound& snd_hurt, Sound& snd_die, enumMaterial Material, int flags, 
    const std::string& AppPath, const Loader::StringContainer& strings, Dmg aura_damage, Dmg aura_radius_damage): 
    hp(hp),  mp(mp), reghp(reghp), regmp(regmp), name(name), 
    snd_hurt(snd_hurt), snd_die(snd_die), Material(Material), flags(flags),
    aura_damage(aura_damage), aura_radius_damage(aura_radius_damage), textures(){
        if (name != NO_STRING) {
            if (Flag(ENT_IS_OMNIDIRECTIONAL)) {
                textures.push_back(GIH->AddTexture(AppPath + Paths::EntitiesPictures + strings[name] + common_extension));
            } else {
                textures.push_back(GIH->AddTexture(AppPath + Paths::EntitiesPictures + strings[name] + int_to_string(1) + common_extension));
                textures.push_back(GIH->AddTexture(AppPath + Paths::EntitiesPictures + strings[name] + int_to_string(2) + common_extension));
                textures.push_back(GIH->AddTexture(AppPath + Paths::EntitiesPictures + strings[name] + int_to_string(3) + common_extension));
                textures.push_back(GIH->AddTexture(AppPath + Paths::EntitiesPictures + strings[name] + int_to_string(4) + common_extension));
                textures.push_back(GIH->AddTexture(AppPath + Paths::EntitiesPictures + strings[name] + int_to_string(5) + common_extension));
                textures.push_back(GIH->AddTexture(AppPath + Paths::EntitiesPictures + strings[name] + int_to_string(6) + common_extension));
                textures.push_back(GIH->AddTexture(AppPath + Paths::EntitiesPictures + strings[name] + int_to_string(7) + common_extension));
                textures.push_back(GIH->AddTexture(AppPath + Paths::EntitiesPictures + strings[name] + int_to_string(8) + common_extension));
            }
        }
    }
DefaultUnit::DefaultUnit(DefaultEntity defent,
        int fireresist, int shockresist, int armor, int speed, int charmsize,
        vector<Action *>& actions_, Weapon& weapon, int ammo):
        DefaultEntity(defent),
        resist_fire(fireresist), resist_shock(shockresist), armor(armor), speed(speed),
        charm_size(charmsize), Actions(actions_), weapon(weapon), ammo(ammo) {}
DefaultProjectile::DefaultProjectile(DefaultEntity defent):
        DefaultEntity(defent) {}

Weapon::Weapon(Action* action, BLOCK_QUALITY blocktype, int manacost, vector<WeaponEnchant> chants): 
        action(action), block_quality(blocktype), mana_per_ammo(manacost) {
            for (unsigned int i = 0; i < min(4, chants.size()); ++i) {
                Enchants[i] = chants[i];
            }
        }

Entity::Entity(DefaultEntity& prototype, const vInt& coor, int team, Direction dir):
    hp(prototype.hp), mp(prototype.mp), flags(0), prototype(prototype), coor(coor),
    dir(dir), team(team), object_owner(0), subs(), is_dead(false) {}
Unit::Unit(DefaultUnit& prototype, vInt& coor, int team, Direction dir):
    Entity(static_cast<DefaultEntity&>(prototype), coor, team, dir),
    enchants(), cooldowns(), movepoints(prototype.speed), ammo(prototype.ammo),
    ammo_manabuffer(0), is_in_blocking_state(true), can_act(2), lspell(), cspell(){}


void Loader::loadWeapons(WeaponContainer& container, ActionContainer& actions){
    container.reserve(Counters::weapons);
    container.push_back(Weapon(actions[ACTION_FIRE_STAFF], BLOCK_STAFF, 3));
    container.push_back(Weapon(actions[ACTION_FORCE_STAFF], BLOCK_STAFF, 1));
    container.push_back(Weapon(actions[ACTION_FON_STAFF], BLOCK_STAFF, 20));
    container.push_back(Weapon(actions[ACTION_LONGSWORD], BLOCK_LONGSWORD, 0));
    container.push_back(Weapon(actions[ACTION_HAMMER], NO_BLOCK, 0));
    container.push_back(Weapon(actions[ACTION_SHURIKEN], BLOCK_SHIELD, 0));
    container.push_back(Weapon(actions[NO_ACTION], NO_BLOCK, 0)); //chakram
    container.push_back(Weapon(actions[ACTION_MACE], BLOCK_SHIELD, 0));
    container.push_back(Weapon(actions[ACTION_FIRESWORD], BLOCK_SHIELD, 0));
    container.push_back(Weapon(actions[ACTION_HELLFIRE_STAFF], BLOCK_STAFF, 6));
    container.push_back(Weapon(actions[ACTION_BOW], NO_BLOCK, 0));
}

void Loader::loadDefaultEntities(EntityContainer& container, ActionContainer& actions, 
                                 SoundContainer& sounds, WeaponContainer& weapons, 
                                 std::string AppPath, const StringContainer& strings){
    ActionContainer tmp;
    tmp.assign(actions_per_set * actionsets_count, actions[NO_ACTION]);

    container.reserve(Counters::entities);
    container.push_back(new DefaultEntity(0, 0, 0, 0, NO_STRING, 
        sounds[NO_SOUND], sounds[NO_SOUND], NO_MATERIAL, 0, AppPath, strings));
    //WIZARD YELLOW
    tmp[0] = actions[ACTION_HASTE];
    tmp[1] = actions[ACTION_PROTECTION_FIRE];
    tmp[2] = actions[ACTION_PROTECTION_SHOCK]; 
    tmp[3] = actions[ACTION_FORCEFIELD]; 
    tmp[4] = actions[ACTION_HEAL_LESSER];

    tmp[5] = actions[ACTION_ANCHOR];
    tmp[6] = actions[ACTION_SWAP];
    tmp[7] = actions[ACTION_REFLECTIVE_SHIELD]; 
    tmp[8] = actions[ACTION_PROTECTION_VENOM]; 
    tmp[9] = actions[ACTION_BURN];

    tmp[10] = actions[ACTION_INVERSION];
    tmp[11] = actions[ACTION_CHANNEL];
    tmp[12] = actions[ACTION_OBLITERATION]; 
    tmp[13] = actions[ACTION_MANA_DRAIN]; 
    tmp[14] = actions[ACTION_COUNTERSPELL];

    tmp[15] = actions[ACTION_EARTHQUAKE];
    tmp[16] = actions[NO_ACTION];
    tmp[17] = actions[NO_ACTION]; 
    tmp[18] = actions[NO_ACTION]; 
    tmp[19] = actions[NO_ACTION];

    tmp[20] = actions[ACTION_PULL];
    tmp[21] = actions[ACTION_TELEPORT];
    tmp[22] = actions[ACTION_DISPELL_UNDEAD]; 
    tmp[23] = actions[ACTION_WALL]; 
    tmp[24] = actions[ACTION_PUSH];

    container.push_back(new DefaultUnit(DefaultEntity(75, 150, 1, 2, STR_ENT_WIZ_YELLOW,
        sounds[NO_SOUND], sounds[NO_SOUND], MATERIAL_FLESH, 
        ENT_IS_UNIT + ENT_IS_SOLID + ENT_IS_TALL + ENT_IS_VISIBLE, AppPath, strings), 
        0, 0, 10, 2, 0, tmp, weapons[WEAP_FIRESTAFF], 20));
    //WIZARD BLUE
//     tmp[0] = actions[ACTION_]; the same as before
     tmp[1] = actions[ACTION_ENERGY_BOLT];
//     tmp[2] = actions[ACTION_]; 
//     tmp[3] = actions[ACTION_]; 
//     tmp[4] = actions[ACTION_];

    tmp[5] = actions[ACTION_INVERSION];
    tmp[6] = actions[ACTION_LIGHTNING];
    tmp[7] = actions[ACTION_REFLECTIVE_SHIELD]; 
    tmp[8] = actions[ACTION_COUNTERSPELL]; 
    tmp[9] = actions[ACTION_BURN];

    tmp[10] = actions[ACTION_CHANNEL];
    tmp[11] = actions[ACTION_EARTHQUAKE];
    tmp[12] = actions[NO_ACTION]; 
    tmp[13] = actions[NO_ACTION]; 
    tmp[14] = actions[NO_ACTION];

    tmp[15] = actions[NO_ACTION];
    tmp[16] = actions[NO_ACTION];
    tmp[17] = actions[NO_ACTION]; 
    tmp[18] = actions[NO_ACTION]; 
    tmp[19] = actions[NO_ACTION];

//     tmp[20] = actions[ACTION_]; the same as before
    tmp[21] = actions[ACTION_SHOCK];
//     tmp[22] = actions[ACTION_]; 
//     tmp[23] = actions[ACTION_]; 
//     tmp[24] = actions[ACTION_];
    container.push_back(new DefaultUnit(DefaultEntity(75, 150, 1, 1, STR_ENT_WIZ_BLUE,
        sounds[NO_SOUND], sounds[NO_SOUND], MATERIAL_FLESH, 
        ENT_IS_UNIT + ENT_IS_SOLID + ENT_IS_TALL + ENT_IS_VISIBLE, AppPath, strings), 
        0, 20, 10, 2, 0, tmp, weapons[WEAP_FORCESTAFF], 60));

    //WIZARD RED
//     tmp[0] = actions[ACTION_]; the same as before
     tmp[1] = actions[ACTION_FIREBALL];
     tmp[2] = actions[ACTION_PROTECTION_FIRE]; 
//     tmp[3] = actions[ACTION_]; 
//     tmp[4] = actions[ACTION_];

    tmp[5] = actions[ACTION_ANCHOR];
    tmp[6] = actions[ACTION_MAGIC_MISSILE];
    tmp[7] = actions[ACTION_SLOW]; 
    tmp[8] = actions[ACTION_SWAP]; 
    tmp[9] = actions[ACTION_BURN];

    tmp[10] = actions[ACTION_TELEPORT];
    tmp[11] = actions[NO_ACTION];
    tmp[12] = actions[NO_ACTION]; 
    tmp[13] = actions[NO_ACTION]; 
    tmp[14] = actions[NO_ACTION];

    tmp[15] = actions[NO_ACTION];
    tmp[16] = actions[NO_ACTION];
    tmp[17] = actions[NO_ACTION]; 
    tmp[18] = actions[NO_ACTION]; 
    tmp[19] = actions[NO_ACTION];

    tmp[20] = actions[ACTION_INVERSION];
    tmp[21] = actions[ACTION_DEATHRAY];
    tmp[22] = actions[ACTION_FIRE_RING]; 
    tmp[23] = actions[ACTION_MANA_DRAIN]; 
    tmp[24] = actions[ACTION_COUNTERSPELL];
    container.push_back(new DefaultUnit(DefaultEntity(75, 150, 1, 1, STR_ENT_WIZ_RED,
        sounds[NO_SOUND], sounds[NO_SOUND], MATERIAL_FLESH, 
        ENT_IS_UNIT + ENT_IS_SOLID + ENT_IS_TALL + ENT_IS_VISIBLE, AppPath, strings), 
        20, 0, 10, 2, 0, tmp, weapons[WEAP_TRIPLEFIRE], 10));


//     tmp[0] = actions[ACTION_];
//     tmp[1] = actions[ACTION_];
//     tmp[2] = actions[ACTION_]; 
//     tmp[3] = actions[ACTION_]; 
//     tmp[4] = actions[ACTION_];
// 
//     tmp[5] = actions[ACTION_];
//     tmp[6] = actions[ACTION_];
//     tmp[7] = actions[ACTION_]; 
//     tmp[8] = actions[ACTION_]; 
//     tmp[9] = actions[ACTION_];
// 
//     tmp[10] = actions[ACTION_];
//     tmp[11] = actions[ACTION_];
//     tmp[12] = actions[ACTION_]; 
//     tmp[13] = actions[ACTION_]; 
//     tmp[14] = actions[ACTION_];
// 
//     tmp[15] = actions[ACTION_];
//     tmp[16] = actions[ACTION_];
//     tmp[17] = actions[ACTION_]; 
//     tmp[18] = actions[ACTION_]; 
//     tmp[19] = actions[ACTION_];
// 
//     tmp[20] = actions[ACTION_];
//     tmp[21] = actions[ACTION_];
//     tmp[22] = actions[ACTION_]; 
//     tmp[23] = actions[ACTION_]; 
//     tmp[24] = actions[ACTION_];
//     container.push_back(new DefaultUnit(75, 150, 1, 2, NO_STRING,
//         sounds[NO_SOUND], sounds[NO_SOUND], MATERIAL_FLESH, 
//         ENT_IS_UNIT + ENT_IS_SOLID + ENT_IS_TALL + ENT_IS_VISIBLE, 0, 0, 10, 
//         2, 0, tmp, weapons[WEAP_FIRESTAFF], 20));
// 
// 
//     tmp[0] = actions[ACTION_];
//     tmp[1] = actions[ACTION_];
//     tmp[2] = actions[ACTION_]; 
//     tmp[3] = actions[ACTION_]; 
//     tmp[4] = actions[ACTION_];
// 
//     tmp[5] = actions[ACTION_];
//     tmp[6] = actions[ACTION_];
//     tmp[7] = actions[ACTION_]; 
//     tmp[8] = actions[ACTION_]; 
//     tmp[9] = actions[ACTION_];
// 
//     tmp[10] = actions[ACTION_];
//     tmp[11] = actions[ACTION_];
//     tmp[12] = actions[ACTION_]; 
//     tmp[13] = actions[ACTION_]; 
//     tmp[14] = actions[ACTION_];
// 
//     tmp[15] = actions[ACTION_];
//     tmp[16] = actions[ACTION_];
//     tmp[17] = actions[ACTION_]; 
//     tmp[18] = actions[ACTION_]; 
//     tmp[19] = actions[ACTION_];
// 
//     tmp[20] = actions[ACTION_];
//     tmp[21] = actions[ACTION_];
//     tmp[22] = actions[ACTION_]; 
//     tmp[23] = actions[ACTION_]; 
//     tmp[24] = actions[ACTION_];
//     container.push_back(new DefaultUnit(75, 150, 1, 2, NO_STRING,
//         sounds[NO_SOUND], sounds[NO_SOUND], MATERIAL_FLESH, 
//         ENT_IS_UNIT + ENT_IS_SOLID + ENT_IS_TALL + ENT_IS_VISIBLE, 0, 0, 10, 
//         2, 0, tmp, weapons[WEAP_FIRESTAFF], 20));
// 
// 
//     tmp[0] = actions[ACTION_];
//     tmp[1] = actions[ACTION_];
//     tmp[2] = actions[ACTION_]; 
//     tmp[3] = actions[ACTION_]; 
//     tmp[4] = actions[ACTION_];
// 
//     tmp[5] = actions[ACTION_];
//     tmp[6] = actions[ACTION_];
//     tmp[7] = actions[ACTION_]; 
//     tmp[8] = actions[ACTION_]; 
//     tmp[9] = actions[ACTION_];
// 
//     tmp[10] = actions[ACTION_];
//     tmp[11] = actions[ACTION_];
//     tmp[12] = actions[ACTION_]; 
//     tmp[13] = actions[ACTION_]; 
//     tmp[14] = actions[ACTION_];
// 
//     tmp[15] = actions[ACTION_];
//     tmp[16] = actions[ACTION_];
//     tmp[17] = actions[ACTION_]; 
//     tmp[18] = actions[ACTION_]; 
//     tmp[19] = actions[ACTION_];
// 
//     tmp[20] = actions[ACTION_];
//     tmp[21] = actions[ACTION_];
//     tmp[22] = actions[ACTION_]; 
//     tmp[23] = actions[ACTION_]; 
//     tmp[24] = actions[ACTION_];
//     container.push_back(new DefaultUnit(75, 150, 1, 2, NO_STRING,
//         sounds[NO_SOUND], sounds[NO_SOUND], MATERIAL_FLESH, 
//         ENT_IS_UNIT + ENT_IS_SOLID + ENT_IS_TALL + ENT_IS_VISIBLE, 0, 0, 10, 
//         2, 0, tmp, weapons[WEAP_FIRESTAFF], 20));
// 
// 
//     tmp[0] = actions[ACTION_];
//     tmp[1] = actions[ACTION_];
//     tmp[2] = actions[ACTION_]; 
//     tmp[3] = actions[ACTION_]; 
//     tmp[4] = actions[ACTION_];
// 
//     tmp[5] = actions[ACTION_];
//     tmp[6] = actions[ACTION_];
//     tmp[7] = actions[ACTION_]; 
//     tmp[8] = actions[ACTION_]; 
//     tmp[9] = actions[ACTION_];
// 
//     tmp[10] = actions[ACTION_];
//     tmp[11] = actions[ACTION_];
//     tmp[12] = actions[ACTION_]; 
//     tmp[13] = actions[ACTION_]; 
//     tmp[14] = actions[ACTION_];
// 
//     tmp[15] = actions[ACTION_];
//     tmp[16] = actions[ACTION_];
//     tmp[17] = actions[ACTION_]; 
//     tmp[18] = actions[ACTION_]; 
//     tmp[19] = actions[ACTION_];
// 
//     tmp[20] = actions[ACTION_];
//     tmp[21] = actions[ACTION_];
//     tmp[22] = actions[ACTION_]; 
//     tmp[23] = actions[ACTION_]; 
//     tmp[24] = actions[ACTION_];
//     container.push_back(new DefaultUnit(75, 150, 1, 2, NO_STRING,
//         sounds[NO_SOUND], sounds[NO_SOUND], MATERIAL_FLESH, 
//         ENT_IS_UNIT + ENT_IS_SOLID + ENT_IS_TALL + ENT_IS_VISIBLE, 0, 0, 10, 
//         2, 0, tmp, weapons[WEAP_FIRESTAFF], 20));
//         STR_ENT_WIZ_BLUE,
//         STR_ENT_WIZ_RED,
//         STR_ENT_CONJ_GREEN,
//         STR_ENT_CONJ_ORANGE,
//         STR_ENT_WAR_RED,
//         STR_ENT_WAR_BLUE,
//         STR_ENT_FIREBALL,
//         STR_ENT_FIREBALL_SMALL,
//         STR_ENT_ARROW,
//         STR_ENT_SHURIKEN,
//         STR_ENT_FON,
//         STR_ENT_FON_SMALL,
//         STR_ENT_ENCHANTBALL,
//         STR_ENT_FLAME,
//         STR_ENT_BLUEFLAME,
//         STR_ENT_DISPELLER_CENTER,
//         STR_ENT_DISPELLER_LIGHT,
//         STR_ENT_MAGIC_MISSILE,
//         STR_ENT_PIXIE_SWARM,
//         STR_ENT_FIST_SHADOW,
//         STR_ENT_METEOR_SHADOW,
//         STR_ENT_FIRERING_FLAME,
//         STR_ENT_MAGICWALL,
//         STR_ENT_OBELISK,
//         STR_ENT_OBELISK_PRIMITIVE,
//         STR_ENT_OBELISK_LOTD,
//         STR_ENT_TELEPORT_PENTAGRAM,
//         STR_ENT_TOXIC_CLOUD,
//         STR_ENT_DOOR_WOODEN,
//         STR_ENT_DOOR_JAIL,
}
bool Entity::CheckForValidity(const vInt& dest, const Entity* target, const Action* action, bool IsVisible) const { return false; }
bool Entity::PerformAction(const vInt& dest, const Entity* target, const int action_id, bool IsVisible) {return false;}
bool Unit::CheckForValidity(const vInt& dest, const Entity* target, const Action* action, bool IsVisible) const {
    const DefaultUnit& me = GetPrototype();
    bool is_lengthy = action->IsLengthy();

    if (action->Flag(AFLAG_IS_SPELL) && IsEnchanted(ENCHANT_NULL)) {
        return false;
    }
    if (action->Flag(AFLAG_IS_ABILITY) && 
        cooldowns[dynamic_cast<const Ability*>(action)->cooldown_index()] > 0) {
            return false;
    }

    if (action->Flag(AFLAG_IS_QUICK)) {
        if (!can_act) { return false; }
    } else {
        if (can_act < 2) {return false; }
    }

    if (!is_lengthy && action->Flag(AFLAG_CANT_TARGET_SELF)) {
        if (coor == dest) { return false; } //DONT TARGET YOURSELF >(
    }
    if (!is_lengthy && action->Flag(AFLAG_MUST_TARGET_EMPTY)) {
        if (!target) { return false; }
    }
    if (!is_lengthy && action->Flag(AFLAG_MUST_TARGET_UNIT)) {
        if (!target || !target->Flag(ENT_IS_UNIT)) { return false; }
    }
    if (action->Flag(AFLAG_CANT_BE_ANCHORED)) {
        if (enchants[ENCHANT_ANCHOR]) { return false; }
    }
    if (action->Flag(AFLAG_NEED_AMMO)) {
        if (!ammo) { return false; }
    }
    if (action->Flag(AFLAG_MUST_TURN_TO_TARGET) || action->Flag(AFLAG_IS_MELEE)) {    //melee always requires turning
        if (coor.DefineDirection(dest) != dir && !movepoints) { return false; } //directions not match and no points left to turn
    }
    if (action->Flag(AFLAG_IS_MELEE)) {
        if ((coor-dest).intlength(true) != 1) { return false; } //too far away
    }
    if (action->Flag(AFLAG_IS_TARGETLESS)) {
        if (coor != dest) { return false; }
    }
    if (!is_lengthy && action->Flag(AFLAG_RANGE_LIMITED)) {
        if ((coor-dest).intlength(true) > action->Range()) { return false; }
    }
    if (action->Flag(AFLAG_IS_SPELL)) {
        if (mp < dynamic_cast<const Spell*>(action)->Cost()) { return false; }
    }
    if (action->Flag(AFLAG_IS_ABILITY)) {
        if (!cooldowns[ dynamic_cast<const Ability*>(action)->cooldown_index() ]) { return false; }
    }
    if (!is_lengthy && action->Flag(AFLAG_MUST_TARGET_VISIBLE)) {
        if (!IsVisible) { return false; }
    }
    return true;
}
bool Unit::PerformAction(const vInt& dest, const Entity* target, const int action_id, bool IsVisible) {
    const DefaultUnit& me = GetPrototype();
    if(me.Actions.size() <= (unsigned int)action_id) { return false; } //wrong action
    const Action* action = me.Actions[action_id];
    bool is_lengthy = action->IsLengthy();

    if (CheckForValidity(dest, target, GetPrototype().Actions[action_id], IsVisible)) {
        ApplyPenalties(dest, action);
        if (is_lengthy) {
            lspell = LSpell(action->TimeToPerform(), vInt(dest), action);
            return true;
        } else {
            return action->Perform(this, dest);
        }
    } else {
        return false;
    }
}

void Entity::ApplyPenalties(const vInt& dest, const Action* action) {
}
void Unit::ApplyPenalties(const vInt& dest, const Action* action) {
    if (action->Flag(AFLAG_IS_QUICK)) {
        can_act--;
    } else {
        can_act -= 2;
    }
    if (action->Flag(AFLAG_NEED_AMMO)) {
        ammo--;
    }
    if (action->Flag(AFLAG_MUST_TURN_TO_TARGET) || action->Flag(AFLAG_IS_MELEE)) {
        if (coor.DefineDirection(dest) != dir) { movepoints--;}
    }
    if (action->Flag(AFLAG_IS_SPELL)) {
        mp -= dynamic_cast<const Spell*>(action)->Cost();
    }
    if (action->Flag(AFLAG_IS_ABILITY)) {
        cooldowns[ dynamic_cast<const Ability*>(action)->cooldown_index() ] = 
            dynamic_cast<const Ability*>(action)->cooldown_value();
    }
    BlockingState() = false;
}

#pragma warning(disable:4244)
void Entity::GetDamage(Dmg damage) {
    hp = max(0, hp-damage.value);
}
void Unit::GetDamage(Dmg damage) {
    switch (damage.type){
    case DMG_FIRE:
        damage.value = ceil(static_cast<double>(damage.value) * (100 - FireResist()) / 100);
    case DMG_SHOCK:
        damage.value = ceil(static_cast<double>(damage.value) * (100 - ShockResist()) / 100);
    case DMG_VENOM:
        damage.value = ceil(static_cast<double>(damage.value) * (100 - VenomResist()) / 100);
    case DMG_PHYS:
        damage.value = ceil(static_cast<double>(damage.value) * (100 - Armor()) / 100);
    case DMG_BLUDGEON:
        damage.value = ceil(static_cast<double>(damage.value) * (100 - Armor()/2) / 100);
    };
    if (IsEnchanted(ENCHANT_FORCE_FIELD)) {
        damage.value /= 2;
        enchants[ENCHANT_FORCE_FIELD] = max(0, enchants[ENCHANT_FORCE_FIELD] - damage.value);
        hp = max(1, hp - damage.value);
    }
    
}
void Unit::CastEnchant(const Enchant& enchant) {
    enumEnchants id = static_cast<enumEnchants>(enchant.id);
    if (!IsEnchanted(id)) {
        switch (id){
        case ENCHANT_SLOW:
        case ENCHANT_HASTE:
            movepoints += (Speedtest(id) - Speed());
            break;
        case ENCHANT_STUN:
            movepoints = 0;
            break;
        case NO_ENCHANT:
            return;
        };
    }

    //enchant.sound_on.PlaySound();
    enchants[id] = enchant.time_to_last;
}

void Entity::Turn(const Direction dir) { this->dir = dir;}
void Unit::Turn(const Direction dir) { 
    if (this->dir != dir) {
        this->dir = dir;
        movepoints--;    
    }
}

void Entity::StartTurn(){
    hp += GetPrototype().reghp;
    mp += GetPrototype().regmp;
}
void Entity::EndTurn(){}
void Unit::StartTurn(){
    hp += GetPrototype().reghp;
    mp += GetPrototype().regmp;
    movepoints = Speed();
    if (!IsBusy()) {
        can_act = 2;
    }
    for (int i = 0; i < Counters::cooldowns; ++i) {
        if (cooldowns[i] > 0) {cooldowns[i]--;}
    }
}
void Unit::EndTurn(){
    switch (GetPrototype().weapon.block_quality) {
    case BLOCK_STAFF:
    case BLOCK_LONGSWORD:
    case BLOCK_QUICKSHIELD:
        if (can_act != 0 && movepoints != 0) {
            BlockingState() = true;
        }
        break;
    case BLOCK_SHIELD:
        if (can_act == 2 && movepoints == Speed()) {
            BlockingState() = true;
        }
        break;
    };
}

void Entity::Move(const vInt& dest, const Direction dir){
    coor = dest;
    if (dir != NO_DIRECTION) {
        this->dir = dir;
    }
}
void Unit::Move(const vInt& dest, const Direction dir) {}

void Unit::DrainMana(Entity *source) {
    int mana_available = min(source->mp, mana_drain_per_turn);
    int mana_needed = GetPrototype().mp - mp;
    int mana = min(mana_needed, mana_available);
    mp += mana;
    source->mp -= mana;

    int mana_per_ammo = GetPrototype().weapon.mana_per_ammo;
    if (source->mp != 0 &&
        mana_per_ammo != 0 && 
        GetPrototype().ammo != ammo) {
            
        mana_available = min(source->mp, mana_drain_per_turn);
        mana_needed = mana_per_ammo * (GetPrototype().ammo - ammo) - ammo_manabuffer;
        mana = min(mana_needed, mana_available);

        ammo += (mana + ammo_manabuffer / mana_per_ammo);
        ammo_manabuffer = (mana + ammo_manabuffer % mana_per_ammo);
        source->mp -= mana;
    }
}
