#ifndef Tree_h__
#define Tree_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CStaticMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;
class COptimization;
class CShader;

END

BEGIN(Client)
class CTree : public Engine::CGameObject
{
private:
	explicit CTree(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTree(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		Add_Component(void);
	void		SetUp_OnTerrain(void);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:

	Engine::CStaticMesh* m_pMeshCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CRenderer* m_pRendererCom = nullptr;
	Engine::CCalculator* m_pCalculatorCom = nullptr;
	Engine::CShader* m_pShaderCom = nullptr;

public:
	static CTree* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif // Tree_h__
