#ifndef Monster2_CrossBow_h__
#define Monster2_CrossBow_h__

#include "Monster.h"


BEGIN(Client)
class CMonster2_CrossBow : public CMonster
{
public:
	enum State {
		State_Wait, State_Move, State_Rocate, State_Skill,
		State_JumpEnd,
		State_Damaged, State_KnockBack, State_Groggy, State_GroggyUp,
		State_Respawn, State_Dead, State_DeadDown,
		State_End
	};
	enum TimeCheck {
		TimeCheck_End
	};
	enum Skill_TH {
		Skill_TH_None, Skill_TH_1, Skill_TH_2, Skill_TH_3, Skill_TH_End
	};
	enum SKill_Cool_TH {
		SCool_TH_1, SCool_TH_2, SCool_TH_3, SCool_TH_Total, SCool_TH_End
	};
private:
	explicit CMonster2_CrossBow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster2_CrossBow(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual _int LateUpdate_Object(const _float& fTimeDelta);
	virtual void Render_Object(void) override;

public:

private:
	HRESULT		Add_Component(void);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	void		SetUp_OnTerrain(void);
	void		Calc_Time(_float fTimeDelta);
	void		Init_BoneAttack();
	void		Set_StateToAnimation(State _state, Skill_TH _skill = Skill_TH_None);
	void		Pattern(_float fTimeDelta);
	virtual void	 Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);
	void		Event_Skill(float fTimeDelta, Engine::CNaviMesh* pNaviMeshCom, _vec3 vPos, _vec3 vDir, float playerTodisTance);
	bool		Random_Skill(float playerTodisTance);
	void		Emplace_DelayHpDec(float _attack);
	void		Update_DelayHpDec(float fTimeDelta);


	//float						m_TimeCheck[TimeCheck::TimeCheck_End];
	
public:
	State						m_State;
	Skill_TH					m_SkillState = Skill_TH::Skill_TH_None;
	//bool						m_StateReady = false;
	float						m_SkillCool[SCool_TH_End];
public:
	static CMonster2_CrossBow*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif
