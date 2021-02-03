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
class CTexture;

END
#include "SphereCollider.h"
//class CSphereCollider;
//class CSphereCollider::BonePart;
BEGIN(Client)
struct sComponent {
	Engine::CDynamicMesh* m_pMeshCom = nullptr;
	Engine::CStaticMesh* m_pStaticMeshCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CRenderer* m_pRendererCom = nullptr;
	Engine::CCalculator* m_pCalculatorCom = nullptr;
	Engine::CCollider* m_pColliderCom = nullptr;
	Engine::CShader* m_pShaderCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
};
enum RandomSound {
	Sound_AttackVoice, Sound_DamagedVoice, Sound_SkillVoice, Sound_Arrow_Hit, Sound_Arrow, Sound_BowTwang, Sound_Dash,
	Sound_GHAttack, Sound_GHHit, Sound_Jump, Sound_Phoenix, Sound_Run, Sound_MonDamaged, Sound_End
};
struct sDelayHpDec {
	_float		m_fDelayTime;
	_float		m_fHpDec;
};
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
	void Set_BonePartCollider_Attack(CSphereCollider::BonePart _bonePart, bool _attackCheck, float _attack);
	void Set_BonePartColliderAttack(CSphereCollider::BonePart _bonePart, float _fattack, bool _battack, float _power = 1.f);

	void Set_StaticSphere();
	sComponent* Get_sComponent() { return &m_sComponent; }
protected:
	HRESULT		Add_Component(void);
	void		SetUp_OnTerrain(void);
	_bool		Collision_ToObject(const _tchar* pLayerTag, const _tchar* pObjTag);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	void		Sound_RandomPlay(RandomSound _voice, float _addVolume = 1.f);
protected:
	sComponent					m_sComponent;

public:
	//_bool						m_bColl = false;
	//_bool						m_bDraw = false;

	//////////////
	
	//static CUnit_D* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	vector<CSphereCollider*>		m_VecSphereCollider;
	vector<sDelayHpDec*>			m_VecDelayHpDec;
	
protected:
	virtual void Free(void) override;

};

END
#endif
