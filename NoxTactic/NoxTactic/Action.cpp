#include "Action.h"
//#include "Sound.h"
#include "Strings.h"
#include "Constants.h"
#include "Graphics.h"

#undef PlaySound
Action::Action(void (*action)(Entity*, const vInt&, const Action*),
    int range, int damage1, int damage2, int timevalue, int performtime,
    Sound& sound, enumStrings name, enumStrings msg_done, enumStrings msg_started, 
    enumStrings msg_stopped, enumStrings msg_affected, enumStrings msg_killed, int flags,
    int icon_texture_id, int active_icon_texture_id):
               action(action), range(range), damage1(damage1), damage2(damage2), time_value(timevalue),
               time_to_perform(performtime), sound(sound), name(name), msg_performed(msg_done),
               msg_started(msg_started), msg_stopped(msg_stopped), msg_affected(msg_affected),
               msg_killed(msg_killed), flags(flags), 
               icon_texture_id(icon_texture_id), active_icon_texture_id(active_icon_texture_id){}
Spell::Spell(Action action, int manacost, enumGestures g1, enumGestures g2, enumGestures g3, 
    enumGestures g4, enumGestures g5, enumGestures g6, enumGestures g7, enumGestures g8):
    Action(action), manacost(manacost){
                gesture[0] = g1;
                gesture[1] = g2;
                gesture[2] = g3;
                gesture[3] = g4;
                gesture[4] = g5;
                gesture[5] = g6;
                gesture[6] = g7;
                gesture[7] = g8;
    }
Ability::Ability(Action action, int cooldown, enumCooldowns cd_index) :
    Action(action), cooldown(cooldown), cd_index(cd_index) {}

ContinuousSpell::ContinuousSpell(Spell spell, enumContinuousSpells id, 
    void (*continuous_action)(Entity*, Entity*, const Action*)): 
    Spell(spell),
    continuous_action(continuous_action), id(id){}

EnchantSpell::EnchantSpell(Spell spell, enumEnchants id) :
    Spell(spell), id(id){}

    //implement common action funcs (continuous, enchanting, projectiles, melee strikes)
bool ContinuousSpell::ApplyEffect(Entity* actor, Entity* target) const {
    continuous_action(actor, target, this);
    return true;
}
bool ContinuousSpell::Perform(Entity* actor, const vInt& target) const {
    action(actor, target, this);
    return true;
}
bool Action::Perform(Entity* actor, const vInt& target) const {
    action(actor, target, this);
    return true;
}
bool Spell::Perform(Entity* actor, const vInt& target) const {
    action(actor, target, this);
    return true;
}
bool Ability::Perform(Entity* actor, const vInt& target) const {
    action(actor, target, this);
    return true;
}
bool EnchantSpell::Perform(Entity* actor, const vInt& target) const {
    return this->Spell::Perform(actor, target);
}
Gesture::Gesture(const Sound& sound, const int number): 
sound(sound), powernumber(number){}
void Gesture::CastMe(const int volume) const {
	sound.PlaySound(volume);
}
Enchant::Enchant(const enumStrings name, const Sound& snd_on, const Sound& snd_off, const int time, const int id, const bool wear_off_auto):
name(name), sound_on(snd_on), sound_off(snd_off), time_to_last(time), id(id), does_wearoff_auto(wear_off_auto) {}


int get_icon(std::string AppPath, const std::string& name) {
    return GIH->AddTexture(AppPath + Paths::ActionsIcons + name + common_extension);
}
int get_active_icon(std::string AppPath, const std::string& name) {
    return GIH->AddTexture(AppPath + Paths::ActionsActiveIcons + name + common_extension);
}

void Loader::loadActions(ActionContainer& container, SoundContainer& sounds, 
    FunctionContainer& functions, ContinuousFunctionContainer& continuous_functions,
    std::string AppPath, StringContainer strings) {
    container.reserve(Counters::actions);
    FunctionContainer::iterator iter = functions.begin();
    ContinuousFunctionContainer::iterator continuous_iter = continuous_functions.begin();

    container.push_back(new Action(*(iter++), 0, 0, 0, 0, 0, sounds[NO_SOUND], NO_STRING,
        NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 0, 0, 0));
    
    container.push_back(new Spell(Action(*(iter++), 0, 127, 0, 4, 0, sounds[SND_FireballMove], 
        /*strng*/STR_ACTION_FIREBALL, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_CANT_TARGET_SELF + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TURN_TO_TARGET,
        get_icon(AppPath, strings[STR_ACTION_FIREBALL]), get_active_icon(AppPath, strings[STR_ACTION_FIREBALL])), 
        /*spell*/30, G_ZO, G_ZO, G_UN, G_, G_, G_, G_, G_));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantMove],
        /*strng*/STR_ACTION_ANCHOR, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE,
        get_icon(AppPath, strings[STR_ACTION_ANCHOR]), get_active_icon(AppPath, strings[STR_ACTION_ANCHOR])),
        /*spell*/10, G_ZO, G_ZO, G_, G_, G_, G_, G_, G_), ENCHANT_ANCHOR));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 30, 0, 10, 0, sounds[SND_BurnCast],
        /*strng*/STR_ACTION_BURN, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING,NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_MUST_TARGET_VISIBLE,
        get_icon(AppPath, strings[STR_ACTION_BURN]), get_active_icon(AppPath, strings[STR_ACTION_BURN])),
        /*spell*/10, G_ZO, G_ZO, G_UN, G_UN, G_, G_, G_, G_));
    container.push_back(new ContinuousSpell(Spell(Action(*(iter++), 0, 40, 40, 0, 0, sounds[SND_GHealOff],
        /*strng*/STR_ACTION_CHANNEL, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS + AFLAG_IS_CONTINUOUS,
        get_icon(AppPath, strings[STR_ACTION_CHANNEL]), get_active_icon(AppPath, strings[STR_ACTION_CHANNEL])),
        /*spell*/0, G_KA, G_ZO, G_IN, G_, G_, G_, G_, G_), CONT_CHANNEL_LIFE, *(continuous_iter++)));
    container.push_back(new Spell(Action(*(iter++), visibility_range / 2, 0, 0, 0, 0, sounds[SND_CounterSpellCast],
        /*strng*/STR_ACTION_COUNTERSPELL, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS + AFLAG_IS_AGGRESSIVE,
        get_icon(AppPath, strings[STR_ACTION_COUNTERSPELL]), get_active_icon(AppPath, strings[STR_ACTION_COUNTERSPELL])),
        /*spell*/20, G_ZO, G_DO, G_, G_, G_, G_, G_, G_));
    container.push_back(new Spell(Action(*(iter++), 2, 200, 0, 3, 2, sounds[SND_DispellUndeadCast],
        /*strng*/STR_ACTION_DISPELL_UNDEAD, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS + AFLAG_IS_AGGRESSIVE,
        get_icon(AppPath, strings[STR_ACTION_DISPELL_UNDEAD]), get_active_icon(AppPath, strings[STR_ACTION_DISPELL_UNDEAD])),
        /*spell*/60, G_UN, G_KA, G_ET, G_RO, G_ZO, G_DO, G_CHA, G_IN));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 100, 0, 0, 0, sounds[SND_DeathRayCast],
        /*strng*/STR_ACTION_DEATHRAY, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_MUST_TARGET_VISIBLE + AFLAG_IS_AGGRESSIVE + AFLAG_CANT_TARGET_SELF + AFLAG_MUST_TURN_TO_TARGET,
        get_icon(AppPath, strings[STR_ACTION_DEATHRAY]), get_active_icon(AppPath, strings[STR_ACTION_DEATHRAY])),
        /*spell*/60, G_DO, G_DO, G_, G_, G_, G_, G_, G_));
    container.push_back(new ContinuousSpell(Spell(Action(*(iter++), visibility_range/2 + 1, 40, 40, 0, 0, sounds[SND_EboltCast],
        /*strng*/STR_ACTION_ENERGY_BOLT, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_IS_CONTINUOUS + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_TARGET_SELF,
        get_icon(AppPath, strings[STR_ACTION_ENERGY_BOLT]), get_active_icon(AppPath, strings[STR_ACTION_ENERGY_BOLT])),
        /*spell*/0, G_ZO, G_CHA, G_ET, G_UN, G_, G_, G_, G_), CONT_ENERGY_BOLT, *(continuous_iter++)));
    container.push_back(new Spell(Action(*(iter++), visibility_range/2 + 1, 40, 8, 0, 2, sounds[SND_EarthquakeCast],
        /*strng*/STR_ACTION_EARTHQUAKE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS + AFLAG_IS_AGGRESSIVE,
        get_icon(AppPath, strings[STR_ACTION_EARTHQUAKE]), get_active_icon(AppPath, strings[STR_ACTION_EARTHQUAKE])),
        /*spell*/60, G_DO, G_ET, G_IN, G_RO, G_CHA, G_KA, G_, G_));
    container.push_back(new ContinuousSpell(Spell(Action(*(iter++), visibility_range/2 + 1, 40, 30, 0, 2, sounds[SND_LightningCast],
        /*strng*/STR_ACTION_LIGHTNING, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_IS_CONTINUOUS + AFLAG_CANT_TARGET_SELF + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET,
        get_icon(AppPath, strings[STR_ACTION_LIGHTNING]), get_active_icon(AppPath, strings[STR_ACTION_LIGHTNING])),
        /*spell*/0, G_ZO, G_CHA, G_ET, G_UN, G_ET, G_CHA, G_ZO, G_), CONT_LIGHTNING, *(continuous_iter++)));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        /*strng*/STR_ACTION_PROTECTION_FIRE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET,
        get_icon(AppPath, strings[STR_ACTION_PROTECTION_FIRE]), get_active_icon(AppPath, strings[STR_ACTION_PROTECTION_FIRE])),
        /*spell*/30, G_ET, G_CHA, G_DO, G_KA, G_, G_, G_, G_), ENCHANT_PROTECTION_FIRE));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        /*strng*/STR_ACTION_PROTECTION_SHOCK, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET,
        get_icon(AppPath, strings[STR_ACTION_PROTECTION_SHOCK]), get_active_icon(AppPath, strings[STR_ACTION_PROTECTION_SHOCK])),
        /*spell*/30, G_CHA, G_ET, G_DO, G_KA, G_, G_, G_, G_), ENCHANT_PROTECTION_SHOCK));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        /*strng*/STR_ACTION_PROTECTION_VENOM, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET,
        get_icon(AppPath, strings[STR_ACTION_PROTECTION_VENOM]), get_active_icon(AppPath, strings[STR_ACTION_PROTECTION_VENOM])),
        /*spell*/30, G_ET, G_CHA, G_RO, G_IN, G_, G_, G_, G_), ENCHANT_PROTECTION_POISON));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 50, 2, sounds[SND_EnchantCast],
        /*strng*/STR_ACTION_FORCEFIELD, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET,
        get_icon(AppPath, strings[STR_ACTION_FORCEFIELD]), get_active_icon(AppPath, strings[STR_ACTION_FORCEFIELD])),
        /*spell*/60, G_UN, G_ET, G_ZO, G_CHA, G_UN, G_ET, G_ZO, G_CHA), ENCHANT_FORCE_FIELD));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 15, 0, 0, 0, sounds[SND_EnchantCast],
        /*strng*/STR_ACTION_HEAL_LESSER, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET,
        get_icon(AppPath, strings[STR_ACTION_HEAL_LESSER]), get_active_icon(AppPath, strings[STR_ACTION_HEAL_LESSER])),
        /*spell*/30, G_DO, G_UN, G_RO, G_, G_, G_, G_, G_));
    container.push_back(new ContinuousSpell(Spell(Action(*(iter++), visibility_range/2 + 1, 40, 40, 0, 0, sounds[SND_GHealOn],
        /*strng*/STR_ACTION_HEAL_GREATER, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_MUST_TARGET_UNIT + AFLAG_IS_CONTINUOUS + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET,
        get_icon(AppPath, strings[STR_ACTION_HEAL_GREATER]), get_active_icon(AppPath, strings[STR_ACTION_HEAL_GREATER])),
        /*spell*/0, G_RO, G_UN, G_DO, G_, G_, G_, G_, G_), CONT_GREATER_HEAL, *(continuous_iter++)));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        /*strng*/STR_ACTION_HASTE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET,
        get_icon(AppPath, strings[STR_ACTION_HASTE]), get_active_icon(AppPath, strings[STR_ACTION_HASTE])),
        /*spell*/10, G_ET, G_CHA, G_CHA, G_, G_, G_, G_, G_), ENCHANT_HASTE));
    container.push_back(new Spell(Action(*(iter++), visibility_range/2, 0, 0, 0, 0, sounds[SND_Inversion],
        /*strng*/STR_ACTION_INVERSION, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS,
        get_icon(AppPath, strings[STR_ACTION_INVERSION]), get_active_icon(AppPath, strings[STR_ACTION_INVERSION])),
        /*spell*/10, G_KA, G_IN, G_, G_, G_, G_, G_, G_));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 40, 0, 4, 0, sounds[SND_MissilesMove],
        /*strng*/STR_ACTION_MAGIC_MISSILE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_TARGET_SELF,
        get_icon(AppPath, strings[STR_ACTION_MAGIC_MISSILE]), get_active_icon(AppPath, strings[STR_ACTION_MAGIC_MISSILE])),
        /*spell*/20, G_ET, G_UN, G_CHA, G_UN, G_, G_, G_, G_));
    container.push_back(new ContinuousSpell(Spell(Action(*(iter++), visibility_range/2 + 1, 40, 0, 0, 0, sounds[SND_ManaDrainCast],
        /*strng*/STR_ACTION_MANA_DRAIN, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS + AFLAG_IS_CONTINUOUS,
        get_icon(AppPath, strings[STR_ACTION_MANA_DRAIN]), get_active_icon(AppPath, strings[STR_ACTION_MANA_DRAIN])),
        /*spell*/0, G_UN, G_KA, G_ZO, G_IN, G_, G_, G_, G_), CONT_DRAIN_MANA, *(continuous_iter++)));
    container.push_back(new Spell(Action(*(iter++), visibility_range/2, 0, 0, 0, 0, sounds[SND_Pull],
        /*strng*/STR_ACTION_PULL, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS,
        get_icon(AppPath, strings[STR_ACTION_PULL]), get_active_icon(AppPath, strings[STR_ACTION_PULL])),
        /*spell*/10, G_UN, G_UN, G_ZO, G_, G_, G_, G_, G_));
    container.push_back(new Spell(Action(*(iter++), visibility_range/2, 0, 0, 0, 0, sounds[SND_Push],
        /*strng*/STR_ACTION_PUSH, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS,
        get_icon(AppPath, strings[STR_ACTION_PUSH]), get_active_icon(AppPath, strings[STR_ACTION_PUSH])),
        /*spell*/10, G_UN, G_UN, G_UN, G_, G_, G_, G_, G_));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), 0, 0, 0, 0, 0, sounds[SND_RSOn],
        /*strng*/STR_ACTION_REFLECTIVE_SHIELD, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_IS_TARGETLESS,
        get_icon(AppPath, strings[STR_ACTION_REFLECTIVE_SHIELD]), get_active_icon(AppPath, strings[STR_ACTION_REFLECTIVE_SHIELD])),
        /*spell*/30, G_UN, G_ZO, G_ZO, G_RO, G_DO, G_, G_, G_), ENCHANT_REFLECTIVE_SHIELD));
    container.push_back(new Spell(Action(*(iter++), 2, 30, 0, 3, 0, sounds[SND_RingOfFireCast],
        /*strng*/STR_ACTION_FIRE_RING, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_IS_TARGETLESS,
        get_icon(AppPath, strings[STR_ACTION_FIRE_RING]), get_active_icon(AppPath, strings[STR_ACTION_FIRE_RING])),
        /*spell*/60, G_DO, G_ZO, G_RO, G_UN, G_, G_, G_, G_));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 45, 0, 0, 0, sounds[SND_EnchantCast],
        /*strng*/STR_ACTION_SHOCK, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET,
        get_icon(AppPath, strings[STR_ACTION_SHOCK]), get_active_icon(AppPath, strings[STR_ACTION_SHOCK])),
        /*spell*/30, G_ZO, G_CHA, G_ET, G_ET, G_, G_, G_, G_), ENCHANT_SHOCK));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        /*strng*/STR_ACTION_SLOW, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET,
        get_icon(AppPath, strings[STR_ACTION_SLOW]), get_active_icon(AppPath, strings[STR_ACTION_SLOW])),
        /*spell*/15, G_ZO, G_ZO, G_ZO, G_, G_, G_, G_, G_), ENCHANT_SLOW));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        /*strng*/STR_ACTION_SWAP, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_BE_ANCHORED + AFLAG_CANT_TARGET_SELF,
        get_icon(AppPath, strings[STR_ACTION_SWAP]), get_active_icon(AppPath, strings[STR_ACTION_SWAP])),
        /*spell*/15, G_UN, G_UN, G_ZO, G_ZO, G_, G_, G_, G_));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_Teleport],
        /*strng*/STR_ACTION_TELEPORT, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_MUST_TARGET_EMPTY + AFLAG_CANT_BE_ANCHORED,
        get_icon(AppPath, strings[STR_ACTION_TELEPORT]), get_active_icon(AppPath, strings[STR_ACTION_TELEPORT])),
        /*spell*/20, G_ZO, G_UN, G_ET, G_CHA, G_, G_, G_, G_));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 255, 0, 0, 0, sounds[SND_WallOn],
        /*strng*/STR_ACTION_WALL, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_MUST_TARGET_EMPTY + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET,
        get_icon(AppPath, strings[STR_ACTION_WALL]), get_active_icon(AppPath, strings[STR_ACTION_WALL])),
        /*spell*/30, G_KA, G_UN, G_IN, G_, G_, G_, G_, G_));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 250, 0, 1, 0, sounds[SND_FistCast],
        /*strng*/STR_ACTION_FIST_OF_VENGEANCE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_AGGRESSIVE,
        get_icon(AppPath, strings[STR_ACTION_FIST_OF_VENGEANCE]), get_active_icon(AppPath, strings[STR_ACTION_FIST_OF_VENGEANCE])),
        /*spell*/60, G_KA, G_IN, G_UN, G_ZO, G_, G_, G_, G_));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        /*strng*/STR_ACTION_VAMPIRISM, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET,
        get_icon(AppPath, strings[STR_ACTION_VAMPIRISM]), get_active_icon(AppPath, strings[STR_ACTION_VAMPIRISM])),
        /*spell*/30, G_UN, G_ZO, G_ET, G_CHA, G_, G_, G_, G_), ENCHANT_VAMPIRISM));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        /*strng*/STR_ACTION_STUN, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_AGGRESSIVE,
        get_icon(AppPath, strings[STR_ACTION_STUN]), get_active_icon(AppPath, strings[STR_ACTION_STUN])),
        /*spell*/10, G_KA, G_ZO, G_, G_, G_, G_, G_, G_), ENCHANT_STUN));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_ToxicCloudCast],
        /*strng*/STR_ACTION_TOXIC_CLOUD, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_AGGRESSIVE,
        get_icon(AppPath, strings[STR_ACTION_TOXIC_CLOUD]), get_active_icon(AppPath, strings[STR_ACTION_TOXIC_CLOUD])),
        /*spell*/60, G_IN, G_RO, G_KA, G_, G_, G_, G_, G_));
    container.push_back(new Spell(Action(*(iter++), 0, 800, 30, 3, 2, sounds[SND_ForceOfNatureCast],
        /*strng*/STR_ACTION_FORCE_OF_NATURE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_AGGRESSIVE + AFLAG_CANT_TARGET_SELF,
        get_icon(AppPath, strings[STR_ACTION_FORCE_OF_NATURE]), get_active_icon(AppPath, strings[STR_ACTION_FORCE_OF_NATURE])),
        /*spell*/60, G_DO, G_RO, G_ZO, G_, G_, G_, G_, G_));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), 0, 40, 0, 0, 0, sounds[SND_PixiesCast],
        /*strng*/STR_ACTION_PIXIES, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_IS_AGGRESSIVE + AFLAG_IS_TARGETLESS,
        get_icon(AppPath, strings[STR_ACTION_PIXIES]), get_active_icon(AppPath, strings[STR_ACTION_PIXIES])),
        /*spell*/30, G_ET, G_ZO, G_CHA, G_ZO, G_, G_, G_, G_), ENCHANT_PIXIES));
    container.push_back(new ContinuousSpell(Spell(Action(*(iter++), visibility_range/2 + 1, 0, 0, 0, 0, sounds[SND_CharmCast],
        /*strng*/STR_ACTION_CHARM, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_IS_CONTINUOUS + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_TARGET_SELF,
        get_icon(AppPath, strings[STR_ACTION_CHARM]), get_active_icon(AppPath, strings[STR_ACTION_CHARM])),
        /*spell*/10, G_KA, G_IN, G_ZO, G_, G_, G_, G_, G_), CONT_CHARM, *(continuous_iter++)));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 60, 0, 0, 0, sounds[SND_MeteorCast],
        /*strng*/STR_ACTION_METEOR, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_AGGRESSIVE,
        get_icon(AppPath, strings[STR_ACTION_METEOR]), get_active_icon(AppPath, strings[STR_ACTION_METEOR])),
        /*spell*/30, G_RO, G_RO, G_, G_, G_, G_, G_, G_));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        /*strng*/STR_ACTION_POISON, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_TARGET_SELF,
        get_icon(AppPath, strings[STR_ACTION_POISON]), get_active_icon(AppPath, strings[STR_ACTION_POISON])),
        /*spell*/10, G_IN, G_RO, G_, G_, G_, G_, G_, G_), ENCHANT_POISON));
    container.push_back(new ContinuousSpell(Spell(Action(*(iter++), visibility_range, 50, 4, 0, 2, sounds[NO_SOUND],
        /*strng*/STR_ACTION_OBLITERATION, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        /*flags*/AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_IS_CONTINUOUS + AFLAG_IS_TARGETLESS,
        get_icon(AppPath, strings[STR_ACTION_OBLITERATION]), get_active_icon(AppPath, strings[STR_ACTION_OBLITERATION])),
        /*spell*/10, G_UN, G_UN, G_ET, G_ET, G_ZO, G_ZO, G_CHA, G_CHA), CONT_OBLITERATION, *(continuous_iter++)));
    container.push_back(new Ability(Action(*(iter++), visibility_range, 150, 20, 0, 0, sounds[SND_Berserker],
        /*strng*/STR_ACTION_BERSERKER, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        /*flags*/AFLAG_IS_ABILITY + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_TARGET_SELF,
        get_icon(AppPath, strings[STR_ACTION_BERSERKER]), get_active_icon(AppPath, strings[STR_ACTION_BERSERKER])),
        /*ablty*/5, CD_BERSERKER));
    container.push_back(new Ability(Action(*(iter++), visibility_range/2, 0, 0, 0, 0, sounds[SND_HarpoonReleased],
        /*strng*/STR_ACTION_HARPOON, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        /*flags*/AFLAG_IS_ABILITY + AFLAG_IS_AGGRESSIVE + AFLAG_CANT_TARGET_SELF + AFLAG_MUST_TURN_TO_TARGET,
        get_icon(AppPath, strings[STR_ACTION_HARPOON]), get_active_icon(AppPath, strings[STR_ACTION_HARPOON])),
        /*ablty*/4, CD_HARPOON));
    container.push_back(new Ability(Action(*(iter++), visibility_range/2 + 1, 0, 0, 0, 0, sounds[SND_WarCry],
        /*strng*/STR_ACTION_WARCRY, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        /*flags*/AFLAG_IS_ABILITY + AFLAG_IS_AGGRESSIVE + AFLAG_IS_TARGETLESS,
        get_icon(AppPath, strings[STR_ACTION_WARCRY]), get_active_icon(AppPath, strings[STR_ACTION_WARCRY])),
        /*ablty*/5, CD_WARCRY));
    container.push_back(new Action(*(iter++), 1, 130, 0, 0, 2, sounds[SND_HammerStrike],
        /*strng*/STR_ACTION_HAMMER, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        /*flags*/AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_IS_MELEE,
        get_icon(AppPath, strings[STR_ACTION_HAMMER]), get_active_icon(AppPath, strings[STR_ACTION_HAMMER])));
    container.push_back(new Action(*(iter++), 1, 80, 0, 0, 0, sounds[SND_LongSwordStrike],
        /*strng*/STR_ACTION_LONGSWORD, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        /*flags*/AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_IS_MELEE,
        get_icon(AppPath, strings[STR_ACTION_LONGSWORD]), get_active_icon(AppPath, strings[STR_ACTION_LONGSWORD])));
    container.push_back(new Action(*(iter++), 1, 40, 0, 0, 0, sounds[NO_SOUND],
        /*strng*/STR_ACTION_MACE, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        /*flags*/AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_IS_MELEE,
        get_icon(AppPath, strings[STR_ACTION_MACE]), get_active_icon(AppPath, strings[STR_ACTION_MACE])));
    container.push_back(new Action(*(iter++), 0, 64, 0, 4, 0, sounds[SND_BowShot],
        /*strng*/STR_ACTION_BOW, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        /*flags*/AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO,
        get_icon(AppPath, strings[STR_ACTION_BOW]), get_active_icon(AppPath, strings[STR_ACTION_BOW])));
    container.push_back(new Action(*(iter++), visibility_range/2 + 1, 40, 30, 0, 0 ,sounds[SND_LightningCast],
        /*strng*/STR_ACTION_FORCE_STAFF, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        /*flags*/AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO+AFLAG_MUST_TARGET_VISIBLE+AFLAG_MUST_TARGET_UNIT,
        get_icon(AppPath, strings[STR_ACTION_FORCE_STAFF]), get_active_icon(AppPath, strings[STR_ACTION_FORCE_STAFF])));
    container.push_back(new Action(*(iter++), 0, 64, 0, 4, 0, sounds[SND_FireStaffShot],
        /*strng*/STR_ACTION_FIRE_STAFF, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        /*flags*/AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO,
        get_icon(AppPath, strings[STR_ACTION_FIRE_STAFF]), get_active_icon(AppPath, strings[STR_ACTION_FIRE_STAFF])));
    container.push_back(new Action(*(iter++), 0, 64, 0, 4, 0, sounds[SND_TripleStaffShot],
        /*strng*/STR_ACTION_HELLFIRE_STAFF, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        /*flags*/AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO,
        get_icon(AppPath, strings[STR_ACTION_HELLFIRE_STAFF]), get_active_icon(AppPath, strings[STR_ACTION_HELLFIRE_STAFF])));
    container.push_back(new Action(*(iter++), 0, 40, 0, 4, 0, sounds[SND_ShurikenThrow],
        /*strng*/STR_ACTION_SHURIKEN, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        /*flags*/AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO+AFLAG_IS_QUICK,
        get_icon(AppPath, strings[STR_ACTION_SHURIKEN]), get_active_icon(AppPath, strings[STR_ACTION_SHURIKEN])));
    container.push_back(new Action(*(iter++), 1, 40, 8, 0, 0, sounds[SND_LongSwordStrike],
        /*strng*/STR_ACTION_FIRESWORD, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        /*flags*/AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_IS_MELEE+AFLAG_IS_QUICK,
        get_icon(AppPath, strings[STR_ACTION_FIRESWORD]), get_active_icon(AppPath, strings[STR_ACTION_FIRESWORD])));
    container.push_back(new Action(*(iter++), 0, 0, 0, 0, 2, sounds[SND_ForceOfNatureCast],
        /*strng*/STR_ACTION_FON_STAFF, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        /*flags*/AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO,
        get_icon(AppPath, strings[STR_ACTION_FON_STAFF]), get_active_icon(AppPath, strings[STR_ACTION_FON_STAFF])));
}
void Loader::loadGestures(GestureContainer& container, SoundContainer& sounds) {
    container.reserve(Counters::gestures);
    container.push_back(Gesture(sounds[NO_SOUND],0));
    container.push_back(Gesture(sounds[SND_GESTURE_UN],1));
    container.push_back(Gesture(sounds[SND_GESTURE_KA],2));
    container.push_back(Gesture(sounds[SND_GESTURE_ET],3));
    container.push_back(Gesture(sounds[SND_GESTURE_RO],4));
    container.push_back(Gesture(sounds[SND_GESTURE_ZO],5));
    container.push_back(Gesture(sounds[SND_GESTURE_DO],6));
    container.push_back(Gesture(sounds[SND_GESTURE_CHA],7));
    container.push_back(Gesture(sounds[SND_GESTURE_IN],8));
}
void Loader::loadEnchants(EnchantContainer& container, SoundContainer& sounds) {
    container.reserve(Counters::enchants);
    container.push_back(Enchant(STR_ENCHANT_ANCHOR,            sounds[SND_AnchorOn]     , sounds[SND_AnchorOff]    , 1, 8));
    container.push_back(Enchant(STR_ENCHANT_FORCE_FIELD,       sounds[SND_ForceFieldOn] , sounds[SND_ForceFieldOff], 2, 50, false));
    container.push_back(Enchant(STR_ENCHANT_HASTE,             sounds[SND_HasteOn]      , sounds[SND_HasteOff]     , 3, 4));
    container.push_back(Enchant(STR_ENCHANT_PIXIES,            sounds[NO_SOUND]         , sounds[NO_SOUND]         , 4, 0, false));
    container.push_back(Enchant(STR_ENCHANT_POISON,            sounds[SND_PoisonOn]     , sounds[SND_PoisonOff]    , 5, 8));
    container.push_back(Enchant(STR_ENCHANT_PROTECTION_FIRE,   sounds[SND_ProtectionOn] , sounds[SND_ProtectionOff], 6, 6));
    container.push_back(Enchant(STR_ENCHANT_PROTECTION_POISON, sounds[SND_ProtectionOn] , sounds[SND_ProtectionOff], 7, 6));
    container.push_back(Enchant(STR_ENCHANT_PROTECTION_SHOCK,  sounds[SND_ProtectionOn] , sounds[SND_ProtectionOff], 8, 6));
    container.push_back(Enchant(STR_ENCHANT_REFLECTIVE_SHIELD, sounds[SND_RSOn]         , sounds[SND_RSOff]        , 9, 0, false));
    container.push_back(Enchant(STR_ENCHANT_SLOW,              sounds[SND_SlowOn]       , sounds[SND_SlowOff]      , 10, 3));
    container.push_back(Enchant(STR_ENCHANT_STUN,              sounds[SND_StunOn]       , sounds[SND_StunOff]      , 11, 1));
    container.push_back(Enchant(STR_ENCHANT_VAMPIRISM,         sounds[SND_VampirismOn]  , sounds[SND_VampirismOff] , 12, 6));
    container.push_back(Enchant(STR_ENCHANT_SHOCK,             sounds[SND_ShockOn]      , sounds[SND_ShockOff]     , 13, 6));
    container.push_back(Enchant(STR_ENCHANT_NULL,              sounds[SND_NullOn]       , sounds[SND_NullOff]      , 14, 3));
}
