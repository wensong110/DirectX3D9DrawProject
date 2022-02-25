#include<windows.h>
#include<iostream>
#include"D3D.h"
#include"object.h"
#include"util.h"
using namespace std;
struct Cube:public OVertexSet<SNormalVertex>{
    float rot=0;
    Cube(SApplication* p):OVertexSet<SNormalVertex>(p){};
    virtual void tick(float deltaTime){
        rot+=deltaTime/1000;
        if(rot>6.28) rot=0;
        SetRotation(D3DXVECTOR3(1,rot,0));
    }
};
int WinMain(HINSTANCE hInstance,HINSTANCE hPreInstance,LPSTR args,int nCmd){
    SApplication a;
    a.Init(hInstance,"test","test",1280,720,true,D3DDEVTYPE_HAL);
    a.CreateScence();
    Cube x(&a);
    x.CreateBuffer(8);
    OVertexSet<SNormalVertex>::vertex_pointer_type v;
    x.getVertexBuffer()->Lock(0,0,(void**)&v,0);
    v[0]=SNormalVertex(-0.25,-0.25,0.25,-0.5,-0.5f,-0.5f);
    v[1]=SNormalVertex(0.25,-0.25,0.25,0.5,-0.5f,-0.5f);
    v[2]=SNormalVertex(-0.25,0.25,0.25,-0.5,0.5f,-0.5f);
    v[3]=SNormalVertex(0.25,0.25,0.25,0.5,0.5f,-0.5f);
    v[4]=SNormalVertex(-0.25,-0.25,-0.25,-0.5,-0.5f,0.5f);
    v[5]=SNormalVertex(0.25,-0.25,-0.25,0.5,-0.5f,0.5f);
    v[6]=SNormalVertex(-0.25,0.25,-0.25,-0.5,0.5f,0.5f);
    v[7]=SNormalVertex(0.25,0.25,-0.25,0.5,0.5f,0.5f);
    x.getVertexBuffer()->Unlock();
    x.CreateIndexBuffer(36);
    OVertexSet<SNormalVertex>::index_pointer_type i;
    x.getIndexBuffer()->Lock(0,0,(void**)&i,0);
    int ok[]={0,2,1,2,3,1,4,5,6,6,5,7,0,4,6,6,2,0,1,3,5,3,7,5,2,6,3,6,7,3,0,1,4,4,1,5};
    for(int ii=0;ii<36;ii++){
        i[ii]=ok[ii];
    }
    x.getIndexBuffer()->Unlock();
    D3DMATERIAL9 material;
    material.Ambient=D3DXCOLOR(1,1,1,1);
    material.Diffuse=D3DXCOLOR(1,1,1,1);
    material.Specular=D3DXCOLOR(1,1,1,1);
    material.Emissive=D3DXCOLOR(0,0,0,1);
    material.Power=5.0f;
    a.getDevice()->SetMaterial(&material);
    auto light_color=D3DXCOLOR(0,0,1,1);
    OPointLight light(&a,1,D3DXVECTOR3(0,0,-0.5),light_color,light_color*0.25f,light_color*0.6f,5);
    a.run();
    return 0;
}