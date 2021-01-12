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
	//_pos : ��ġ
	//_scale : ũ��(float)
	//_team : ��
	//_attack : ���ݷ�
	// _lifeTime = ��������ð�
	// _interval = ���Ӱ� ������ �ֱ�/�ð�  // Default = �ѹ��� ����
	// _interval_Attack = ���Ӱ� ������ ���������� �ð� // Default = ���Ӱ� ���������� �� ����
	// _startTime = �����ϰ� �������� ��������. // Default = 0.f �ٷ�
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
