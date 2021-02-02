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
	enum ShotCheck {
		ShotCheck_None, ShotCheck_1, ShotCheck_2, ShotCheck_3, ShotCheck_End
	};
	enum Skill_CB {
		Skill_CB_None, Skill_CB_1, Skill_CB_2, Skill_CB_3, Skill_CB_3_2, Skill_CB_3_3, Skill_CB_End
	};
	enum SKill_Cool_CB {
		SCool_CB_1, SCool_CB_2, SCool_CB_3, SCool_CB_Total, SCool_CB_End
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
	void		Set_StateToAnimation(State _state, Skill_CB _skill = Skill_CB_None);
	void		Pattern(_float fTimeDelta);
	virtual void	 Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);
	void		Event_Skill(float fTimeDelta, Engine::CNaviMesh* pNaviMeshCom, _vec3 vPos, _vec3 vDir, float playerTodisTance);
	bool		Random_Skill(float playerTodisTance);
	void		Emplace_DelayHpDec(float _attack);
	void		Update_DelayHpDec(float fTimeDelta);
	void		Create_ArrowShot(_vec3 _vPos, float fTimeDelta);

	void		Sound_RandomPlayer_Arrow();



	//float						m_TimeCheck[TimeCheck::TimeCheck_End];
	
public:
	State						m_State;
	Skill_CB					m_SkillState = Skill_CB::Skill_CB_None;
	//bool						m_StateReady = false;
	float						m_SkillCool[SCool_CB_End];
	ShotCheck					m_ShotCheck;
public:
	static CMonster2_CrossBow*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif
