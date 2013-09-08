#pragma once
//using namespace std;
#include <vector>
#include "Vector.h"
#include "Sound.h"
#include "Constants.h"
#include "Strings.h"
#include "Action.h"
#include "Entity.h"
#include "Grid.h"
struct Game;
extern Game *game;
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

struct WallCoor
{
    vInt coor;
    Wall wall;
    WallCoor(const vInt& coor, Wall wall): coor(coor), wall(wall){}
};
class Map
{
    vInt size;
    vector<Entity *> entities;
    vector<WallCoor> walls;
    vector<vector<Tile>> tiles;
    int players_count;
public:
    Map(const vInt& size, const vector<Entity *>& entities, vector<WallCoor> walls, vector<vector<Tile>> tiles, const int players_count): 
      size(size), entities(entities), walls(walls), tiles(tiles), players_count(players_count) {}
    void Destroy();
    friend class Battle;
};
enum CursorMode
{
    CURSORMODE_NO,
    CURSORMODE_SELECT,
    CURSORMODE_TARGETING,
    CURSORMODE_LTARGETING
};
class Battle 
{
    const Map& map;
    vector<Entity *> ent;   //list of all entities in field
    Grid grid;     //index in 'ent'
    Team curplayer;
    int players_count;
    CursorMode CurMode;
    vInt MapScreenPosition;
    vInt FieldWindowSize;
    struct CurrentlySelected
    {
        Entity *ent;
        vInt coor;
        void Set(Entity *entity, const vInt& coord) { 
            ent = entity;
            coor = coord;
        }
        void SetNull() {
            ent = nullptr;
            coor = vInt();
        }
        CurrentlySelected(): ent(nullptr), coor(){}
    } CurSel;
    struct Spell_bar
    {
        int CurSet;
        int CurSpell;
        int ID() { return CurSet * actions_per_set + CurSpell; }
        void Clear() {*this = Spell_bar();}
        Spell_bar():CurSet(0), CurSpell(0) {}   //enchantt == 1?
    } Spellbar;
    struct TraceStep
    {
        vInt coor;
        bool IsJump;
        Direction dir;
        TraceStep(const vInt& coor, bool IsJump, Direction dir): 
            coor(coor), IsJump(IsJump), dir(dir) {}
    };
    struct TraceHolder
    {
        vector<TraceStep> trace;
    } MoveTrace;
    vector<Unit *> units_finishing_lactions;
    int current_laction_completing;
public:
    Battle(const Map& map);
    void EndTurn();
    void EndTurn(Entity* ent);
    void StartTurn(Entity* ent);
    //return true, if further automatic move is impossible
    bool ApplyEnvironmentEffects(Entity* ent);

    void CellClick(const vInt& coor, const MouseFlags flags);
    void KeyClick(const Keys Key);
    void ButtonClick(const Buttons Button);
    void ChooseSpell(const int id);

    void SetCurSel(Entity *entity);
    void SetCurSel(const vInt& coord);
    void Damage(Entity* source, Entity* target, const Dmg damage);
    //return true, if further automatic move is impossible
    bool Move(Entity* ent, const vInt& dest, const Direction dir = NO_DIRECTION);
    bool Push(Entity* ent, const Direction dir);
    void PushTo(Entity* ent, const vInt& coor);
    void PushFrom(Entity* ent, const vInt& coor);
    void Swap(Entity* ent1, Entity* ent2);
    void Turn(Entity* ent, const Direction dir);
    void Create(DefaultEntity& prototype, const vInt& coor, int team, Direction dir = NO_DIRECTION);
    void Remove(Entity* ent);
    //CursorMode Mode() { return CurMode; }

    void AddStep(const vInt& coor);
    void ClearMoveTrace();
    int EvaluateMoveCost();
    void ApplyUnitMove();

    void ClearLActions() { units_finishing_lactions.clear(); }
    void AddLAction(Unit* unit) { units_finishing_lactions.push_back(unit); }
    void NextLAction() {} //TODO!

    void RenderMap();
    void RenderCell(const vInt& coor);
    void RenderEntity(const Entity* ent, bool ClearCell = false);
    void RefreshScreen();
    void MoveScreen(const Direction dir);
    void MoveScreenTo(const vInt& coor);
    void RenderSelection();
    void RenderSelection(const vInt& coor);
    void RenderEntityInfo(const Entity* ent);
    void RenderEntityInfo(); //renders info of currently selected unit; does RenderSpellbar(); in it
    void RenderSpellbar(); //renders spellbar of currently selected unit\set\spell
    void RenderMoveTrace();
    void RenderMoveArrow(const TraceStep& step);
    ~Battle() {
        for (auto it = ent.begin(); it != ent.end(); ++it) {
            delete (*it);
        }
    }
};

namespace Loader {
    typedef vector<Map>             MapContainer;
    void loadMaps(MapContainer& container, EntityContainer& entities, 
        WallContainer walls, TileContainer tiles);
};

struct Game
{
    Loader::StringContainer  Strings;
    Loader::SoundContainer   Sounds;
    Loader::GestureContainer Gestures;
    Loader::EnchantContainer Enchants;
    Loader::ActionContainer  Actions;
    Loader::EntityContainer  Entities;
    Loader::WeaponContainer  Weapons;
    Loader::MapContainer     Maps;
    Loader::WallContainer    Walls;
    Loader::TileContainer    Tiles;
    Battle *battle;
    int Selection_Texture;
    int MoveArrows_Texture[Counters::directions];
    string AppPath;
//public:
    void load();
	Game();
    ~Game();
    void RenderInterface() {}
    int MoveArrow_Texture (Direction dir);
    int SelectionFrame_Texture ();
};
