#include<d3d9.h>
#include<d3dx9.h>
#include<windows.h>
#include<vector>
#include<list>
#ifndef HEADER_D3D
#define HEADER_D3D
class SApplication;
class OBase{
    protected:
        SApplication* parent;
        std::list<OBase*>::iterator id;
    public:
        OBase(SApplication* p);
        virtual ~OBase();
        virtual void tick(float deltaTime)=0;
        virtual void Render()=0;
        bool needTick=1;
        bool needRender=1;
};
class SWindow{
    private:
        SApplication* parent;
        HWND hWnd;
        float lastTime;
        void msgLoop();
        virtual void tick(float deltaTime);
        static LRESULT WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam){
            switch (msg)
            {
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            
            default:
                break;
            }
            return DefWindowProc(hWnd,msg,wParam,lParam);
        };
    public:
        SWindow(SApplication* p):parent(p){
            hWnd=0;
            lastTime=0.0f;
        }
        HWND getHwnd()const{
            return hWnd;
        }
        bool Init(HINSTANCE hInstance,LPCSTR lpszClassName,LPCSTR lpszWindowName,int width,int height){
            int ret;
            WNDCLASS wndSet;
            wndSet.cbClsExtra=0;
            wndSet.cbWndExtra=0;
            wndSet.hbrBackground=HBRUSH(COLOR_WINDOWFRAME);
            wndSet.hCursor=LoadCursor(NULL,IDC_ARROW);
            wndSet.hIcon=LoadIcon(NULL,IDI_APPLICATION);
            wndSet.hInstance=hInstance;
            wndSet.lpszClassName=lpszClassName;
            wndSet.lpszMenuName=NULL;
            wndSet.lpfnWndProc=(WNDPROC)(this->WndProc);
            wndSet.style=CS_HREDRAW|CS_VREDRAW;
            ret=RegisterClass(&wndSet);
            if(ret==0) return 0;
            hWnd=CreateWindow(lpszClassName,lpszWindowName,WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,width,height,NULL,NULL,hInstance,0);
            if(hWnd==0) return 0;
            return 1;
        }
        void exec(){
            ShowWindow(hWnd,10);
            UpdateWindow(hWnd);
            msgLoop();
        }
};
class SD3D{
    private:
        SApplication* parent;
        IDirect3DDevice9* _device;
    public:
        SD3D(SApplication* p):parent(p){
            _device=0;
        }
        bool Init(HINSTANCE hInstance,int width,int height,bool windowed,D3DDEVTYPE deviceType,const SWindow& w){
            HRESULT hr=0;
            IDirect3D9* d3d9=0;
            d3d9=Direct3DCreate9(D3D_SDK_VERSION);
            if(!d3d9){
                return 0;
            }
            D3DCAPS9 caps;
            d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT,deviceType,&caps);
            int vp=0;
            if(D3DDEVCAPS_HWTRANSFORMANDLIGHT&caps.DevCaps){
                vp=D3DCREATE_HARDWARE_VERTEXPROCESSING;
            }else{
                vp=D3DCREATE_SOFTWARE_VERTEXPROCESSING;
            }
            D3DPRESENT_PARAMETERS d3dpp;
            d3dpp.BackBufferWidth=width;
            d3dpp.BackBufferHeight=height;
            d3dpp.BackBufferFormat=D3DFMT_A8R8G8B8;
            d3dpp.BackBufferCount=2;
            d3dpp.MultiSampleType=D3DMULTISAMPLE_NONE;
            d3dpp.MultiSampleQuality=0;
            d3dpp.SwapEffect=D3DSWAPEFFECT_FLIP;
            d3dpp.hDeviceWindow=w.getHwnd();
            d3dpp.Windowed=windowed;
            d3dpp.AutoDepthStencilFormat=D3DFMT_D24S8;
            d3dpp.EnableAutoDepthStencil=true;
            d3dpp.Flags=0;
            d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;
            d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
            hr=d3d9->CreateDevice(D3DADAPTER_DEFAULT,deviceType,w.getHwnd(),vp,&d3dpp,&_device);
            if(FAILED(hr)){
                d3d9->Release();
                return 0;
            }
            d3d9->Release();
            return 1;
        }
        IDirect3DDevice9* getDevice()const{
            return _device;
        }
};
class SApplication{
    public:
        SApplication();
        ~SApplication();
        bool Init(HINSTANCE hInstance,LPCSTR lpszClassName,LPCSTR lpszWindowName,int width,int height,bool windowed,D3DDEVTYPE deviceType);
        bool CreateScence();
        virtual void tick(float deltaTime);
        void Render();
        IDirect3DDevice9* getDevice(){
            return d3d.getDevice();
        }
        std::list<OBase*>::iterator addObject(OBase* o){
            allObject.push_front(o);
            //std::cout<<allObject.rbegin().base()<<endl;
            return allObject.begin();
        }
        void deleteObject(std::list<OBase*>::iterator id){
            allObject.erase(id);
        }
        void run(){
            window.exec();
        }
    private:
        int width;
        int height;
        std::list<OBase*> allObject;
        SWindow window;
        SD3D d3d;
        bool inited=0;
};
SApplication::SApplication():window(this),d3d(this){

}
SApplication::~SApplication(){
    if(inited){
        CloseWindow(window.getHwnd());
        d3d.getDevice()->Release();
    }
}
bool SApplication::Init(HINSTANCE hInstance,LPCSTR lpszClassName,LPCSTR lpszWindowName,int width,int height,bool windowed,D3DDEVTYPE deviceType){
    this->width=width;
    this->height=height;
    window.Init(hInstance,lpszClassName,lpszWindowName,width,height);
    d3d.Init(hInstance,width,height,windowed,deviceType,window);
    inited=1;
    return 1;
}
bool SApplication::CreateScence(){
    D3DXVECTOR3 postion(0.0f,0.0f,-3.0f);
    D3DXVECTOR3 target(0.0f,0.0f,0.0f);
    D3DXVECTOR3 up(0.0f,1.0f,0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V,&postion,&target,&up);
    d3d.getDevice()->SetTransform(D3DTS_VIEW,&V);
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj,D3DX_PI*0.5f,(float)width/(float)height,1.0f,1000.0f);
    d3d.getDevice()->SetTransform(D3DTS_PROJECTION,&proj);
    d3d.getDevice()->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
    return true;
}
void SApplication::tick(float deltaTime){
    for(auto X:allObject){
        if(X->needTick)
        X->tick(deltaTime);
    }
}
void SApplication::Render(){
    getDevice()->Clear(0,0,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0xffffffff,1.0f,0);
    getDevice()->BeginScene();
    for(auto X:allObject){
        if(X->needRender)
        X->Render();
    }
    getDevice()->EndScene();
    getDevice()->Present(0,0,0,0);
}
class OTeapot:public OBase{
    public:
    ID3DXMesh* teapot=0;
    D3DXMATRIX Model;
    float rot_y=0;
    OTeapot(SApplication* p):OBase(p){
        D3DXCreateTeapot(p->getDevice(),&teapot,0);
    }
    virtual ~OTeapot(){
        if(teapot){
            teapot->Release();
        }
    }
    virtual void tick(float deltaTime){
        D3DXMatrixRotationY(&Model,rot_y);
        rot_y+=deltaTime/1000;
        if(rot_y>6.28) rot_y=0;
    }
    virtual void Render(){
        parent->getDevice()->SetTransform(D3DTS_WORLD,&Model);
        teapot->DrawSubset(0);
    }
};
OBase::OBase(SApplication* p):parent(p){
    id = parent->addObject(this);
}
OBase::~OBase(){
    parent->deleteObject(id);
};
void SWindow::msgLoop(){
            MSG msg;
            while(msg.message!=WM_QUIT){
                if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else {
                    float currTime=(float) timeGetTime();
                    tick(currTime-lastTime);
                    lastTime=currTime;
                    parent->Render();
                }
            }
        }
void SWindow::tick(float deltaTime){
            parent->tick(deltaTime);
            return;
        }
class SVertex{
    public:
        float x,y,z;
        SVertex(){};
        SVertex(float x1,float y1,float z1){
            x=x1;
            y=y1;
            z=z1;
        };
        static const DWORD FVF = D3DFVF_XYZ;
};
class SColorVertex:public SVertex{
    public:
        SColorVertex(){};
        SColorVertex(float x1,float y1,float z1,D3DCOLOR color1):SVertex(x1,y1,z1){
            color=color1;
        };
        D3DCOLOR color;
        static const DWORD FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE;
};
class STextureVertex:public SVertex{
    public:
        STextureVertex(){}
        STextureVertex(float x1,float y1,float z1,float u1,float v1):SVertex(x1,y1,z1){
            u=u1;
            v=v1;
        }
        float u,v;
        static const DWORD FVF=D3DFVF_XYZ|D3DFVF_TEX1;
};
class SNormalVertex:public SVertex{
    public:
        SNormalVertex(){}
        SNormalVertex(float x1,float y1,float z1,float nx1,float ny1,float nz1);
        float nx,ny,nz;
        static const DWORD FVF=D3DFVF_XYZ|D3DFVF_NORMAL;
};
SNormalVertex::SNormalVertex(float x1,float y1,float z1,float nx1,float ny1,float nz1):SVertex(x1,y1,z1){
    nx=nx1;
    ny=ny1;
    nz=nz1;
};
#endif