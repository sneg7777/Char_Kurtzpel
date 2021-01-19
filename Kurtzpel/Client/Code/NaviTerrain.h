#ifndef NaviTerrain_h__
#define NaviTerrain_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRenderer;
class CCalculator;
class CCollider;
class CNaviMesh;
class CShader;
class CTransform;

END
BEGIN(Client)
class CHammer;
class CNaviTerrain : public Engine::CGameObject
{
private:
	explicit CNaviTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CNaviTerrain(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		Add_Component(void);

public:
	Engine::CNaviMesh* m_pNaviMeshCom = nullptr;
	Engine::CRenderer* m_pRendererCom = nullptr;

public:
	static CNaviTerrain*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif
