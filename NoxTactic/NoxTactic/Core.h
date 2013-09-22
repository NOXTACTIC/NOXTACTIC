#include "Engine.h"
#include "Geometry.h"
#include "Constants.h"
#include "Grid.h"
#include "Entity.h"
#include "Enchants.h"
#pragma once
extern EngineCore* core;
class Battle;
class Graphics;
class Game: public EngineCore
{
    ContainerDefs::StringContainer  Strings;
    ContainerDefs::SoundContainer   Sounds;
    ContainerDefs::GestureContainer Gestures;
    ContainerDefs::EnchantContainer Enchants;
    ContainerDefs::ActionContainer  Actions;
    ContainerDefs::EntityContainer  Entities;
    ContainerDefs::WeaponContainer  Weapons;
    ContainerDefs::MapContainer     Maps;
    ContainerDefs::WallContainer    Walls;
    ContainerDefs::TileContainer    Tiles;
    Battle *battle;
    int Selection_Texture;
    int MoveArrows_Texture[Counters::directions];
public:
    Graphics* Graphic();
    Game(EngineCore core);

    const Enchant* DefEnchant(enumEnchants id) const { return Enchants[id]; }
    const std::string String(enumStrings id) const { return Strings[id]; }
    const DefaultEntity* DefEntity(enumEntities id) const { return Entities[id]; }
    const Wall* DefWall(enumWalls id) const { return Walls[id]; }
    const Tile* DefTile(enumTiles id) const { return Tiles[id]; }
    const Map* DefMap(int id) const { return Maps[id]; }
    const Weapon* DefWeapon(enumWeapons id) const { return Weapons[id]; }
    const Action* DefAction(enumActions id) const { return Actions[id]; }
    const Gesture* DefGesture(enumGestures id) const { return Gestures[id]; }
    const Sound* DefSound(enumSounds id) const { return Sounds[id]; }

    virtual void Init();
    virtual void Shutdown();
    virtual void Frame(){}
    virtual ~Game(){}

    void StartBattle(Battle* battle);
    void EndBattle() {}

    int MoveArrow_Texture (Direction dir);
    int SelectionFrame_Texture ();

    void InitActionTextures(Action* action);
    void InitEntTextures(DefaultEntity* defent);
};

struct Picture 
{
    vInt coor;
    int texture;
    Picture(const vInt& coor, const int texture): coor(coor), texture(texture){}
    Picture(): coor(0,0), texture(0){}
};
struct Button: public Picture
{
    Buttons ID;
    Button(Picture pic, Buttons ID): Picture(pic), ID(ID) {}
    Button(): Picture(), ID(NO_BUTTON) {}
};
struct Label 
{
    vInt coor;
    std::string text;
    Label(const vInt& coor, const std::string& text): coor(coor), text(text){}
    Label(): coor(0,0), text(){}
};
struct SpellIcon
{
    int texture, active_texture;
};
struct SpellBar
{
    Picture bar;
    vector<SpellIcon> icons;
    static const int icons_startposx = 10;
    static const int icons_startposy = 10;
    static const int indent = 10;
    int active_index;
    vInt GetIconCoor(int num) const {
        return bar.coor + vInt(icons_startposx, icons_startposy) + vInt(indent * (num-1), 0);
    }
    int GetIconTexture(int num) const {
        if (num == active_index) {
            return icons[num].active_texture;
        } else {
            return icons[num].texture;
        }
    }
    SpellBar(Picture pic = Picture()): 
        bar(pic), icons(vector<SpellIcon>(actions_per_set)), active_index(-1) {}
};
class Graphics: public GraphicCore
{
    std::vector<std::vector<std::vector<int>>> map;
    vInt size;
    vector<Picture> pics;
    vector<Button> buttons;
    vector<Label> labels;
    SpellBar spellbar;
public:
    Graphics(Game* game): GraphicCore(static_cast<EngineCore*>(game)) {}
    void SetMapSize(const vInt& size);
    void SetMapSize(int x, int y);
    void PushTexture(const vInt& coor, int texture);
    void ClearCell(const vInt& coor);
    vInt Size() const { return size; }

    void AddPicture(Picture pic);
    void AddLabel(Label label);
    void AddButton(Button button);
    void SetSpellbar(SpellBar bar);
    
    void Paint(const vInt& coor1, const vInt& coor2);
};

class Battle 
{
    Game* game;
    const Map* map;
    vector<Entity *> ent;   //list of all entities in field
    Grid grid;
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
    struct LActionsInCompletionInfo
    {
        vector<Unit *> units;
        int i;
    } LongActions;
public:
    Battle(const Map* map, Game* game);
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

    void LaunchProjectile(const DefaultProjectile& projectile_type, Entity* source, const vInt& dest);
    void LaunchHomingProjectile(const DefaultProjectile& projectile_type, Entity* source, Entity* target);
    void MoveProjectile(Entity* projectile, bool IsFirstMove = false);

    void AddStep(const vInt& coor);
    void ClearMoveTrace();
    int EvaluateMoveCost();
    void ApplyUnitMove();

    void ClearLActions() { LongActions.units.clear(); }
    void AddLAction(Unit* unit) { LongActions.units.push_back(unit); }
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
    friend class Game;
};

class Gesture
{
	const Sound* sound;
	const int powernumber;
public:
	Gesture(const Sound* sound, const int number);
	void CastMe(const int volume) const;
};

class Action
{
protected:
	int range, damage1, damage2, time_value, time_to_perform;
	Sound* sound;
    enumStrings name, msg_performed, msg_started, msg_stopped, msg_affected, msg_killed;
    int flags;
    int icon_texture_id;
    int active_icon_texture_id;
    void (*action)(Entity*, const vInt&, const Action*); //func to be called 
public:
    Action(void (*action)(Entity*, const vInt&, const Action*), 
            int range, int damage1, int damage2, int timevalue, int performtime,
            Sound* sound, enumStrings name, enumStrings msg_done, enumStrings msg_started, 
            enumStrings msg_stopped, enumStrings msg_affected, enumStrings msg_killed, int flags);
	virtual bool Perform(Entity* actor, const vInt& target, const Grid& grid) const;
    virtual bool CheckForValidity(Entity* actor, const vInt& target, const Grid& grid) const;
    int Range() const {return range;}
    bool Flag(const ActionFlags flag) const { return ((flags & flag) == 1); }
    bool IsLengthy() const { return time_to_perform != 0; }
    int TimeToPerform() const { return time_to_perform; }
    int MyIcon() const { return icon_texture_id; }
    int MyActiveIcon() const { return active_icon_texture_id; }
    enumStrings Name() const { return name; }
    void SetIcon(int texture) { icon_texture_id = texture; }
    void SetActiveIcon(int texture) { active_icon_texture_id = texture; }
};
class Spell: public Action
{
	enumGestures gesture[8];
	const int manacost;
public:
    Spell(Action action, int manacost, enumGestures g1, enumGestures g2, enumGestures g3, 
        enumGestures g4, enumGestures g5, enumGestures g6, enumGestures g7, enumGestures g8);
    
	virtual bool Perform(Entity* actor, const vInt& target, const Grid& grid) const;
    virtual bool CheckForValidity(Entity* actor, const vInt& target, const Grid& grid) const;
    int Cost() const {return manacost;}
};
class ContinuousSpell: public Spell
{
    enumContinuousSpells id;
    void (*continuous_action)(Entity*, Entity*, const Action*); //func to be called 
public:
    ContinuousSpell(Spell spell, enumContinuousSpells id, void (*continuous_action)(Entity*, Entity*, const Action*));
    enumContinuousSpells ID() const { return id;}
	virtual bool Perform(Entity* actor, const vInt& target, const Grid& grid) const;
    virtual bool CheckForValidity(Entity* actor, const vInt& target, const Grid& grid) const;
    virtual bool ApplyEffect(Entity* actor, Entity* target) const;
};
class EnchantSpell: public Spell
{
    enumEnchants id;
public:
    EnchantSpell(Spell spell, enumEnchants id);
	virtual bool Perform(Entity* actor, const vInt& target, const Grid& grid) const;
    virtual bool CheckForValidity(Entity* actor, const vInt& target, const Grid& grid) const;
};
class Ability: public Action
{
    const int cooldown;
    enumCooldowns cd_index;
public:
    Ability(Action action, int cooldown, enumCooldowns cd_index);
	virtual bool Perform(Entity* actor, const vInt& target, const Grid& grid) const;
    virtual bool CheckForValidity(Entity* actor, const vInt& target, const Grid& grid) const;
    int cooldown_index() const {return cd_index;}
    int cooldown_value() const {return cooldown;}
};

struct WallCoor
{
    vInt coor;
    Wall* wall;
    WallCoor(const vInt& coor, Wall* wall): coor(coor), wall(wall){}
};
class Map
{
    vInt size;
    vector<Entity *> entities;
    vector<WallCoor> walls;
    vector<vector<Tile*>> tiles;
    int players_count;
public:
    Map(const vInt& size, const vector<Entity *>& entities, vector<WallCoor> walls, vector<vector<Tile*>> tiles, const int players_count): 
      size(size), entities(entities), walls(walls), tiles(tiles), players_count(players_count) {}
    ~Map();
    void Destroy();
    friend class Battle;
};

namespace Loader {
    void loadActions(ActionContainer& container, SoundContainer& sounds, 
        FunctionContainer& functions, ContinuousFunctionContainer& continuous_functions);
    
    void loadGestures(GestureContainer& container, SoundContainer& sounds);
    
    void loadEnchants(EnchantContainer& container, SoundContainer& sounds);
    
    void loadMaps(MapContainer& container, EntityContainer& entities, 
        WallContainer& walls, TileContainer& tiles);
};