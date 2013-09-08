#pragma once
#include "Vector.h"
const int visibility_range = 8;
const int actions_per_set = 5, actionsets_count = 5;
const int picsizex = 50, picsizey = 50;
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
}
const string common_extension = ".png";
namespace Paths 
{
    const string MoveArrow = "\\data\\pictures\\common\\Arrow";
    const string SelectionFrame = "\\data\\pictures\\common\\Selection" + common_extension;
    const string EntitiesPictures = "\\data\\pictures\\entities\\";
    const string ActionsIcons = "\\data\\pictures\\actions icons\\";
    const string ActionsActiveIcons = "\\data\\pictures\\actions icons\\active_";
}