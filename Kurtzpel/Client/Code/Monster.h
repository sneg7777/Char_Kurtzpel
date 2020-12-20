#ifndef Monster_h__
#define Monster_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CDynamicMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;
class COptimization;
class CShader;

END

class CSphereCollider;
BEGIN(Client)
class CMonster : public Engine::CGameObject
{
public:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		Add_Component(void);
	void		SetUp_OnTerrain(void);
	_bool		Collision_ToObject(const _tchar* pLayerTag, const _tchar* pObjTag);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
public:

	Engine::CDynamicMesh* m_pMeshCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CRenderer* m_pRendererCom = nullptr;
	Engine::CCalculator* m_pCalculatorCom = nullptr;
	Engine::CCollider* m_pColliderCom = nullptr;
	Engine::COptimization* m_pOptimizationCom = nullptr;
	Engine::CShader* m_pShaderCom = nullptr;
	_bool						m_bColl = false;
	_bool						m_bDraw = false;

public:
	static CMonster* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	vector<CSphereCollider*>		m_VecSphereCollider;
private:
	virtual void Free(void) override;

};

END
#endif // Stone_h__
