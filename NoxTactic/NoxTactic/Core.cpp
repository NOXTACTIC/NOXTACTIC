#include "Core.h"
#include "Action.h"
#include "Graphics.h"
#include "Grid.h"
#include <algorithm>
#include "Strings.h"
#pragma once
using namespace std;
Game *game;

#include <windows.h>
#include <windowsx.h>
#undef PlaySound
Game::Game() {
    
    //code taken from 
    //http://stackoverflow.com/questions/2647429/c-windows-path-to-the-folder-where-the-executable-is-located
    {
        HMODULE hModule = GetModuleHandleW(NULL);
        CHAR path[MAX_PATH];
        GetModuleFileName(hModule, path, MAX_PATH);

        AppPath = string(path);
        int dotpos = AppPath.find_last_of('\\');
        AppPath.erase(AppPath.begin() + dotpos, AppPath.end());
    }

    Selection_Texture = GIH->AddTexture(AppPath + Paths::SelectionFrame);
    for (char i = 0; i < Counters::directions; ++i) {
        MoveArrows_Texture[i] = GIH->AddTexture(AppPath + Paths::MoveArrow + int_to_string(i + 1) + common_extension);
    }

    load();
}
Game::~Game() {
    for_each(Maps.begin(), Maps.end(), [](Map map){
        map.Destroy();
    });
}
int Game::MoveArrow_Texture (Direction dir) {
    if (dir == NO_DIRECTION) { 
        return 0;
    } else {
        return MoveArrows_Texture[static_cast<int>(dir) - 1];
    }
}
int Game::SelectionFrame_Texture () {
    return Selection_Texture;
}

void MainLoop(); 
void Game::load() {
    Loader::FunctionContainer actions;
    Loader::ContinuousFunctionContainer continuous_actions;
    actions.resize(Counters::actions);
    continuous_actions.resize(Counters::continuous_spells);
    actions.push_back(nullptr); //NO_ACTION
    // fill the funcs arrays

    Loader::loadWalls(Walls, AppPath, Strings);
    Loader::loadTiles(Tiles, AppPath, Strings);
    Loader::loadStrings(Strings);
    Loader::loadSounds(Sounds);
    Loader::loadGestures(Gestures, Sounds);
    Loader::loadEnchants(Enchants, Sounds);
    Loader::loadActions(Actions, Sounds, actions, continuous_actions, AppPath, Strings);
    Loader::loadWeapons(Weapons, Actions);
    Loader::loadDefaultEntities(Entities, Actions, Sounds, Weapons, AppPath, Strings);
    Loader::loadMaps(Maps, Entities, Walls, Tiles);
}
void Map::Destroy() {
    for_each(entities.begin(), entities.end(), [](Entity *ent){
        delete ent;
    });
}

void Loader::loadMaps(MapContainer& container, EntityContainer& entities, WallContainer walls, TileContainer tiles){
    vector<Entity*> tmpent;
    vector<vector<Tile>> tmptiles(10, vector<Tile>(10, tiles[NO_TILE]));
    tmpent.reserve(6);
    tmpent.push_back(new Unit(*dynamic_cast<DefaultUnit*>(entities[ENT_WIZ_RED]), vInt(2,2), TEAM_BLUE, RIGHT));
    tmpent.push_back(new Unit(*dynamic_cast<DefaultUnit*>(entities[ENT_WIZ_BLUE]), vInt(2,3), TEAM_BLUE, RIGHT));
    tmpent.push_back(new Unit(*dynamic_cast<DefaultUnit*>(entities[ENT_WIZ_YELLOW]), vInt(2,4), TEAM_BLUE, RIGHT));
    tmpent.push_back(new Unit(*dynamic_cast<DefaultUnit*>(entities[ENT_WIZ_RED]), vInt(6,2), TEAM_RED, LEFT));
    tmpent.push_back(new Unit(*dynamic_cast<DefaultUnit*>(entities[ENT_WIZ_BLUE]), vInt(6,3), TEAM_RED, LEFT));
    tmpent.push_back(new Unit(*dynamic_cast<DefaultUnit*>(entities[ENT_WIZ_YELLOW]), vInt(6,4), TEAM_RED, LEFT));
    Map tmpmap = Map(vInt(10,10), tmpent, vector<WallCoor>(), tmptiles, 2);
    container.push_back(tmpmap);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShow){
    InitializeRenderer(hInstance);
    game = new Game();
    GIH->SetScreenCoor(vInt(1,1));
    GIH->SetSize(field_window_size);

    game->battle = new Battle(game->Maps[0]);
    MainLoop();
}

void MainLoop() {
    MSG msg;
    while(TRUE) {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if(msg.message == WM_QUIT){ break; }
        render_frame();
        // Run game code here
        // ...
        // ...
    }
}

Battle::Battle(const Map& map): map(map), ent(), curplayer(TEAM_BLUE), 
    grid(map.size), players_count(map.players_count), CurMode(CURSORMODE_NO),
    MapScreenPosition(vInt(1,1)), FieldWindowSize(field_window_size), CurSel(), 
    Spellbar(), MoveTrace(), units_finishing_lactions(), current_laction_completing(0) { 
        for (auto it = map.entities.begin(); it != map.entities.end(); ++it) {
            ent.push_back(new Entity(**it));
        }
        for (auto it = ent.begin(); it != ent.end(); ++it) {
            grid.Add(*it);
        }
        for (auto it = map.walls.begin(); it != map.walls.end(); ++it) {
            grid.SetWall((*it).coor, (*it).wall);
        }
        for (int x = 0; x < map.size.x; ++x) {
            for (int y = 0; y < map.size.y; ++y) {
                grid.SetTile(vInt(x,y), map.tiles[x][y]);
            }
        }
        RefreshScreen();
}

void Battle::CellClick(const vInt& coor, const MouseFlags flags) {
    switch(CurMode) {
    case CURSORMODE_SELECT:
        if (CLICK_LEFT & flags) {
            SetCurSel(coor);
        } else {
            if (CLICK_SHIFT & flags) {
                ClearMoveTrace();
                Turn(CurSel.ent, CurSel.coor.DefineDirection(coor));
            } else {
                AddStep(coor);
            }
        }
        break;
    case CURSORMODE_TARGETING:
        if (CurSel.ent->PerformAction(coor, grid(coor), Spellbar.ID(), 
            grid.IsSeenFrom(CurSel.ent->Coor(), coor))) {
                Spellbar.Clear();
        }
        break;
    case CURSORMODE_LTARGETING:
        Unit* unit = dynamic_cast<Unit*>(CurSel.ent);
        if (unit->CheckForValidity(coor, grid(coor), unit->PerformingAction().action, 
            grid.IsSeenFrom(CurSel.ent->Coor(), coor))) {
                unit->PerformingAction().Nullify();
                NextLAction();
        }
        break;
    };
}
void Battle::KeyClick(const Keys Key) {
    switch(Key) {
    case KEY_SPELL1:
        if (CurMode != CURSORMODE_LTARGETING)
            ChooseSpell(1);
        break;
    case KEY_SPELL2:
        if (CurMode != CURSORMODE_LTARGETING)
            ChooseSpell(2);
        break;
    case KEY_SPELL3:
        if (CurMode != CURSORMODE_LTARGETING)
            ChooseSpell(3);
        break;
    case KEY_SPELL4:
        if (CurMode != CURSORMODE_LTARGETING)
            ChooseSpell(4);
        break;
    case KEY_SPELL5:
        if (CurMode != CURSORMODE_LTARGETING)
            ChooseSpell(5);
        break;
    case KEY_ARROW_LEFT:
        MoveScreen(LEFT);
        break;
    case KEY_ARROW_RIGHT:
        MoveScreen(RIGHT);
        break;
    case KEY_ARROW_UP:
        MoveScreen(UP);
        break;
    case KEY_ARROW_DOWN:
        MoveScreen(DOWN);
        break;
    case KEY_ESC:
        switch (CurMode){
        case CURSORMODE_TARGETING:
            CurMode = CURSORMODE_SELECT;
            Spellbar.CurSpell = 0;
            break;
        case CURSORMODE_SELECT:
            ClearMoveTrace();
            break;
        case CURSORMODE_LTARGETING:
            dynamic_cast<Unit*>(CurSel.ent)->PerformingAction().Nullify();
            //continue lactions;
            break;
        };
        break;
    case KEY_ACCEPT:
        if (CurSel.ent) {
            if (CurSel.ent->Flag(ENT_IS_UNIT)) {
                ApplyUnitMove();
            }
        }
        break;
    };
}
void Battle::ChooseSpell(const int id) {
    if (CurSel.ent) {
        if (CurMode == CURSORMODE_TARGETING || CurMode == CURSORMODE_SELECT) {
            Spellbar.CurSpell = id;
            CurMode = CURSORMODE_TARGETING;
        }
    }
}
void Battle::ButtonClick(const Buttons Button) {
    switch (Button) {
    case BUTTON_ENDTURN:
        EndTurn();
        break;
    };
}

void Battle::SetCurSel(Entity *entity) {
    if (!entity) {
        CurSel.SetNull();
    } else {
        CurSel.Set(entity, entity->Coor());
        RenderSelection();
        Spellbar.Clear();
        if (CurSel.ent) {
            RenderEntityInfo(CurSel.ent);
        }
    }
        
}
void Battle::SetCurSel(const vInt& coord) {
    if (coord == vInt()) {
        CurSel.SetNull();
    } else {
        CurSel.Set(grid(coord), coord);
        RenderSelection();
        Spellbar.Clear();
        if (CurSel.ent) {
            RenderEntityInfo(CurSel.ent);
        }
    }
}

void Battle::Turn(Entity* ent, const Direction dir) {
    grid.Move(ent, ent->Coor(), dir);
}

//return true, if further automatic move is impossible
bool Battle::Move(Entity* ent, const vInt& dest, const Direction dir) {
    grid.Move(ent, dest, dir);
    bool ret = ApplyEnvironmentEffects(ent);
    if (ent->AuraRadiusDamage().value != 0) {
        EntContainer ents = grid.GetCell(dest, GridComparer_All());
        for (auto it = ents.begin(); it != ents.end(); ++it) {
            Damage(ent, *it, ent->AuraDamage());
        }
    }    
    if (ent->AuraDamage().value != 0) {
        EntContainer ents = grid.GetSquare(dest, 1, GridComparer_All());
        for (auto it = ents.begin(); it != ents.end(); ++it) {
            Damage(ent, *it, ent->AuraRadiusDamage());
        }
    }
    this->RefreshScreen();
    return ret;
}
bool Battle::Push(Entity* ent, const Direction dir) {
    vInt dest = ent->Coor() + vInt(dir);
    if (grid.BlocksMoving(ent, dest)) {
        return false;
    } else {
        Move(ent, dest);
        return true;
    }
}
void Battle::PushTo(Entity* ent, const vInt& coor) {
    auto dirs = ent->Coor().DefineDirection2(coor);
    if (!Push(ent, dirs.first) && dirs.first != dirs.second) {
        Push(ent, dirs.second);
    }
}
void Battle::PushFrom(Entity* ent, const vInt& coor) {
    auto dirs = coor.DefineDirection2(ent->Coor());
    if (!Push(ent, dirs.first) && dirs.first != dirs.second) {
        Push(ent, dirs.second);
    }
}

void Battle::Damage(Entity* source, Entity* target, const Dmg damage) {
    target->GetDamage(damage);
}

void Battle::AddStep(const vInt& coor){
    if (!CurSel.ent) { return; }
    for (auto it = MoveTrace.trace.begin(); it != MoveTrace.trace.end(); ++it) {
        if ((*it).coor == coor) {
            MoveTrace.trace.erase(it, MoveTrace.trace.end());
            return;
        }
    }

    vInt LastPos = (MoveTrace.trace.empty())? CurSel.coor : (*(MoveTrace.trace.end()-1)).coor;
    vInt dif = (coor - LastPos);
    int dist = dif.intlength(false);
    if (dist == 1) {
        if (!grid.BlocksMoving(CurSel.ent, coor)) {
            TraceStep curstep(coor, false, LastPos.DefineDirection(coor));
            MoveTrace.trace.push_back(curstep);
            RenderMoveArrow(curstep);
        }
    }else if (dist == 2 && (dif.y == 0 || dif.x == 0 || abs(dif.x) == abs(dif.y))) { //only straight lines accepted for jump
        vInt middle_cell = LastPos + dif/2;
        if (!grid.BlocksMoving(CurSel.ent, coor) && !grid.BlocksMoving(CurSel.ent, middle_cell, true) ) {
            TraceStep curstep(coor, false, LastPos.DefineDirection(coor));
            TraceStep middle_step(middle_cell, true, curstep.dir);
            MoveTrace.trace.push_back(middle_step);
            MoveTrace.trace.push_back(curstep);
            RenderMoveArrow(middle_step);
            RenderMoveArrow(curstep);
        }
    } else {
        Path path = grid.FindPath(LastPos, coor, CurSel.ent);
        for (auto it = path.begin(); it != path.end(); ++it) {
            AddStep(*it);
        }
    }
}
void Battle::ClearMoveTrace() {
    MoveTrace.trace.clear();  //TODO repaint
}
int  Battle::EvaluateMoveCost() {
    if (!CurSel.ent) { return 0; }
    int cost = 0;
    for (auto it = MoveTrace.trace.begin();
        it != MoveTrace.trace.end();
        ++it) {
            ++cost;
            if ((*it).IsJump) {
                Direction lastdir = (it == MoveTrace.trace.begin())?
                                    CurSel.ent->Dir() : (*(it-1)).dir;
                if (lastdir != (*it).dir) {
                    ++cost;
                }
            }
    }
    return cost;
}
void Battle::ApplyUnitMove() {
    if (MoveTrace.trace.empty()) { return; }
    if (!CurSel.ent ||                          //nobody selected
        !CurSel.ent->Flag(ENT_IS_UNIT) ||       //not a unit
        CurSel.ent->Team() != curplayer) {      //not your unit
            return; 
    }
    int cost = EvaluateMoveCost();
    Unit* unit = dynamic_cast<Unit*>(CurSel.ent);
    if (unit->MovePoints() < cost) { return; } //not enough movepoints
    
    unit->BlockingState() = false;

    for (auto it = MoveTrace.trace.begin();
        it != MoveTrace.trace.end();
        ++it) {
            if (Move(CurSel.ent, (*it).coor, (*it).dir)) {
                break;
            }
    }
}
void Battle::EndTurn() {
    //Current player's units finish turn
    ClearLActions();
    for (auto it = ent.begin(); it != ent.end(); ++it) {
        if ((*it)->Team() == curplayer) {
            EndTurn(*it);
        }
    }
    if (curplayer == players_count) {
        //Neutral objects complete their turn
        for (auto it = ent.begin(); it != ent.end(); ++it) {
            if ((*it)->Team() == NO_TEAM) {
                StartTurn(*it);
                EndTurn(*it);
            }
        }
        curplayer = static_cast<Team>(1);
    } else {
        curplayer = static_cast<Team>(static_cast<int>(curplayer) + 1);
    }
    //New player's units start their turn
    for (auto it = ent.begin(); it != ent.end(); ++it) {
        if ((*it)->Team() == curplayer) {
            StartTurn(*it);
        }
    }
}

bool Battle::ApplyEnvironmentEffects(Entity* ent) {
    EntContainer ents = grid.GetCell(ent->Coor(), GridComparer_EnvironmentAffecting());
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        if (ent->Flag(ENT_IS_UNIT) && (*it)->Flag(ENT_IS_OBELISK)) {
            dynamic_cast<Unit*>(ent)->DrainMana(*it);
        }
        //TODO: Check for projectiles
        if (!ent->Flag(ENT_IS_IMMORTAL) && (*it)->AuraDamage().value > 0) {
            ent->GetDamage((*it)->AuraDamage());
        }
    }
    ents = grid.GetSquare(ent->Coor(), 1, GridComparer_EnvironmentAffecting());
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        if (ent->Flag(ENT_IS_UNIT) && (*it)->Flag(ENT_IS_OBELISK)) {
            dynamic_cast<Unit*>(ent)->DrainMana(*it);
        }
        if (!ent->Flag(ENT_IS_IMMORTAL) && (*it)->AuraRadiusDamage().value > 0) {
            ent->GetDamage((*it)->AuraRadiusDamage());
        }
        if (!ent->Flag(ENT_IS_IMMORTAL) && (*it)->Flag(ENT_IS_UNIT) &&
            ent->Team() != (*it)->Team()&&
            dynamic_cast<Unit*>(*it)->IsEnchanted(ENCHANT_SHOCK)) { //enemy
                ent->GetDamage(Dmg(DMG_SHOCK, 45, DFLAG_NOFLAGS)); //FIXME: hide magic numbers
                dynamic_cast<Unit*>(*it)->EnchantTime(ENCHANT_SHOCK) = 0;
                //TODO: play proper sound
        }
    }
    return false; //TODO: rework
}
void Battle::EndTurn(Entity* ent) {
    if (ent->IsDead()) { return; }
    ApplyEnvironmentEffects(ent);
    
    if (ent->Flag(ENT_IS_UNIT)) {
        Unit* unit = dynamic_cast<Unit*>(ent);
        if (unit->IsBusy()) {
            unit->PerformingAction().time--;
            if (unit->PerformingAction().time == 0) {
                AddLAction(unit);
            }
        }
        for (int i = 0; i < Counters::continuous_spells; ++i) {
            enumContinuousSpells id = static_cast<enumContinuousSpells>(i);
            if (unit->ContinuousAction(id).Is_Cast) {
                unit->ContinuousAction(id).action->ApplyEffect(ent, unit->ContinuousAction(id).target);
            }
        }
    }
    ent->EndTurn();
}
void Battle::StartTurn(Entity* ent) {
    ApplyEnvironmentEffects(ent);
    Unit* unit = nullptr;
    if (ent->Flag(ENT_IS_UNIT)) {
        unit = dynamic_cast<Unit*>(ent);
    }
    if (unit != nullptr) {
        if (unit->IsBusy()) {
            unit->PerformingAction().time--;
            if (unit->PerformingAction().time == 0) {
                AddLAction(unit);
            }
        }
    }
    ent->StartTurn();
    if (unit != nullptr) {
        for (int i = 0; i < Counters::enchants; ++i) {
            if (unit->IsEnchanted(static_cast<enumEnchants>(i)) && game->Enchants[i].does_wearoff_auto) {
                unit->EnchantTime(static_cast<enumEnchants>(i))--;
                if (!unit->IsEnchanted(static_cast<enumEnchants>(i))) {
                    game->Enchants[i].sound_off.PlaySound(0);
                }
            }
        }
    }
}

void Battle::RenderMap() {
    for (int x = 1; x <= grid.sizex(); ++x) {
        for (int y = 1; y <= grid.sizey(); ++y) {
            RenderCell(vInt(x,y));
        }
    }
}
void Battle::RenderCell(const vInt& coor) {
    EntContainer ent = grid.GetCell(coor, GridComparer_All());
    ent.sort(EntityZOrderSorter());
    GIH->ClearCell(coor);

    //render wall and tile
    for (auto it = ent.begin(); it != ent.end(); ++it) {
        RenderEntity(*it);
    }
}
void Battle::RenderEntity(const Entity* ent, bool ClearCell) {
    vInt coor = ent->Coor();
    if (ClearCell) {
        GIH->ClearCell(coor);
    }

    const Unit* unit = nullptr;
    if (ent->Flag(ENT_IS_UNIT)) {
        unit = dynamic_cast<const Unit*>(ent);
    }
    //under unit
    if (unit != nullptr) {
        if (unit->IsEnchanted(ENCHANT_ANCHOR)) {
            GIH->SetTexture(coor, game->Enchants[ENCHANT_ANCHOR].MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_HASTE)) {
            GIH->SetTexture(coor, game->Enchants[ENCHANT_HASTE].MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_SLOW)) {
            GIH->SetTexture(coor, game->Enchants[ENCHANT_SLOW].MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_REFLECTIVE_SHIELD)) {
            if (ent->Dir() == UP_LEFT || ent->Dir() == UP_RIGHT || ent->Dir() == UP) { 
                GIH->SetTexture(coor, game->Enchants[ENCHANT_REFLECTIVE_SHIELD].MyTexture());
            }
        }
    }
    GIH->SetTexture(coor, ent->MyTexture()); //add rendering curses
    //over unit
    if (unit != nullptr) {
        if (unit->IsEnchanted(ENCHANT_STUN)) {
            GIH->SetTexture(coor, game->Enchants[ENCHANT_STUN].MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_VAMPIRISM)) {
            GIH->SetTexture(coor, game->Enchants[ENCHANT_VAMPIRISM].MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_SHOCK)) {
            GIH->SetTexture(coor, game->Enchants[ENCHANT_SHOCK].MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_PROTECTION_FIRE)) {
            GIH->SetTexture(coor, game->Enchants[ENCHANT_PROTECTION_FIRE].MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_PROTECTION_SHOCK)) {
            GIH->SetTexture(coor, game->Enchants[ENCHANT_PROTECTION_SHOCK].MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_PROTECTION_POISON)) {
            GIH->SetTexture(coor, game->Enchants[ENCHANT_PROTECTION_POISON].MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_FORCE_FIELD)) {
            GIH->SetTexture(coor, game->Enchants[ENCHANT_FORCE_FIELD].MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_PIXIES)) {
            GIH->SetTexture(coor, game->Enchants[ENCHANT_PIXIES].MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_REFLECTIVE_SHIELD)) {
            if (ent->Dir() != UP_LEFT && ent->Dir() != UP_RIGHT && ent->Dir() != UP) { 
                GIH->SetTexture(coor, game->Enchants[ENCHANT_REFLECTIVE_SHIELD].MyTexture());
            }
        }
    }
}
void Battle::RefreshScreen() {
    game->RenderInterface();
    if (CurSel.ent != nullptr) {
        RenderEntityInfo(CurSel.ent);
    }
    for (int x = MapScreenPosition.x; 
        x < min(grid.sizex(), (MapScreenPosition+FieldWindowSize).x);
        ++x) {
        for (int y = MapScreenPosition.y; 
            y < min(grid.sizey(), (MapScreenPosition+FieldWindowSize).y); 
            ++y) {
            RenderCell(vInt(x,y));
        }
    }
    RenderMoveTrace();
    RenderSelection();
}
void Battle::MoveScreen(const Direction dir) {
    if (dir != NO_DIRECTION) {
        MoveScreenTo(MapScreenPosition + vInt(dir));
    }
}
void Battle::MoveScreenTo(const vInt& coor) {
    MapScreenPosition = coor;
    //moar
}
void Battle::RenderSelection(const vInt& coor){
    GIH->SetTexture(coor, game->SelectionFrame_Texture());
}
void Battle::RenderSelection() { 
    RenderSelection(CurSel.coor); 
}
void Battle::RenderEntityInfo(const Entity* ent){}
void Battle::RenderEntityInfo(){
    RenderSpellbar();
    RenderEntityInfo(CurSel.ent);
}
void Battle::RenderSpellbar() {}
void Battle::RenderMoveTrace() {
    for (auto it = MoveTrace.trace.begin(); it != MoveTrace.trace.end(); ++it) {
        RenderMoveArrow(*it);
    }
}
void Battle::RenderMoveArrow(const TraceStep& step) {
    GIH->SetTexture(step.coor, game->MoveArrow_Texture(step.dir));
}