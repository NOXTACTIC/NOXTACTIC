#pragma once
#include <vector>
#include <list>
#include <string>
#include "Strings.h"
#include "Vector.h"
class Entity;
#include "Entity.h"
typedef std::list<Entity*> EntContainer;
typedef std::vector<vInt> Path;
class GridComparer_All
{
public:
    bool operator()(const Entity* ent) { return true; }
};
class GridComparer_Obelisks
{
public:
    bool operator()(const Entity* ent) { return ent->Flag(ENT_IS_OBELISK); }
};
class GridComparer_Units
{
public:
    bool operator()(const Entity* ent) { return ent->Flag(ENT_IS_UNIT); }
};
class GridComparer_EnvironmentAffecting
{
public:
    bool operator()(const Entity* ent) { 
        return (ent->Flag(ENT_IS_PROJECTILE) || ent->Flag(ENT_IS_OBELISK) ||
            ent->AuraDamage().value > 0 || ent->AuraRadiusDamage().value > 0 ||
            (ent->Flag(ENT_IS_UNIT) && dynamic_cast<const Unit*>(ent)->IsEnchanted(ENCHANT_SHOCK)));
    }
};

enum enumTiles 
{
    NO_TILE,
    TILE_LAVA,
    TILE_RUG,
    TILE_GRASS
    //etc
};
enum enumWalls
{
    NO_WALL,
    WALL_COBBLESTONE,
    WALL_BRICK,
    WALL_UNDERWORLD,
    WALL_MAGIC
    //etc
};
class Tile
{
    int texture_id;
    enumStrings name;
public:
    Tile(int texture, enumStrings name): texture_id(texture), name(name) {}
    Tile(): texture_id(0), name(NO_STRING) {}
    int MyTexture() const { return texture_id; }
};
class Wall
{
    int texture_id;
    bool window;
    bool indestructible;
    enumStrings name;
public:
    Wall(int texture, bool window, enumStrings name, bool indestructible = true): 
      texture_id(texture), window(window), name(name), indestructible(indestructible) {}
    Wall(): texture_id(0), window(false), name(NO_STRING), indestructible(true) {}
    int MyTexture() const { return texture_id; }
    bool HasWindow() const { return window; }
    bool IsDestructible() const { return !indestructible; }
};

namespace Loader
{
    typedef std::vector<Tile> TileContainer;
    typedef std::vector<Wall> WallContainer;
    void loadTiles(TileContainer& container, std::string AppPath, const StringContainer& strings);
    void loadWalls(WallContainer& container, std::string AppPath, const StringContainer& strings);
};

class EntityCoorSorter {
    int center;
public:
    EntityCoorSorter(int center): center(center) {}
    bool operator()(const Entity* ent1, const Entity* ent2) {
        return ((ent1->Coor() - center).length() < (ent2->Coor() - center).length());
    }
};
class EntityZOrderSorter {
public:
    bool operator()(const Entity* ent1, const Entity* ent2) {
        int weight1 = 0, weight2 = 0;
        if (ent1->Flag(ENT_IS_SOLID)) { weight1 += 100; }
        if (ent2->Flag(ENT_IS_SOLID)) { weight2 += 100; }
        //moar weight
        return (weight1 < weight2);
    }
};

class Grid
{
    struct Cell
    {
        list<Entity *> ents;
        bool is_wall;
        Wall wall;
        Tile tile;
    };
    std::vector<std::vector<Cell>> data;
    int size_x, size_y;
    list<Entity*>::iterator FindMe(Entity* value);
    bool IsInside(const vInt& v) const;
public:
    Grid(int sizex, int sizey);
    Grid(vInt size);
    //note that it DOES change their coordinates
    void Move(Entity* ent, const vInt& dest, const Direction dir = NO_DIRECTION);
    void Swap(Entity* ent1, Entity* ent2);
    void Add(Entity* ent);
    void SetWall(const vInt& coor, Wall wall);
    void SetTile(const vInt& coor, Tile tile);
    void RemoveWall(const vInt& coor);

    bool BlocksMoving(const Entity* ent, const vInt& dest, bool UnitJumps = false) const;
    bool BlocksSight(const vInt& dest) const;
    bool IsSeenFrom(const vInt& start, const vInt& dest) const;
    Entity* operator()(const int x, const int y);
    Entity* operator()(const vInt& coor){ return (*this)(coor.x, coor.y);}
    int sizex(){return size_x;}
    int sizey(){return size_y;}

    Path TraceRay(vInt start, vInt dest) const;
    Path TraceLine(vInt start, vInt dest) const;
    Path FindPath(vInt start, vInt dest, const Entity* ent) const;

    //Comparer has to have "bool operator()(const Entity*);"
    template<class Comparer> 
    void Getter_ProcessCell(EntContainer& container, const vInt& coor, Comparer comparer = GridComparer_All()) const {
        if (IsInside(coor)) {
            const Cell& cell = data[coor.x][coor.y];
            for (auto it = cell.ents.begin(); it != cell.ents.end(); ++it) {
                if (comparer(*it)) {
                    container.push_back(*it);
                }
            }
        }
    }
    
    template<class Comparer> 
    EntContainer GetCell(const vInt& coor, Comparer comparer = GridComparer_All()) {
        EntContainer container;
        Getter_ProcessCell(container, coor, comparer);
        return container;
    }
    template<class Comparer> 
    EntContainer GetRing(const vInt& center, const int radius, Comparer comparer = GridComparer_All()) const;
    template<class Comparer> 
    EntContainer GetSolidRing(const vInt& center, const int radius, Comparer comparer = GridComparer_All()) const;
    template<class Comparer> 
    EntContainer GetSquare(const vInt& center, const int radius, Comparer comparer = GridComparer_All()) const {
        EntContainer container;
        int x, y = center.y - radius;
        for (x = center.x - radius; x <= center.x + radius; ++x) {
            Getter_ProcessCell(container, vInt(x, y), comparer);
        }
        for (y = center.y - radius + 1; y <= center.y - 1; ++y) {
            Getter_ProcessCell(container, vInt(center.x - radius, y), comparer);
            Getter_ProcessCell(container, vInt(center.x + radius, y), comparer);
        }
        y = center.y + radius;
        for (x = center.x - radius; x <= center.x + radius; ++x) {
            Getter_ProcessCell(container, vInt(x, y), comparer);
        }
        return container;
    }
    template<class Comparer> 
    EntContainer GetSolidSquare(const vInt& center, const int radius, Comparer comparer = GridComparer_All()) const {
        EntContainer container, tmp;
        for (int i = 1; i <= radius; ++i) {
            tmp = GetSquare(center, i, comparer);
            container.splice(container.end(), tmp);
        }
        return container;
    }
    
    template<class Comparer> Entity* FindNearest(const vInt& center, Comparer comparer = GridComparer_All(), int max_radius = 0) const {
        if (IsInside(center)) {
            if (max_radius == 0) {
                max_radius = max(max(abs(size_x - center.x), center.x - 1), 
                                 max(abs(size_y - center.y), center.y - 1));
            }
            EntContainer container = GetSolidSquare(center, i, comparer);
            if (!container.empty()){
                container.sort(EntityCoorSorter(center));
                return *(container.begin());
            }
        }
        return nullptr;
    }
};