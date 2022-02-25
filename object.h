#ifndef HEADER_OBG
#define HEADER_OBG
#include"D3D.h"
class OActorBase:public OBase{
    public:
    OActorBase(SApplication* p);
    virtual ~OActorBase(){};
    virtual void tick(float deltaTime)=0;
    virtual void Render()=0;
    void SetScale(D3DXVECTOR3 scale1);
    void SetTranslation(D3DXVECTOR3 translate1);
    void SetRotation(D3DXVECTOR3 rotate);
    void SetHierarchy(D3DXVECTOR3* scale,D3DXVECTOR3* translate,D3DXVECTOR3* rotate,D3DXMATRIX* parent_transform);
    D3DXVECTOR3 vScale;
    D3DXVECTOR3 vTranslate;
    D3DXVECTOR3 vRotate;
    protected:
        D3DXMATRIX mTransform;
        D3DXMATRIX mScale;
        D3DXMATRIX mTranslate;
        D3DXMATRIX mRotation;
};
OActorBase::OActorBase(SApplication* p):OBase(p){
    vScale=D3DXVECTOR3(1.0f,1.0f,1.0f);
    vTranslate=D3DXVECTOR3(0.0f,0.0f,0.0f);
    vRotate=D3DXVECTOR3(0.0f,0.0f,0.0f);
    D3DXMatrixScaling(&mScale,vScale.x,vScale.y,vScale.z);
    D3DXMatrixTranslation(&mTranslate,vTranslate.x,vTranslate.y,vTranslate.z);
    D3DXMatrixRotationYawPitchRoll(&mRotation,vRotate.x,vRotate.y,vRotate.z);
    mTransform=mScale*mRotation*mTranslate;
};
void OActorBase::SetScale(D3DXVECTOR3 scale1){
    vScale=scale1;
    D3DXMatrixScaling(&mScale,vScale.x,vScale.y,vScale.z);
    mTransform=mScale*mRotation*mTranslate;
}
void OActorBase::SetRotation(D3DXVECTOR3 rotation1){
    vRotate=rotation1;
    D3DXMatrixRotationYawPitchRoll(&mRotation,vRotate.x,vRotate.y,vRotate.z);
    mTransform=mScale*mRotation*mTranslate;
}
void OActorBase::SetTranslation(D3DXVECTOR3 translate1){
    vTranslate=translate1;
    D3DXMatrixTranslation(&mTranslate,vTranslate.x,vTranslate.y,vTranslate.z);
    mTransform=mScale*mRotation*mTranslate;
}
void OActorBase::SetHierarchy(D3DXVECTOR3* scale,D3DXVECTOR3* translate,D3DXVECTOR3* rotate,D3DXMATRIX* parent_transform){
    if(scale){
        SetScale(*scale);
    }
    if(translate){
        SetTranslation(*translate);
    }
    if(rotate){
        SetRotation(*rotate);
    }
    mTransform=mTransform*(*parent_transform);
};
template<typename VertexType>
class OVertexSet:public OActorBase{
    public:
        typedef VertexType* vertex_pointer_type;
        typedef WORD* index_pointer_type;
        virtual void tick(float deltaTime);
        virtual void Render();
        OVertexSet(SApplication* p);
        bool CreateBuffer(size_t n);
        bool CreateIndexBuffer(size_t n);
        virtual ~OVertexSet();
        IDirect3DVertexBuffer9* getVertexBuffer(){
            return vb;
        };
        IDirect3DIndexBuffer9* getIndexBuffer(){
            return ib;
        }
    private:
        IDirect3DVertexBuffer9* vb=0;
        IDirect3DIndexBuffer9* ib=0;
        size_t numOfVertex;
        size_t numOfIndex;
};
template<typename VertexType>
OVertexSet<VertexType>::OVertexSet(SApplication* p):OActorBase(p){};
template<typename VertexType>
bool OVertexSet<VertexType>::CreateBuffer(size_t n){
    if(vb){
        vb->Release();
        vb=0;
    }
    numOfVertex=n;
    HRESULT hr = parent->getDevice()->CreateVertexBuffer(n*sizeof(VertexType),D3DUSAGE_WRITEONLY,VertexType::FVF,D3DPOOL_MANAGED,&vb,0);
    if(FAILED(hr)){
        return 0;
    }
    return 1;
}
template<typename VertexType>
bool OVertexSet<VertexType>::CreateIndexBuffer(size_t n){
    if(ib){
        ib->Release();
        ib=0;
    }
    numOfIndex=n;
    HRESULT hr = parent->getDevice()->CreateIndexBuffer(n*sizeof(WORD),D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&ib,0);
    if(FAILED(hr)){
        return 0;
    }
    return 1;
}
template<typename VertexType>
OVertexSet<VertexType>::~OVertexSet(){
    if(vb){
        vb->Release();
    }
    if(ib){
        ib->Release();
    }
}
template<typename VertexType>
void OVertexSet<VertexType>::tick(float deltaTime){

}
template<typename VertexType>
void OVertexSet<VertexType>::Render(){
    parent->getDevice()->SetStreamSource(0,vb,0,sizeof(VertexType));
    parent->getDevice()->SetIndices(ib);
    parent->getDevice()->SetFVF(VertexType::FVF);
    parent->getDevice()->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
    parent->getDevice()->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
    parent->getDevice()->SetTransform(D3DTS_WORLD,&mTransform);
    parent->getDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,numOfVertex,0,(numOfIndex/3));
}
template<>
void OVertexSet<SColorVertex>::Render(){
    parent->getDevice()->SetStreamSource(0,vb,0,sizeof(SColorVertex));
    parent->getDevice()->SetIndices(ib);
    parent->getDevice()->SetFVF(SColorVertex::FVF);
    parent->getDevice()->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
    parent->getDevice()->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
    parent->getDevice()->SetRenderState(D3DRS_LIGHTING,true);
    parent->getDevice()->SetTransform(D3DTS_WORLD,&mTransform);
    parent->getDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,numOfVertex,0,(numOfIndex/3));
}
template<>
void OVertexSet<STextureVertex>::Render(){
    parent->getDevice()->SetStreamSource(0,vb,0,sizeof(STextureVertex));
    parent->getDevice()->SetIndices(ib);
    parent->getDevice()->SetFVF(SColorVertex::FVF);
    parent->getDevice()->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
    parent->getDevice()->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
    parent->getDevice()->SetRenderState(D3DRS_LIGHTING,false);
    parent->getDevice()->SetTransform(D3DTS_WORLD,&mTransform);
    parent->getDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,numOfVertex,0,(numOfIndex/3));
}
class OPointLight:public OActorBase{
    private:
    public:
    D3DXVECTOR3 pos;
    D3DLIGHT9 light;
    int id;
    OPointLight(SApplication* p,int id1,D3DXVECTOR3 pos1,D3DXCOLOR amb,D3DXCOLOR diff,D3DXCOLOR spec,float range,float a0=1.0,
    float a1=1.0,float a2=1.0
    ):OActorBase(p){
        needTick=0;
        needRender=1;
        pos=pos1;
        D3DXVECTOR3 temp;
        D3DXVec3TransformCoord(&temp,&pos,&mTransform);
        light.Type=D3DLIGHT_POINT;
        light.Position=temp;
        light.Ambient=amb;
        light.Diffuse=diff;
        light.Specular=spec;
        light.Range=range;
        light.Attenuation0=a0;
        light.Attenuation1=a1;
        light.Attenuation2=a2;
        id=id1;
    }
    virtual void tick(float deltaTime){};
    virtual void Render(){
        parent->getDevice()->SetLight(id,&light);
        parent->getDevice()->LightEnable(id,true);
        needRender=0;
    };
    void reRender(){
        needRender=1;
    }
};
#endif