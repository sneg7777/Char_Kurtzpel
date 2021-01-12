#ifndef SkillCollider_h__
#define SkillCollider_h__

#include "Unit.h"
#include "Define.h"

BEGIN(Engine)

class CStaticMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;
class CShader;

END

BEGIN(Client)
class CSkillCollider : public CUnit {
private:
	explicit CSkillCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSkillCollider(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;
	//
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	virtual void Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);
	//_pos : 위치
	//_scale : 크기(float)
	//_team : 팀
	//_attack : 공격력
	// _lifeTime = 살아있을시간
	// _interval = 새롭게 때리는 주기/시간  // Default = 한번만 때림
	// _interval_Attack = 새롭게 때리고 데미지판정 시간 // Default = 새롭게 때릴때까지 쭉 판정
	// _startTime = 생성하고 언제부터 때릴건지. // Default = 0.f 바로
	void Set_Collider(_vec3 _pos, float _scale, CSphereCollider::BoneTeam _team, float _attack, float _lifeTime, float _interval = 0.f, float _interval_AttackTime = -1.f, float _startTime = 0.f);
	//
private:
	HRESULT		Add_Component(void);

private:
	float		m_LifeTime = 0.f;
	float		m_IntervalTime = 0.f;
	float		m_Interval = 0.f;
	float		m_Interval_Attack = 0.f;
	float		m_Interval_AttackTime = 0.f;
	float		m_StartTime = 0.f;
public:
	static CSkillCollider*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif // Sword_h__
