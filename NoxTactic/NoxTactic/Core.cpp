#include "Core.h"
#include "Entity.h"
#include "Constants.h"
#include <algorithm>
#include <math.h>
#include <Windows.h>
#include <WindowsX.h>
#pragma once
#pragma warning (disable:4244)
extern EngineCore *core;
using namespace ContainerDefs;
using namespace std;
void DefaultKeyBindings(Keys *keybinds) {
    keybinds[DIK_A] = KEY_SPELL1;
    keybinds[DIK_S] = KEY_SPELL2;
    keybinds[DIK_D] = KEY_SPELL3;
    keybinds[DIK_F] = KEY_SPELL4;
    keybinds[DIK_G] = KEY_SPELL5;
    keybinds[DIK_LEFT]  = KEY_ARROW_LEFT;
    keybinds[DIK_RIGHT] = KEY_ARROW_RIGHT;
    keybinds[DIK_UP]    = KEY_ARROW_UP;
    keybinds[DIK_DOWN]  = KEY_ARROW_DOWN;
    keybinds[DIK_RETURN] = KEY_ACCEPT;
    keybinds[DIK_ESCAPE] = KEY_ESC;
}

void MainLoop(Game* game) {
    MSG msg;
    BOOL bRet;

    while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
    { 
        if (bRet == -1)
        {
            // handle the error and possibly exit
        }
        else
        {
            TranslateMessage(&msg); 
            DispatchMessage(&msg); 
        }
        switch(msg.message) {
        case WM_CLOSE:
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_PAINT:
            game->Frame();
        };
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShow){
    core = new Game(EngineCore(hInstance, "NoxTactic", "NoxTactic", 800, 600, 100, 100));
    
    Game* game = dynamic_cast<Game*>(core);
    core->SetGraphicCore(new Graphics(game));
    game->Graphic()->Init();
    core->Init();

    Keys keybindings[KEYS];
    DefaultKeyBindings(keybindings);

    game->StartBattle(new Battle(game->DefMap(0), game));
    //entering main loop
    MainLoop(game);

    game->Shutdown();
    delete core;
    PostQuitMessage(0);
}

int round(double val) {
    double tmp = floor(val);
    if (val > tmp + 0.5) {
        return tmp + 1;
    } else {
        return tmp;
    }
}
void Game::InitEntTextures(DefaultEntity* defent) {
    std::vector<int> textures;
    if (defent->Name() != NO_STRING) {
        if (defent->Flag(ENT_IS_OMNIDIRECTIONAL)) {
            
            textures.push_back(Graphic()->AddTexture_A(AppPath() + Paths::EntitiesPictures + Strings[defent->Name()] + common_extension));
        } else {
            textures.push_back(Graphic()->AddTexture_A(AppPath() + Paths::EntitiesPictures + Strings[defent->Name()] + int_to_string(1) + common_extension));
            textures.push_back(Graphic()->AddTexture_A(AppPath() + Paths::EntitiesPictures + Strings[defent->Name()] + int_to_string(2) + common_extension));
            textures.push_back(Graphic()->AddTexture_A(AppPath() + Paths::EntitiesPictures + Strings[defent->Name()] + int_to_string(3) + common_extension));
            textures.push_back(Graphic()->AddTexture_A(AppPath() + Paths::EntitiesPictures + Strings[defent->Name()] + int_to_string(4) + common_extension));
            textures.push_back(Graphic()->AddTexture_A(AppPath() + Paths::EntitiesPictures + Strings[defent->Name()] + int_to_string(5) + common_extension));
            textures.push_back(Graphic()->AddTexture_A(AppPath() + Paths::EntitiesPictures + Strings[defent->Name()] + int_to_string(6) + common_extension));
            textures.push_back(Graphic()->AddTexture_A(AppPath() + Paths::EntitiesPictures + Strings[defent->Name()] + int_to_string(7) + common_extension));
            textures.push_back(Graphic()->AddTexture_A(AppPath() + Paths::EntitiesPictures + Strings[defent->Name()] + int_to_string(8) + common_extension));
        }
    }
    defent->SetTextures(textures);
}
void Game::InitActionTextures(Action* action) {
    action->SetIcon(Graphic()->AddTexture_A(AppPath() + Paths::ActionsIcons + String(action->Name()) + common_extension));
    action->SetActiveIcon(Graphic()->AddTexture_A(AppPath() + Paths::ActionsActiveIcons + String(action->Name()) + common_extension));
}
void Game::RefreshScreen() {
    battle->RefreshScreen();
}

void Game::StartBattle(Battle* battle) {
    this->battle = battle;
    Graphic()->SetMapSize(battle->grid.size());
    battle->RefreshScreen();
}
void Game::EndBattle() {
    
    delete battle;
}

Graphics* Game::Graphic() { return static_cast<Graphics*>(graphic); }
//::Input*    Game::Input() {return static_cast<::Input*>(input); }

void Graphics::SetMapSize(const vInt& size) { 
    SetMapSize(size.x, size.y); 
}
void Graphics::SetMapSize(int x, int y) { 
    map.resize(x); 
    for (auto it = map.begin(); it != map.end(); ++it) {
        (*it).resize(y);
    }
    size = vInt(x,y);
}
void Graphics::PushTexture(const vInt& coor, int texture) {
    map[coor.x][coor.y].push_back(texture);
}
void Graphics::ClearCell(const vInt& coor) {
    map[coor.x-1][coor.y-1].clear();
}

void Graphics::AddPicture(Picture pic) { 
    pics.push_back(pic); 
}
void Graphics::AddLabel(Label label) { 
    labels.push_back(label); 
}
void Graphics::AddButton(Button button) { 
    buttons.push_back(button); 
}
void Graphics::SetSpellbar(SpellBar bar) { 
    spellbar = bar; 
}

void Graphics::Paint(const vInt& coor1, const vInt& coor2) {
    ClearDisplay();
    BeginSprite();
    for (int x = coor1.x - 1; x < min(size.x, coor2.x) - 1; ++x) {
        for (int y = coor1.y - 1; y < min(size.y, coor2.y) - 1; ++y) {
            for (unsigned int i = 0; i < map[x][y].size(); ++i) {
                vInt tmp = (vInt(x+1, y+1) - coor1) * picsize;
                PaintSprite(map[x][y][i], D3DXVECTOR2(tmp.x, tmp.y));
            }
        }
    }
    EndSprite();
}
/*
Input::Input(FieldInfo field): field(field), buttons() {}
void Input::AddButton(ButtonField button) {
    buttons.push_back(button);
}

void Input::SetKeybindings(Keys keybinds[KEYS]) {
    for (int i = 0; i < KEYS; ++i) {
        this->keybinds[i] = keybinds[i];
    }
}

InputReturnMouse Input::CheckMouse(){
    DIMOUSESTATE MouseState;
    ReadMouse((void*)&MouseState);

    cursor_coor.x += MouseState.lX;
    cursor_coor.y += MouseState.lY;
    
    if (IsKeyPressed(MouseState.rgbButtons, 0)) {
        mouseflags = mouseflags | CLICK_LEFT;
    } else {
        mouseflags = mouseflags & (!CLICK_LEFT);
    }
    if (IsKeyPressed(MouseState.rgbButtons, 1)) {
        mouseflags = mouseflags | CLICK_RIGHT;
    } else {
        mouseflags = mouseflags & (!CLICK_RIGHT);
    }
    if (!(mouseflags & (CLICK_RIGHT | CLICK_LEFT))) {
        return InputReturnMouse(vInt(0,0), mouseflags);
    }

    vLng cell_coor = (cursor_coor - field.startpos) / (field.cellsize + vLng(1,1)*field.cell_indent);
    if (cell_coor.IsInside(vLng(1,1), field.windowsize)) {
        return InputReturnMouse(vInt(static_cast<int>(cell_coor.x), static_cast<int>(cell_coor.y)), mouseflags);
    } else {
        return InputReturnMouse(vInt(0,0), mouseflags);
    }
}
InputReturnKey Input::CheckKeyboard(){
    ReadKeyboard(keyboard_state);
    if (IsKeyPressed(keyboard_state, DIK_LSHIFT) ||
        IsKeyPressed(keyboard_state, DIK_RSHIFT)) {
            mouseflags = mouseflags | CLICK_SHIFT;
    } else {
        mouseflags = mouseflags & (!CLICK_SHIFT);
    }
    if (IsKeyPressed(keyboard_state, DIK_LMENU) ||
        IsKeyPressed(keyboard_state, DIK_RMENU)) {
            mouseflags = mouseflags | CLICK_ALT;
    } else {
        mouseflags = mouseflags & (!CLICK_ALT);
    }
    if (IsKeyPressed(keyboard_state, DIK_LCONTROL) ||
        IsKeyPressed(keyboard_state, DIK_RCONTROL)) {
            mouseflags = mouseflags | CLICK_CTRL;
    } else {
        mouseflags = mouseflags & (!CLICK_CTRL);
    }

    for (int i = 0; i < KEYS; ++i) {
        if (IsKeyPressed(keyboard_state, i)) {
            return InputReturnKey(keybinds[i]);
        }
    }
    return InputReturnKey(KEY_NONE);
}
InputReturnButton Input::CheckButtons() {
    DIMOUSESTATE MouseState;
    ReadMouse((void*)&MouseState);
    
    cursor_coor.x += MouseState.lX;
    cursor_coor.y += MouseState.lY;
    if (IsKeyPressed(MouseState.rgbButtons, 0)) {
        for (auto it = buttons.begin(); it != buttons.end(); ++it) {
            if (cursor_coor.IsInside((*it).startpos, (*it).endpos)) {
                return InputReturnButton((*it).ID);
            }
        }
    }
    return InputReturnButton(NO_BUTTON);
}
*/
#undef PlaySound
Game::Game(EngineCore engine): EngineCore(engine) {}
template <class Container>
void FreeContainer(Container& container) {
   for (Container::iterator it = container.begin(); it != container.end(); ++it) {
       delete (*it);
   }
}

void Game::Init(){
    Selection_Texture = Graphic()->AddTexture(AppPath() + Paths::SelectionFrame);
    for (char i = 0; i < Counters::directions; ++i) {
        MoveArrows_Texture[i] = Graphic()->AddTexture(AppPath() + Paths::MoveArrow + int_to_string(i + 1) + common_extension);
    }

    FunctionContainer actions;
    ContinuousFunctionContainer continuous_actions;
    actions.resize(Counters::actions);
    continuous_actions.resize(Counters::continuous_spells);
    actions.push_back(nullptr); //NO_ACTION
    // fill the funcs arrays

    Loader::loadWalls(Walls, AppPath(), Strings);
    Loader::loadTiles(Tiles, AppPath(), Strings);
    Loader::loadStrings(Strings);
    Loader::loadSounds(Sounds);
    Loader::loadGestures(Gestures, Sounds);
    Loader::loadEnchants(Enchants, Sounds);
    Loader::loadActions(Actions, Sounds, actions, continuous_actions);
    Loader::loadWeapons(Weapons, Actions);
    Loader::loadDefaultEntities(Entities, Actions, Sounds, Weapons);
    Loader::loadMaps(Maps, Entities, Walls, Tiles);
    for (unsigned int i = 0; i < Entities.size(); ++i) {
        InitEntTextures(Entities[i]);
    }
    for (int i = 0; i < Counters::actions; ++i) {
        InitActionTextures(Actions[i]);
    }
}
void Game::Shutdown(){
    Graphic()->Shutdown();
    delete graphic;
    input->Shutdown();
    delete input;
    FreeContainer(Sounds);
    FreeContainer(Gestures);
    FreeContainer(Enchants);
    FreeContainer(Actions);
    FreeContainer(Weapons);
    FreeContainer(Maps);
    FreeContainer(Walls);
    FreeContainer(Tiles);
}
void Game::Frame() {
    CheckInput();
    RefreshScreen();
}

int  Game::MoveArrow_Texture (Direction dir) {
    if (dir == NO_DIRECTION) { 
        return 0;
    } else {
        return MoveArrows_Texture[static_cast<int>(dir) - 1];
    }
}
int  Game::SelectionFrame_Texture () {
    return Selection_Texture;
}

void Game::CellClick(const vInt& coor, const long flags) {
    if (battle) { battle->CellClick(coor, flags); }
}
void Game::KeyClick(const Keys key) {
    if (battle) { battle->KeyClick(key); }
}
void Game::ButtonClick(const Buttons button) {
    if (battle) { battle->ButtonClick(button); }
}
void Game::CheckInput() {

    /*::Input* input = this->Input();
    auto checkkeyboard = input->CheckKeyboard();
    if (checkkeyboard.ID != KEY_NONE) {
        KeyClick(checkkeyboard.ID);
    }
    auto checkbuttons = input->CheckButtons();
    if (checkbuttons.ID != NO_BUTTON) {
        ButtonClick(checkbuttons.ID);
    }
    
    auto checkmouse = input->CheckMouse();
    if (checkmouse.flags & CLICK_LEFT) {
        CellClick(checkmouse.coor, checkmouse.flags);
    }*/
}

Map::~Map() {
    for_each(entities.begin(), entities.end(), [](Entity *ent){
        delete ent;
    });
}

void Loader::loadMaps(MapContainer& container, EntityContainer& entities, WallContainer& walls, TileContainer& tiles){
    vector<Entity*> tmpent;
    vector<vector<Tile*>> tmptiles(10, vector<Tile*>(10, tiles[NO_TILE]));
    tmpent.reserve(6);
    tmpent.push_back(new Unit(*dynamic_cast<DefaultUnit*>(entities[ENT_WIZ_RED]), vInt(2,2), TEAM_BLUE, RIGHT));
    tmpent.push_back(new Unit(*dynamic_cast<DefaultUnit*>(entities[ENT_WIZ_BLUE]), vInt(2,3), TEAM_BLUE, RIGHT));
    tmpent.push_back(new Unit(*dynamic_cast<DefaultUnit*>(entities[ENT_WIZ_YELLOW]), vInt(2,4), TEAM_BLUE, RIGHT));
    tmpent.push_back(new Unit(*dynamic_cast<DefaultUnit*>(entities[ENT_WIZ_RED]), vInt(6,2), TEAM_RED, LEFT));
    tmpent.push_back(new Unit(*dynamic_cast<DefaultUnit*>(entities[ENT_WIZ_BLUE]), vInt(6,3), TEAM_RED, LEFT));
    tmpent.push_back(new Unit(*dynamic_cast<DefaultUnit*>(entities[ENT_WIZ_YELLOW]), vInt(6,4), TEAM_RED, LEFT));
    Map* tmpmap = new Map(vInt(10,10), tmpent, vector<WallCoor>(), tmptiles, 2);
    container.push_back(tmpmap);
}

/*void MainLoop() {
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
}*/
Battle::Battle(const Map* map, Game* game): game(game), map(map), ent(), curplayer(TEAM_BLUE), 
    grid(map->size), players_count(map->players_count), CurMode(CURSORMODE_NO),
    MapScreenPosition(vInt(1,1)), FieldWindowSize(field_window_size), CurSel(), 
    Spellbar(), MoveTrace(), LongActions() { 
        for (auto it = map->entities.begin(); it != map->entities.end(); ++it) {
            ent.push_back(new Entity(**it));
        }
        for (auto it = ent.begin(); it != ent.end(); ++it) {
            grid.Add(*it);
        }
        for (auto it = map->walls.begin(); it != map->walls.end(); ++it) {
            grid.SetWall((*it).coor, (*it).wall);
        }
        for (int x = 0; x < map->size.x; ++x) {
            for (int y = 0; y < map->size.y; ++y) {
                grid.SetTile(vInt(x,y), map->tiles[x][y]);
            }
        }
}

void Battle::CellClick(const vInt& coor, const long flags) {
    Unit* unit = dynamic_cast<Unit*>(CurSel.ent);
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
        if (unit->Spellbar(Spellbar.ID())->Perform(CurSel.ent, coor, grid)) {
                Spellbar.Clear();
        }
        break;
    case CURSORMODE_LTARGETING:
        if (unit->PerformingAction().action->CheckForValidity(CurSel.ent, coor, grid)) {
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

void Battle::LaunchProjectile(const DefaultProjectile& projectile_type, Entity* source, const vInt& dest) {}
void Battle::LaunchHomingProjectile(const DefaultProjectile& projectile_type, Entity* source, Entity* target) {}
void Battle::MoveProjectile(Entity* projectile, bool IsFirstMove) {
    if (projectile->Flag(ENT_IS_PROJECTILE) && !projectile->Flag(ENT_IS_HOMING_PROJECTILE)) {
        CommonProjectile* pr = dynamic_cast<CommonProjectile*>(projectile);
        for (int i = 0; i < pr->Speed(); ++i) {
            vDbl excoor;
            vInt coor;
            
            excoor = pr->ExCoor() + pr->Angle().ProjectileIncrement();
            coor = vInt(round(excoor.x), round(excoor.y));

            //reflection part

            if (grid.BlocksMoving(projectile, coor)) {
                pr->Collision(grid(coor));
                //destroy projectile?
            }
            Move(projectile, coor);
            pr->ExCoor() = excoor;
        }


    } else if (projectile->Flag(ENT_IS_HOMING_PROJECTILE)) {

    }
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
            if (unit->IsEnchanted(static_cast<enumEnchants>(i)) && game->DefEnchant(static_cast<enumEnchants>(i))->does_wearoff_auto) {
                unit->EnchantTime(static_cast<enumEnchants>(i))--;
                if (!unit->IsEnchanted(static_cast<enumEnchants>(i))) {
                    game->DefEnchant(static_cast<enumEnchants>(i))->sound_off->PlaySound(0);
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
    game->Graphic()->ClearCell(coor);

    //render wall and tile
    for (auto it = ent.begin(); it != ent.end(); ++it) {
        RenderEntity(*it);
    }
}
void Battle::RenderEntity(const Entity* ent, bool ClearCell) {
    vInt coor = ent->Coor();
    if (ClearCell) {
        game->Graphic()->ClearCell(coor);
    }

    const Unit* unit = nullptr;
    if (ent->Flag(ENT_IS_UNIT)) {
        unit = dynamic_cast<const Unit*>(ent);
    }
    //under unit
    if (unit != nullptr) {
        if (unit->IsEnchanted(ENCHANT_ANCHOR)) {
            game->Graphic()->PushTexture(coor, game->DefEnchant(ENCHANT_ANCHOR)->MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_HASTE)) {
            game->Graphic()->PushTexture(coor, game->DefEnchant(ENCHANT_HASTE)->MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_SLOW)) {
            game->Graphic()->PushTexture(coor, game->DefEnchant(ENCHANT_SLOW)->MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_REFLECTIVE_SHIELD)) {
            if (ent->Dir() == UP_LEFT || ent->Dir() == UP_RIGHT || ent->Dir() == UP) { 
                game->Graphic()->PushTexture(coor, game->DefEnchant(ENCHANT_REFLECTIVE_SHIELD)->MyTexture());
            }
        }
    }
    game->Graphic()->PushTexture(coor, ent->MyTexture()); //add rendering curses
    //over unit
    if (unit != nullptr) {
        if (unit->IsEnchanted(ENCHANT_STUN)) {
            game->Graphic()->PushTexture(coor, game->DefEnchant(ENCHANT_STUN)->MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_VAMPIRISM)) {
            game->Graphic()->PushTexture(coor, game->DefEnchant(ENCHANT_VAMPIRISM)->MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_SHOCK)) {
            game->Graphic()->PushTexture(coor, game->DefEnchant(ENCHANT_SHOCK)->MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_PROTECTION_FIRE)) {
            game->Graphic()->PushTexture(coor, game->DefEnchant(ENCHANT_PROTECTION_FIRE)->MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_PROTECTION_SHOCK)) {
            game->Graphic()->PushTexture(coor, game->DefEnchant(ENCHANT_PROTECTION_SHOCK)->MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_PROTECTION_POISON)) {
            game->Graphic()->PushTexture(coor, game->DefEnchant(ENCHANT_PROTECTION_POISON)->MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_FORCE_FIELD)) {
            game->Graphic()->PushTexture(coor, game->DefEnchant(ENCHANT_FORCE_FIELD)->MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_PIXIES)) {
            game->Graphic()->PushTexture(coor, game->DefEnchant(ENCHANT_PIXIES)->MyTexture()); 
        }
        if (unit->IsEnchanted(ENCHANT_REFLECTIVE_SHIELD)) {
            if (ent->Dir() != UP_LEFT && ent->Dir() != UP_RIGHT && ent->Dir() != UP) { 
                game->Graphic()->PushTexture(coor, game->DefEnchant(ENCHANT_REFLECTIVE_SHIELD)->MyTexture());
            }
        }
    }
}
void Battle::RefreshScreen() {
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

    game->Graphic()->Paint(MapScreenPosition, MapScreenPosition + FieldWindowSize);
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
    game->Graphic()->PushTexture(coor, game->SelectionFrame_Texture());
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
    game->Graphic()->PushTexture(step.coor, game->MoveArrow_Texture(step.dir));
}

Action::Action(void (*action)(Entity*, const vInt&, const Action*),
    int range, int damage1, int damage2, int timevalue, int performtime,
    Sound* sound, enumStrings name, enumStrings msg_done, enumStrings msg_started, 
    enumStrings msg_stopped, enumStrings msg_affected, enumStrings msg_killed, long flags):
               action(action), range(range), damage1(damage1), damage2(damage2), time_value(timevalue),
               time_to_perform(performtime), sound(sound), name(name), msg_performed(msg_done),
               msg_started(msg_started), msg_stopped(msg_stopped), msg_affected(msg_affected),
               msg_killed(msg_killed), flags(flags){}
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
bool ContinuousSpell::Perform(Entity* actor, const vInt& target, const Grid& grid) const {
    action(actor, target, this);
    return true;
}
bool ContinuousSpell::CheckForValidity(Entity* actor, const vInt& target, const Grid& grid) const {
    action(actor, target, this);
    return true;
}
bool Action::Perform(Entity* actor, const vInt& target, const Grid& grid) const {
    action(actor, target, this);
    return true;
}
bool Action::CheckForValidity(Entity* actor, const vInt& target, const Grid& grid) const {
    action(actor, target, this);
    return true;
}
bool Spell::Perform(Entity* actor, const vInt& target, const Grid& grid) const {
    action(actor, target, this);
    return true;
}
bool Spell::CheckForValidity(Entity* actor, const vInt& target, const Grid& grid) const {
    action(actor, target, this);
    return true;
}
bool Ability::Perform(Entity* actor, const vInt& target, const Grid& grid) const {
    action(actor, target, this);
    return true;
}
bool Ability::CheckForValidity(Entity* actor, const vInt& target, const Grid& grid) const {
    action(actor, target, this);
    return true;
}
bool EnchantSpell::Perform(Entity* actor, const vInt& target, const Grid& grid) const {
    return this->Spell::Perform(actor, target, grid);
}
bool EnchantSpell::CheckForValidity(Entity* actor, const vInt& target, const Grid& grid) const {
    return this->Spell::CheckForValidity(actor, target, grid);
}
//FIX ACTIONS

Gesture::Gesture(const Sound* sound, const int number): 
sound(sound), powernumber(number){}
void Gesture::CastMe(const int volume) const {
	sound->PlaySound(volume);
}

void Loader::loadActions(ActionContainer& container, SoundContainer& sounds, 
    FunctionContainer& functions, ContinuousFunctionContainer& continuous_functions) {
    container.reserve(Counters::actions);
    FunctionContainer::iterator iter = functions.begin();
    ContinuousFunctionContainer::iterator continuous_iter = continuous_functions.begin();

    container.push_back(new Action(*(iter++), 0, 0, 0, 0, 0, sounds[NO_SOUND], NO_STRING,
        NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 0));
    
    container.push_back(new Spell(Action(*(iter++), 0, 127, 0, 4, 0, sounds[SND_FireballMove], 
        STR_ACTION_FIREBALL, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_CANT_TARGET_SELF + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TURN_TO_TARGET),
        30, G_ZO, G_ZO, G_UN, G_, G_, G_, G_, G_));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantMove],
        STR_ACTION_ANCHOR, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE),
        10, G_ZO, G_ZO, G_, G_, G_, G_, G_, G_), ENCHANT_ANCHOR));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 30, 0, 10, 0, sounds[SND_BurnCast],
        STR_ACTION_BURN, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING,NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_MUST_TARGET_VISIBLE),
        10, G_ZO, G_ZO, G_UN, G_UN, G_, G_, G_, G_));
    container.push_back(new ContinuousSpell(Spell(Action(*(iter++), 0, 40, 40, 0, 0, sounds[SND_GHealOff],
        STR_ACTION_CHANNEL, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS + AFLAG_IS_CONTINUOUS),
        0, G_KA, G_ZO, G_IN, G_, G_, G_, G_, G_), CONT_CHANNEL_LIFE, *(continuous_iter++)));
    container.push_back(new Spell(Action(*(iter++), visibility_range / 2, 0, 0, 0, 0, sounds[SND_CounterSpellCast],
        STR_ACTION_COUNTERSPELL, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS + AFLAG_IS_AGGRESSIVE),
        20, G_ZO, G_DO, G_, G_, G_, G_, G_, G_));
    container.push_back(new Spell(Action(*(iter++), 2, 200, 0, 3, 2, sounds[SND_DispellUndeadCast],
        STR_ACTION_DISPELL_UNDEAD, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS + AFLAG_IS_AGGRESSIVE),
        60, G_UN, G_KA, G_ET, G_RO, G_ZO, G_DO, G_CHA, G_IN));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 100, 0, 0, 0, sounds[SND_DeathRayCast],
        STR_ACTION_DEATHRAY, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_VISIBLE + AFLAG_IS_AGGRESSIVE + AFLAG_CANT_TARGET_SELF + AFLAG_MUST_TURN_TO_TARGET),
        60, G_DO, G_DO, G_, G_, G_, G_, G_, G_));
    container.push_back(new ContinuousSpell(Spell(Action(*(iter++), visibility_range/2 + 1, 40, 40, 0, 0, sounds[SND_EboltCast],
        STR_ACTION_ENERGY_BOLT, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_IS_CONTINUOUS + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_TARGET_SELF),
        0, G_ZO, G_CHA, G_ET, G_UN, G_, G_, G_, G_), CONT_ENERGY_BOLT, *(continuous_iter++)));
    container.push_back(new Spell(Action(*(iter++), visibility_range/2 + 1, 40, 8, 0, 2, sounds[SND_EarthquakeCast],
        STR_ACTION_EARTHQUAKE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS + AFLAG_IS_AGGRESSIVE),
        60, G_DO, G_ET, G_IN, G_RO, G_CHA, G_KA, G_, G_));
    container.push_back(new ContinuousSpell(Spell(Action(*(iter++), visibility_range/2 + 1, 40, 30, 0, 2, sounds[SND_LightningCast],
        STR_ACTION_LIGHTNING, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_IS_CONTINUOUS + AFLAG_CANT_TARGET_SELF + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET),
        0, G_ZO, G_CHA, G_ET, G_UN, G_ET, G_CHA, G_ZO, G_), CONT_LIGHTNING, *(continuous_iter++)));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        STR_ACTION_PROTECTION_FIRE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET),
        30, G_ET, G_CHA, G_DO, G_KA, G_, G_, G_, G_), ENCHANT_PROTECTION_FIRE));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        STR_ACTION_PROTECTION_SHOCK, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET),
        30, G_CHA, G_ET, G_DO, G_KA, G_, G_, G_, G_), ENCHANT_PROTECTION_SHOCK));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        STR_ACTION_PROTECTION_VENOM, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET),
        30, G_ET, G_CHA, G_RO, G_IN, G_, G_, G_, G_), ENCHANT_PROTECTION_POISON));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 50, 2, sounds[SND_EnchantCast],
        STR_ACTION_FORCEFIELD, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET),
        60, G_UN, G_ET, G_ZO, G_CHA, G_UN, G_ET, G_ZO, G_CHA), ENCHANT_FORCE_FIELD));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 15, 0, 0, 0, sounds[SND_EnchantCast],
        STR_ACTION_HEAL_LESSER, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET),
        30, G_DO, G_UN, G_RO, G_, G_, G_, G_, G_));
    container.push_back(new ContinuousSpell(Spell(Action(*(iter++), visibility_range/2 + 1, 40, 40, 0, 0, sounds[SND_GHealOn],
        STR_ACTION_HEAL_GREATER, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_UNIT + AFLAG_IS_CONTINUOUS + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET),
        0, G_RO, G_UN, G_DO, G_, G_, G_, G_, G_), CONT_GREATER_HEAL, *(continuous_iter++)));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        STR_ACTION_HASTE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET),
        10, G_ET, G_CHA, G_CHA, G_, G_, G_, G_, G_), ENCHANT_HASTE));
    container.push_back(new Spell(Action(*(iter++), visibility_range/2, 0, 0, 0, 0, sounds[SND_Inversion],
        STR_ACTION_INVERSION, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS),
        10, G_KA, G_IN, G_, G_, G_, G_, G_, G_));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 40, 0, 4, 0, sounds[SND_MissilesMove],
        STR_ACTION_MAGIC_MISSILE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_TARGET_SELF),
        20, G_ET, G_UN, G_CHA, G_UN, G_, G_, G_, G_));
    container.push_back(new ContinuousSpell(Spell(Action(*(iter++), visibility_range/2 + 1, 40, 0, 0, 0, sounds[SND_ManaDrainCast],
        STR_ACTION_MANA_DRAIN, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS + AFLAG_IS_CONTINUOUS),
        0, G_UN, G_KA, G_ZO, G_IN, G_, G_, G_, G_), CONT_DRAIN_MANA, *(continuous_iter++)));
    container.push_back(new Spell(Action(*(iter++), visibility_range/2, 0, 0, 0, 0, sounds[SND_Pull],
        STR_ACTION_PULL, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS),
        10, G_UN, G_UN, G_ZO, G_, G_, G_, G_, G_));
    container.push_back(new Spell(Action(*(iter++), visibility_range/2, 0, 0, 0, 0, sounds[SND_Push],
        STR_ACTION_PUSH, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS),
        10, G_UN, G_UN, G_UN, G_, G_, G_, G_, G_));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), 0, 0, 0, 0, 0, sounds[SND_RSOn],
        STR_ACTION_REFLECTIVE_SHIELD, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_IS_TARGETLESS),
        30, G_UN, G_ZO, G_ZO, G_RO, G_DO, G_, G_, G_), ENCHANT_REFLECTIVE_SHIELD));
    container.push_back(new Spell(Action(*(iter++), 2, 30, 0, 3, 0, sounds[SND_RingOfFireCast],
        STR_ACTION_FIRE_RING, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_IS_TARGETLESS),
        60, G_DO, G_ZO, G_RO, G_UN, G_, G_, G_, G_));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 45, 0, 0, 0, sounds[SND_EnchantCast],
        STR_ACTION_SHOCK, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET),
        30, G_ZO, G_CHA, G_ET, G_ET, G_, G_, G_, G_), ENCHANT_SHOCK));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        STR_ACTION_SLOW, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET),
        15, G_ZO, G_ZO, G_ZO, G_, G_, G_, G_, G_), ENCHANT_SLOW));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        STR_ACTION_SWAP, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_BE_ANCHORED + AFLAG_CANT_TARGET_SELF),
        15, G_UN, G_UN, G_ZO, G_ZO, G_, G_, G_, G_));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_Teleport],
        STR_ACTION_TELEPORT, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_MUST_TARGET_EMPTY + AFLAG_CANT_BE_ANCHORED),
        20, G_ZO, G_UN, G_ET, G_CHA, G_, G_, G_, G_));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 255, 0, 0, 0, sounds[SND_WallOn],
        STR_ACTION_WALL, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_EMPTY + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET),
        30, G_KA, G_UN, G_IN, G_, G_, G_, G_, G_));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 250, 0, 1, 0, sounds[SND_FistCast],
        STR_ACTION_FIST_OF_VENGEANCE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_AGGRESSIVE),
        60, G_KA, G_IN, G_UN, G_ZO, G_, G_, G_, G_));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        STR_ACTION_VAMPIRISM, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET),
        30, G_UN, G_ZO, G_ET, G_CHA, G_, G_, G_, G_), ENCHANT_VAMPIRISM));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        STR_ACTION_STUN, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_AGGRESSIVE),
        10, G_KA, G_ZO, G_, G_, G_, G_, G_, G_), ENCHANT_STUN));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_ToxicCloudCast],
        STR_ACTION_TOXIC_CLOUD, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_AGGRESSIVE),
        60, G_IN, G_RO, G_KA, G_, G_, G_, G_, G_));
    container.push_back(new Spell(Action(*(iter++), 0, 800, 30, 3, 2, sounds[SND_ForceOfNatureCast],
        STR_ACTION_FORCE_OF_NATURE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_AGGRESSIVE + AFLAG_CANT_TARGET_SELF),
        60, G_DO, G_RO, G_ZO, G_, G_, G_, G_, G_));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), 0, 40, 0, 0, 0, sounds[SND_PixiesCast],
        STR_ACTION_PIXIES, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_IS_AGGRESSIVE + AFLAG_IS_TARGETLESS),
        30, G_ET, G_ZO, G_CHA, G_ZO, G_, G_, G_, G_), ENCHANT_PIXIES));
    container.push_back(new ContinuousSpell(Spell(Action(*(iter++), visibility_range/2 + 1, 0, 0, 0, 0, sounds[SND_CharmCast],
        STR_ACTION_CHARM, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_IS_CONTINUOUS + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_TARGET_SELF),
        10, G_KA, G_IN, G_ZO, G_, G_, G_, G_, G_), CONT_CHARM, *(continuous_iter++)));
    container.push_back(new Spell(Action(*(iter++), visibility_range, 60, 0, 0, 0, sounds[SND_MeteorCast],
        STR_ACTION_METEOR, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_AGGRESSIVE),
        30, G_RO, G_RO, G_, G_, G_, G_, G_, G_));
    container.push_back(new EnchantSpell(Spell(Action(*(iter++), visibility_range, 0, 0, 0, 0, sounds[SND_EnchantCast],
        STR_ACTION_POISON, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_TARGET_SELF),
        10, G_IN, G_RO, G_, G_, G_, G_, G_, G_), ENCHANT_POISON));
    container.push_back(new ContinuousSpell(Spell(Action(*(iter++), visibility_range, 50, 4, 0, 2, sounds[NO_SOUND],
        STR_ACTION_OBLITERATION, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 
        AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_IS_CONTINUOUS + AFLAG_IS_TARGETLESS),
        10, G_UN, G_UN, G_ET, G_ET, G_ZO, G_ZO, G_CHA, G_CHA), CONT_OBLITERATION, *(continuous_iter++)));
    container.push_back(new Ability(Action(*(iter++), visibility_range, 150, 20, 0, 0, sounds[SND_Berserker],
        STR_ACTION_BERSERKER, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_ABILITY + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_TARGET_SELF),
        5, CD_BERSERKER));
    container.push_back(new Ability(Action(*(iter++), visibility_range/2, 0, 0, 0, 0, sounds[SND_HarpoonReleased],
        STR_ACTION_HARPOON, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_ABILITY + AFLAG_IS_AGGRESSIVE + AFLAG_CANT_TARGET_SELF + AFLAG_MUST_TURN_TO_TARGET),
        4, CD_HARPOON));
    container.push_back(new Ability(Action(*(iter++), visibility_range/2 + 1, 0, 0, 0, 0, sounds[SND_WarCry],
        STR_ACTION_WARCRY, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_ABILITY + AFLAG_IS_AGGRESSIVE + AFLAG_IS_TARGETLESS),
        5, CD_WARCRY));
    container.push_back(new Action(*(iter++), 1, 130, 0, 0, 2, sounds[SND_HammerStrike],
        STR_ACTION_HAMMER, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_IS_MELEE));
    container.push_back(new Action(*(iter++), 1, 80, 0, 0, 0, sounds[SND_LongSwordStrike],
        STR_ACTION_LONGSWORD, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_IS_MELEE));
    container.push_back(new Action(*(iter++), 1, 40, 0, 0, 0, sounds[NO_SOUND],
        STR_ACTION_MACE, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_IS_MELEE));
    container.push_back(new Action(*(iter++), 0, 64, 0, 4, 0, sounds[SND_BowShot],
        STR_ACTION_BOW, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO));
    container.push_back(new Action(*(iter++), visibility_range/2 + 1, 40, 30, 0, 0 ,sounds[SND_LightningCast],
        STR_ACTION_FORCE_STAFF, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO+AFLAG_MUST_TARGET_VISIBLE+AFLAG_MUST_TARGET_UNIT));
    container.push_back(new Action(*(iter++), 0, 64, 0, 4, 0, sounds[SND_FireStaffShot],
        STR_ACTION_FIRE_STAFF, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO));
    container.push_back(new Action(*(iter++), 0, 64, 0, 4, 0, sounds[SND_TripleStaffShot],
        STR_ACTION_HELLFIRE_STAFF, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO));
    container.push_back(new Action(*(iter++), 0, 40, 0, 4, 0, sounds[SND_ShurikenThrow],
        STR_ACTION_SHURIKEN, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO+AFLAG_IS_QUICK));
    container.push_back(new Action(*(iter++), 1, 40, 8, 0, 0, sounds[SND_LongSwordStrike],
        STR_ACTION_FIRESWORD, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_IS_MELEE+AFLAG_IS_QUICK));
    container.push_back(new Action(*(iter++), 0, 0, 0, 0, 2, sounds[SND_ForceOfNatureCast],
        STR_ACTION_FON_STAFF, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO));
}
void Loader::loadGestures(GestureContainer& container, SoundContainer& sounds) {
    container.reserve(Counters::gestures);
    container.push_back(new Gesture(sounds[NO_SOUND],0));
    container.push_back(new Gesture(sounds[SND_GESTURE_UN],1));
    container.push_back(new Gesture(sounds[SND_GESTURE_KA],2));
    container.push_back(new Gesture(sounds[SND_GESTURE_ET],3));
    container.push_back(new Gesture(sounds[SND_GESTURE_RO],4));
    container.push_back(new Gesture(sounds[SND_GESTURE_ZO],5));
    container.push_back(new Gesture(sounds[SND_GESTURE_DO],6));
    container.push_back(new Gesture(sounds[SND_GESTURE_CHA],7));
    container.push_back(new Gesture(sounds[SND_GESTURE_IN],8));
}
void Loader::loadEnchants(EnchantContainer& container, SoundContainer& sounds) {
    container.reserve(Counters::enchants);
    container.push_back(new Enchant(STR_ENCHANT_ANCHOR,            sounds[SND_AnchorOn]     , sounds[SND_AnchorOff]    , 1, 8));
    container.push_back(new Enchant(STR_ENCHANT_FORCE_FIELD,       sounds[SND_ForceFieldOn] , sounds[SND_ForceFieldOff], 2, 50, false));
    container.push_back(new Enchant(STR_ENCHANT_HASTE,             sounds[SND_HasteOn]      , sounds[SND_HasteOff]     , 3, 4));
    container.push_back(new Enchant(STR_ENCHANT_PIXIES,            sounds[NO_SOUND]         , sounds[NO_SOUND]         , 4, 0, false));
    container.push_back(new Enchant(STR_ENCHANT_POISON,            sounds[SND_PoisonOn]     , sounds[SND_PoisonOff]    , 5, 8));
    container.push_back(new Enchant(STR_ENCHANT_PROTECTION_FIRE,   sounds[SND_ProtectionOn] , sounds[SND_ProtectionOff], 6, 6));
    container.push_back(new Enchant(STR_ENCHANT_PROTECTION_POISON, sounds[SND_ProtectionOn] , sounds[SND_ProtectionOff], 7, 6));
    container.push_back(new Enchant(STR_ENCHANT_PROTECTION_SHOCK,  sounds[SND_ProtectionOn] , sounds[SND_ProtectionOff], 8, 6));
    container.push_back(new Enchant(STR_ENCHANT_REFLECTIVE_SHIELD, sounds[SND_RSOn]         , sounds[SND_RSOff]        , 9, 0, false));
    container.push_back(new Enchant(STR_ENCHANT_SLOW,              sounds[SND_SlowOn]       , sounds[SND_SlowOff]      , 10, 3));
    container.push_back(new Enchant(STR_ENCHANT_STUN,              sounds[SND_StunOn]       , sounds[SND_StunOff]      , 11, 1));
    container.push_back(new Enchant(STR_ENCHANT_VAMPIRISM,         sounds[SND_VampirismOn]  , sounds[SND_VampirismOff] , 12, 6));
    container.push_back(new Enchant(STR_ENCHANT_SHOCK,             sounds[SND_ShockOn]      , sounds[SND_ShockOff]     , 13, 6));
    container.push_back(new Enchant(STR_ENCHANT_NULL,              sounds[SND_NullOn]       , sounds[SND_NullOff]      , 14, 3));
}