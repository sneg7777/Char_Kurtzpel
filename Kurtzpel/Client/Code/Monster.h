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
//class COptimization;
class CNaviMesh;
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

protected:
	HRESULT		Add_Component(void);
	void		SetUp_OnTerrain(void);
	_bool		Collision_ToObject(const _tchar* pLayerTag, const _tchar* pObjTag);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
public:

	Engine::CDynamicMesh*		m_pMeshCom = nullptr;
	Engine::CNaviMesh*			m_pNaviMeshCom = nullptr; 
	Engine::CTransform*			m_pTransformCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CCalculator*		m_pCalculatorCom = nullptr;
	Engine::CCollider*			m_pColliderCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;

	//_bool						m_bColl = false;
	//_bool						m_bDraw = false;

	_vec3						m_vDir;
	_float						m_fInitSpeed = 10.f;
	_float						m_fSpeed = 7.5f;
	_float						m_AniSpeed = 1.f;
	_float						m_AniTime = 0.f;
public:
	_int Load_ColliderFile(_tchar* pFilePath);
	
	static CMonster* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	vector<CSphereCollider*>		m_VecSphereCollider;
protected:
	virtual void Free(void) override;

};

END
#endif // Stone_h__
