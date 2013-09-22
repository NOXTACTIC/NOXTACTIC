#include <Windows.h>
#include <WindowsX.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <list>
#include <vector>

class GraphicCore;
class EngineCore
{
    HINSTANCE inst;
    HWND wnd;
protected:
    std::string classname;
    std::string caption;
    WNDCLASSEX m_wcex;
    long pos_x, pos_y, width, height;
    std::string ApplicationPath;
    std::string ApplicationExeName;

    GraphicCore *graphic;
public:
    EngineCore(HINSTANCE& hInstance, const std::string& classname, const std::string& caption,
        long window_width, long window_height, long startposx, long startposy);
    HWND GetWnd() const;
    HINSTANCE GetInstance() const;

    void MoveWindow(long newx, long newy);
    void ResizeWindow(long newwidth, long newheight);
    void SetMouseVisibility(bool val);
    long WindowWidth() const { return width; }
    long WindowHeight() const { return height; }
    void SetGraphicCore(GraphicCore* gr) { graphic = gr; }
    GraphicCore* GraphicsCore() const { return graphic; }
    const std::string& AppPath() const { return ApplicationPath; }

    virtual LRESULT WINAPI MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    virtual void Init(){}
    virtual void Shutdown(){}
    virtual void Frame(){}
    virtual ~EngineCore();
};
extern EngineCore* core;

struct Color
{
    int R, G, B, A;
    Color(int R, int G, int B, int A = 0): R(R), G(G), B(B), A(A) {}
    Color(): R(0), G(0), B(0), A(0) {}
    operator D3DCOLOR(){ return D3DCOLOR_ARGB(A,R,G,B); }
};
struct Texture
{
    IDirect3DTexture9 *texture;
    long width, height;
    Texture(IDirect3DDevice9 *device, const std::string& Path, Color transparent);
    Texture(IDirect3DDevice9 *device, const std::string& Path);
};
class GraphicCore
{
    typedef std::vector<Texture> TextureContainer;
    HWND wnd;
    IDirect3D9 *d3d;
    IDirect3DDevice9 *device;
    ID3DXSprite *sprite;

    bool iswindowed, iszbuffer, ishal;
    long width, height;
    char bpp; //bits per pixel
    Color AmbientColor;

    D3DXMATRIX Camera, Perspective, World;

    std::string TexturesPath;
    TextureContainer textures;
public:
    GraphicCore(EngineCore* engine); 
    ~GraphicCore();

    // COM-interface getters
    IDirect3D9* GetD3D();
    IDirect3DDevice9* GetDevice();
    ID3DXSprite* GetSprite();

    void Init();     
    void Shutdown();

    void SetMode(HWND hWnd, bool Windowed = TRUE, bool UseZBuffer = FALSE,
                 long Width = 0, long Height = 0, char BPP = 0);

    char GetFormatBPP(D3DFORMAT Format);

    //void Display();     // Отображает вторичный буфер
                        // (выполняя переключение страниц)

    void BeginScene();  
    void EndScene();  

    void BeginSprite(); 
    void PaintSprite(int TextureID, const D3DXVECTOR2& coor, const D3DXVECTOR2& scaling);
    void EndSprite(); 

    void ClearDisplay(long Color = 0);
    void ClearZBuffer(float ZBuffer = 1.0f);
    void Clear(long Color = 0, float ZBuffer = 1.0f);

    long GetWidth();
    long GetHeight();
    char GetBPP();

    //BOOL GetHAL();
    //BOOL GetZBuffer();

    void SetPerspective(float FOV = D3DX_PI/4, float aspect = 1.3333f,
                        float Near = 1.0f, float Far = 10000.0f);
    void SetCamera(const D3DXVECTOR3& Position, const D3DXVECTOR3& Point, const D3DXVECTOR3& UpDirection);
    void ClearTranslation();
    void WorldMove(const D3DXVECTOR3& coor);
    void WorldRotateX(float angle);
    void WorldRotateY(float angle);
    void WorldRotateZ(float angle);
    void WorldScale(const D3DXVECTOR3& scale);

    // Установка и получение компонент фонового освещения
    Color& AmbientLight();

    //use absolute path
    int AddTexture_A(const std::string& AbsolutePath, const Color& transparent);
    int AddTexture_A(const std::string& AbsolutePath);

    //terminating with '/' is required
    void SetTexturesPath(const std::string& Path);
    //use local path
    int AddTexture(const std::string& SubPath, const Color& transparent);
    int AddTexture(const std::string& SubPath);
};