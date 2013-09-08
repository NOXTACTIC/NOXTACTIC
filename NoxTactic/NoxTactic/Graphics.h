#pragma once
#include "Vector.h"
#include "Constants.h"
#include <vector>
#include <string>
#include <windows.h>
#include <d3dx9.h>
void InitializeRenderer(HINSTANCE& hInstance);
void ShutDownRenderer();
void render_frame();
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
struct Picture 
{
    vInt coor;
    int texture;
    Picture(const vInt& coor, const int texture): coor(coor), texture(texture){}
    Picture(): coor(0,0), texture(0){}
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
    vInt coor;
    int texture, active_texture;
    bool is_active;
    int get_texture() {return (is_active)? active_texture : texture;}
};
struct SpellBar
{
    Picture bar;
    vector<SpellIcon> icons;
};
class GraphicsInfoHolder
{
    std::vector<IDirect3DTexture9*> vtext;
    vInt mapsize, BattleWindowSize; //in cells
    std::vector<std::vector<std::vector<int>>> data;
    int background_index;
    vInt ScreenCoor;    //LU corner
    Picture portrait;
    SpellBar spellbar;
    //Labels + class for label+icon + interacting with spellbar
public:
    GraphicsInfoHolder(): vtext(), mapsize(), BattleWindowSize(), background_index(0), 
                          data(), ScreenCoor(), portrait(), spellbar(){}
    void SetPortrait(const int texture);
    void SetBackgroundTexture(const int texture);
    void SetScreenCoor(const vInt& coor);
    void SetSize(const vInt& size);
    void ClearCell(const vInt& coor);
    void SetTexture(const vInt& coor, const int texture);
    int AddTexture(const std::string& path, 
        const D3DCOLOR transparent_color = D3DCOLOR_XRGB(255,255,255)); //returns index of the added texture
    
    friend void render_frame();
};

extern GraphicsInfoHolder *GIH;