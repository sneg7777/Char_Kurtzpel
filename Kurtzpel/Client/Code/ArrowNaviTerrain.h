#ifndef ArrowNaviTerrain_h__
#define ArrowNaviTerrain_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRenderer;
class CCalculator;
class CCollider;
class CArrowNaviMesh;
class CShader;
class CTransform;

END
BEGIN(Client)
class CHammer;
class CArrowNaviTerrain : public Engine::CGameObject
{
private:
	explicit CArrowNaviTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CArrowNaviTerrain(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		Add_Component(void);

public:
	Engine::CArrowNaviMesh* m_pArrowNaviMeshCom = nullptr;
	Engine::CRenderer* m_pRendererCom = nullptr;

public:
	static CArrowNaviTerrain*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif
