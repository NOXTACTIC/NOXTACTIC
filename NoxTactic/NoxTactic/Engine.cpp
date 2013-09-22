#include "Engine.h"
#include <Windows.h>
#include <WindowsX.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <list>
#include <vector>

EngineCore* core;
LRESULT WINAPI MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (!core) {
        return 0;
    } else {
        return core->MsgProc(hWnd, uMsg, wParam, lParam);
    }
}

HWND EngineCore::GetWnd() const { return wnd; }
HINSTANCE EngineCore::GetInstance() const { return inst; }

void EngineCore::MoveWindow(long newx, long newy) {}
void EngineCore::ResizeWindow(long newwidth, long newheight) {}
void EngineCore::SetMouseVisibility(bool val) {}

EngineCore::EngineCore(HINSTANCE& hInstance, const std::string& classname, const std::string& caption,
    long window_width, long window_height, long startposx, long startposy):
    inst(hInstance), classname(classname), caption(caption), width(window_width), height(window_height),
    pos_x(startposx), pos_y(startposy)
{
    WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_VREDRAW|CS_HREDRAW|CS_OWNDC, 
        ::MsgProc, 0, 0, hInstance, NULL, NULL, (HBRUSH)(COLOR_WINDOW+1), 
        NULL, classname.data(), NULL};
    RegisterClassEx(&wc);
    HWND hWnd = CreateWindowEx(0, classname.data(), caption.data(),
        WS_EX_TOPMOST , 100, 100, width, height,
        NULL, NULL, hInstance, NULL);
    {
    //code taken from 
    //http://stackoverflow.com/questions/2647429/c-windows-path-to-the-folder-where-the-executable-is-located
        HMODULE hModule = GetModuleHandleW(NULL);
        CHAR path[MAX_PATH];
        GetModuleFileName(hModule, path, MAX_PATH);

        ApplicationPath = std::string(path);
        int dotpos = ApplicationPath.find_last_of('\\');
        ApplicationExeName = std::string(ApplicationPath.begin() + dotpos, ApplicationPath.end());
        ApplicationPath.erase(ApplicationPath.begin() + dotpos, ApplicationPath.end());
    }
}
EngineCore::~EngineCore() {}

GraphicCore::GraphicCore(EngineCore* engine): wnd(engine->GetWnd()), 
    width(engine->WindowHeight()), height(engine->WindowHeight()), iszbuffer(false) {} 
GraphicCore::~GraphicCore() {}

// COM-interface getters
IDirect3D9* GraphicCore::GetD3D(){ return d3d; }
IDirect3DDevice9* GraphicCore::GetDevice(){ return device; }
ID3DXSprite* GraphicCore::GetSprite(){ return sprite; }

void GraphicCore::Init(){
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    D3DPRESENT_PARAMETERS d3dpp;        // create a struct to hold various device information
    ZeroMemory(&d3dpp, sizeof(d3dpp));  // clear out the struct for use
    d3dpp.Windowed = TRUE;              // program windowed, not fullscreen
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
    d3dpp.hDeviceWindow = wnd;          // set the window to be used by Direct3D
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;    // set the back buffer format to 32-bit
    d3dpp.BackBufferWidth = width;      // set the width of the buffer
    d3dpp.BackBufferHeight = height;    // set the height of the buffer
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd,
                      D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &device);

    device->SetRenderState(D3DRS_LIGHTING, FALSE); 
    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    
    device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

    D3DXCreateSprite(device, &sprite);

    ShowWindow(wnd, SW_SHOW);
    UpdateWindow(wnd);
}     
void GraphicCore::Shutdown(){
    for (auto it = textures.begin(); it != textures.end(); ++it) {
        (*it).texture->Release();
    }
    sprite->Release();
    device->Release();
    d3d->Release();
}

void GraphicCore::SetMode(HWND hWnd, bool Windowed, bool UseZBuffer,
                long Width, long Height, char BPP){}

//char GraphicCore::GetFormatBPP(D3DFORMAT Format){}

//void Display(){}     // Отображает вторичный буфер
                    // (выполняя переключение страниц)

void GraphicCore::BeginScene(){
    device->BeginScene();
}  
void GraphicCore::EndScene(){
    device->EndScene();
}  

void GraphicCore::BeginSprite(){
    sprite->Begin(0);
} 
void GraphicCore::PaintSprite(int TextureID, const D3DXVECTOR2& coor, const D3DXVECTOR2& scaling) {
    sprite->Draw(textures[TextureID].texture, NULL, &D3DXVECTOR3(coor.x, coor.y, 0),
                    &D3DXVECTOR3(coor.x, coor.y, 0), NULL);
}
void GraphicCore::EndSprite(){
    sprite->End();
} 

void GraphicCore::ClearDisplay(long Color){}
void GraphicCore::ClearZBuffer(float ZBuffer){}
void GraphicCore::Clear(long Color, float ZBuffer){}

long GraphicCore::GetWidth(){ return width; }
long GraphicCore::GetHeight(){ return height; }
char GraphicCore::GetBPP(){ return bpp; }

//BOOL GraphicCore::GetHAL(){}
//BOOL GraphicCore::GetZBuffer(){}

void GraphicCore::SetPerspective(float fov, float aspect, float Near, float Far){
    D3DXMatrixPerspectiveFovLH(&Perspective, fov, aspect, Near, Far);
}
void GraphicCore::SetCamera(const D3DXVECTOR3& Position, const D3DXVECTOR3& Point, const D3DXVECTOR3& UpDirection) {
    D3DXMatrixLookAtLH(&Camera, &Position, &Point, &UpDirection);
}
void GraphicCore::ClearTranslation(){
    D3DXMatrixScaling(&World, 1, 1, 1);
}
void GraphicCore::WorldMove(const D3DXVECTOR3& coor){
    D3DXMATRIX matrix;
    D3DXMatrixTranslation(&matrix, coor.x, coor.y, coor.z);
    World = World * matrix;
}
void GraphicCore::WorldRotateX(float angle){
    D3DXMATRIX matrix;
    D3DXMatrixRotationX(&matrix, angle);
    World = World * matrix;}
void GraphicCore::WorldRotateY(float angle){
    D3DXMATRIX matrix;
    D3DXMatrixRotationY(&matrix, angle);
    World = World * matrix;}
void GraphicCore::WorldRotateZ(float angle){
    D3DXMATRIX matrix;
    D3DXMatrixRotationZ(&matrix, angle);
    World = World * matrix;
}
void GraphicCore::WorldScale(const D3DXVECTOR3& scale){
    D3DXMATRIX matrix;
    D3DXMatrixScaling(&matrix, scale.x, scale.y, scale.z);
    World = World * matrix;
}

// Установка и получение компонент фонового освещения
Color& GraphicCore::AmbientLight(){
    return AmbientColor;
}

//use absolute path
int GraphicCore::AddTexture_A(const std::string& AbsolutePath, const Color& transparent){
    Texture tmptext(device, AbsolutePath, transparent);
    textures.push_back(tmptext);
    return textures.size() - 1;
}
int GraphicCore::AddTexture_A(const std::string& AbsolutePath){
    Texture tmptext(device, AbsolutePath);
    textures.push_back(tmptext);
    return textures.size() - 1;
}

//terminating with '/' is required
void GraphicCore::SetTexturesPath(const std::string& Path){
    TexturesPath = Path;
}
//use local path
int GraphicCore::AddTexture(const std::string& SubPath, const Color& transparent){
    return AddTexture_A(TexturesPath + SubPath, transparent);
}
int GraphicCore::AddTexture(const std::string& SubPath){
    return AddTexture_A(TexturesPath + SubPath);
}

Texture::Texture(IDirect3DDevice9 *device, const std::string& Path, Color transparent) {
    D3DXCreateTextureFromFileEx(device, Path.data(), 0, 0, 0, 0, D3DFMT_UNKNOWN, 
        D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, transparent, 0, 0, &texture);
}
Texture::Texture(IDirect3DDevice9 *device, const std::string& Path) { 
    D3DXCreateTextureFromFileEx(device, Path.data(), 0, 0, 0, 0, D3DFMT_UNKNOWN, 
        D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0, &texture);
}
