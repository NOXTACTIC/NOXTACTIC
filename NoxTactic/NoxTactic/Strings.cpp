#include "Strings.h"
#include "Constants.h"
std::string int_to_string(int a) {
    char buf[100];
    _itoa_s(a, buf, 10);
    std::string tmp(buf);
    return tmp;
}
void Loader::loadStrings(StringContainer& container) {
    container.reserve(Counters::strings);
    container.push_back("");
    //Enchants
    container.push_back("Slow");
    container.push_back("Haste");
    container.push_back("Force Field");
    container.push_back("Reflective Shield");
    container.push_back("Protection from Fire");
    container.push_back("Protection from Shock");
    container.push_back("Protection from Poison");
    container.push_back("Stun");
    container.push_back("Pixies");
    container.push_back("Anchor");
    container.push_back("Vampirism");
    container.push_back("Poison");
    container.push_back("Shock");
    container.push_back("Nullification");
    //enumActions
    container.push_back("Fireball");
    container.push_back("Anchor");
    container.push_back("Burn");
    container.push_back("Channel");
    container.push_back("Counterspell");
    container.push_back("Death ray");
    container.push_back("Dispell Undead");
    container.push_back("Energy Bolt");
    container.push_back("Earthquake");
    container.push_back("Lightning");
    container.push_back("Protection from Fire");
    container.push_back("Protection from Shock");
    container.push_back("Protection from Poison");
    container.push_back("Force Field");
    container.push_back("Lesser Heal");
    container.push_back("Greater Heal");
    container.push_back("Haste");
    container.push_back("Inversion");
    container.push_back("Missiles of Magic");
    container.push_back("Mana Drain");
    container.push_back("Pull");
    container.push_back("Push");
    container.push_back("Reflective Shield");
    container.push_back("Ring of Fire");
    container.push_back("Shock");
    container.push_back("Slow");
    container.push_back("Swap Locations");
    container.push_back("Teleport to Target");
    container.push_back("Magic Wall");
    container.push_back("Fist of Vengeance");
    container.push_back("Vampirism");
    container.push_back("Stun");
    container.push_back("Toxic Cloud");
    container.push_back("Force of Nature");
    container.push_back("Pixie Swarm");
    container.push_back("Charm Creature");
    container.push_back("Meteor");
    container.push_back("Poison");
    container.push_back("Obliteration");
    container.push_back("Berserker Charge");
    container.push_back("Harpoon");
    container.push_back("War cry");
    container.push_back("Hammer strike");
    container.push_back("Longsword strike");
    container.push_back("Mace strike");
    container.push_back("Bow shot");
    container.push_back("Force Staff charge");
    container.push_back("Fire Staff shot");
    container.push_back("Hellfire Staff shot");
    container.push_back("Shuriken throw");
    container.push_back("Fire sword strike");
    container.push_back("Force of Nature Staff charge");
    //Entities
    container.push_back("Mage");
    container.push_back("Guardian wizard");
    container.push_back("Warlock");
    container.push_back("Conjurer");
    container.push_back("Ix priest");
    container.push_back("Fire knight");
    container.push_back("Warrior");
    container.push_back("Fireball");
    container.push_back("Fireball");
    container.push_back("Arrow");
    container.push_back("Shuriken");
    container.push_back("Force of Nature");
    container.push_back("Force of Nature piece");
    container.push_back("Incantation");
    container.push_back("Flame");
    container.push_back("Blue flame");  //зачем закладка? ќо
    container.push_back("Dispeller");
    container.push_back("Dispeller");
    container.push_back("Magic Missile");
    container.push_back("Pixie Swarm");
    container.push_back("Fist of Vengeance Shadow");
    container.push_back("Meteor Shadow");
    container.push_back("Flame");
    container.push_back("Magic Wall");
    container.push_back("Obelisk");
    container.push_back("Obelisk primitive");
    container.push_back("Obelisk LOTD");
    container.push_back("Teleport pentagram");
    container.push_back("Toxic Cloud");
    container.push_back("Wooden Door");
    container.push_back("Jail Door");
    //enumActions
    container.push_back("$u has casted a spell $s");
}