#include "Grid.h"
#include "Entity.h"
#include <algorithm>
#include "Graphics.h"
Grid::Grid(int sizex, int sizey): size_x(sizex), size_y(sizey){
    data.resize(sizex);
    for(auto it = data.begin(); it != data.end(); ++it){
        (*it).resize(sizey);
    }
}
Grid::Grid(vInt size): size_x(size.x), size_y(size.y){
    data.resize(size.x);
    for(auto it = data.begin(); it != data.end(); ++it){
        (*it).resize(size.y);
    }
}

Entity* Grid::operator()(const int x, const int y) {
    for (auto it = begin(data[x][y].ents); it != end(data[x][y].ents); ++it) {
        if (!(*it)->Flag(ENT_IS_UNTARGETABLE)) {
            return *it;
        }
    }
    //nothing was found
    return nullptr;
}

list<Entity*>::iterator Grid::FindMe(Entity* value) {
    return find(data[value->Coor().x][value->Coor().y].ents.begin(),
                data[value->Coor().x][value->Coor().y].ents.end(), value);
}
void Grid::Move(Entity* ent, const vInt& dest, const Direction dir){
    if (ent->Coor() == dest) { return; }
    auto it = FindMe(ent);
    data[ent->Coor().x][ent->Coor().y].ents.erase(it);
    data[dest.x][dest.y].ents.push_back(ent);
    ent->Move(dest, dir);
}
void Grid::Swap(Entity* ent1, Entity* ent2){
    auto it1 = FindMe(ent1);
    auto it2 = FindMe(ent2);
    *it1 = ent2; *it2 = ent1;
    vInt coor1 = ent1->Coor(), coor2 = ent2->Coor();
    ent1->Move(coor2);
    ent2->Move(coor1);
}
void Grid::Add(Entity* ent){
    data[ent->Coor().x][ent->Coor().y].ents.push_back(ent);
}

void Grid::SetTile(const vInt& coor, Tile tile) {
    data[coor.x][coor.y].tile = tile;
}
void Grid::SetWall(const vInt& coor, Wall wall) {
    data[coor.x][coor.y].is_wall = true;
    data[coor.x][coor.y].wall = wall;
}
void Grid::RemoveWall(const vInt& coor) {
    data[coor.x][coor.y].is_wall = false;
}

bool Grid::IsSeenFrom(const vInt& start, const vInt& dest) const {
    Path trace = TraceLine(vInt(start), vInt(dest));
    for (auto it = trace.begin(); it != trace.end(); ++it) {
        if (BlocksSight(*it)) {
            return false;
        }
    }
    return true;
}
bool Grid::BlocksMoving(const Entity* ent, const vInt& dest, bool UnitJumps) const {
    if (ent->Flag(ENT_IS_FLYING)) { //it flies
        if (data[dest.x][dest.y].is_wall || !data[dest.x][dest.y].wall.HasWindow()) { //there is windowless wall
            return true;
        }

        for(auto it = data[dest.x][dest.y].ents.begin();
            it != data[dest.x][dest.y].ents.begin();
            ++it) 
        {
            if ((*it)->Flag(ENT_IS_SOLID) && 
               ((*it)->Flag(ENT_IS_TALL) || (*it)->Flag(ENT_IS_FLYING))) {
                   return true;
            }
        };
    } else { //it doesnt fly
        if (data[dest.x][dest.y].is_wall) { //there is wall
            return true;
        }

        for(auto it = data[dest.x][dest.y].ents.begin();
            it != data[dest.x][dest.y].ents.begin();
            ++it) 
        {
            if ( UnitJumps && (*it)->Flag(ENT_IS_SOLID) &&  //it jumps over short and grounded objects
               ((*it)->Flag(ENT_IS_TALL) || (*it)->Flag(ENT_IS_FLYING))) {
                   return true;
            }
            if (!UnitJumps && (*it)->Flag(ENT_IS_SOLID)) { //it doesnt jump nor flies
                return true;
            }
        };
    }
    return false;
}
bool Grid::BlocksSight(const vInt& dest) const {
    if (data[dest.x][dest.y].is_wall && !data[dest.x][dest.y].wall.HasWindow()) {
        return true;
    }
    for(auto it = data[dest.x][dest.y].ents.begin();
            it != data[dest.x][dest.y].ents.begin();
            ++it) 
    {
        if ((*it)->Flag(ENT_IS_OPAQUE)) {
            return true;
        }
    };
    return false;
}

bool Grid::IsInside(const vInt& v) const {
    return v.IsInside(vInt(1,1), vInt(size_x, size_y));
}

void Bresenham(Path& res, vInt start, vInt dest) {
    const int deltaX = abs(dest.x - start.x);
    const int deltaY = abs(dest.y - start.y);
    const int signX = start.x < dest.x ? 1 : -1;
    const int signY = start.y < dest.y ? 1 : -1;
    //
    int error = deltaX - deltaY;
    //
    res.push_back(dest);
    while(start.x != dest.x || start.y != dest.y) {
        res.push_back(start);
        const int error2 = error * 2;
        //
        if(error2 > -deltaY) {
            error -= deltaY;
            start.x += signX;
        }
        if(error2 < deltaX) {
            error += deltaX;
            start.y += signY;
        }
    }
}
Path Grid::TraceRay(vInt start, vInt dest) const {
    Path res;
    res.push_back(start);
    if (start == dest) {
        return res;
    }
    
    vInt diff = dest - start;
    while (IsInside(dest)) {
        dest += diff;
    }
    Bresenham(res, start, dest);
    return res;
}
Path Grid::TraceLine(vInt start, vInt dest) const {
    Path res;
    res.push_back(start);
    if (start == dest) {
        return res;
    }
    
    Bresenham(res, start, dest);
    return res;
}

Path Grid::FindPath(vInt start, vInt dest, const Entity* ent) const { return Path(); }


void Loader::loadTiles(TileContainer& container, std::string AppPath, const StringContainer& strings) {
    container.push_back(Tile(0, NO_STRING));

}
void Loader::loadWalls(WallContainer& container, std::string AppPath, const StringContainer& strings) {


}
