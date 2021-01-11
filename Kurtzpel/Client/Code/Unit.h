#ifndef Unit_h__
#define Unit_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CDynamicMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;
//class COptimization;
class CNaviMesh;
class CShader;

END
#include "SphereCollider.h"
//class CSphereCollider;
//class CSphereCollider::BonePart;
BEGIN(Client)
class CUnit : public Engine::CGameObject
{
public:
	explicit CUnit(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUnit(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;
	virtual void Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);
	CSphereCollider*	Get_BonePartCollider(CSphereCollider::BonePart _bonePart);
	void Set_BonePartColliderAttack(CSphereCollider::BonePart _bonePart, bool _attack, float _power = 1.f);

	void Set_StaticSphere();
protected:
	HRESULT		Add_Component(void);
	void		SetUp_OnTerrain(void);
	_bool		Collision_ToObject(const _tchar* pLayerTag, const _tchar* pObjTag);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
public:

	Engine::CStaticMesh*		m_pStaticMeshCom = nullptr;
	Engine::CTransform*			m_pTransformCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CCalculator*		m_pCalculatorCom = nullptr;
	Engine::CCollider*			m_pColliderCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;

	//_bool						m_bColl = false;
	//_bool						m_bDraw = false;
	_float						m_fMaxKnockBackHp = 0.f;
	_float						m_fKnockBackHp = 0.f;
	_float						m_fKnockBackPower = 1.f;
	_vec3						m_fKnockBackDir;

	//////////////
	
	//static CUnit_D* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	vector<CSphereCollider*>		m_VecSphereCollider;
	
protected:
	virtual void Free(void) override;

};

END
#endif
