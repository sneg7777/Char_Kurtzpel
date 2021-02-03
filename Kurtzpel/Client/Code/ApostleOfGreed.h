#ifndef ApostleOfGreed_h__
#define ApostleOfGreed_h__

#include "Monster.h"


BEGIN(Client)
class CApostleOfGreed : public CMonster
{
public:
	enum State {
		State_Wait, State_Move, State_Rocate, State_Skill,
		State_JumpEnd,
		State_KnockBack, State_Groggy, State_GroggyUp,
		State_End
	};
	enum bCheck {
		bCheck_Start, bCheck_Sound1, bCheck_End
	};
	enum TimeCheck {
		TimeCheck_Dash, TimeCheck_End
	};
	enum Skill_Ap {
		Skill_Ap_None, Skill_Ap_1, Skill_Ap_3, Skill_Ap_5, Skill_Ap_7, Skill_Ap_10, Skill_Ap_End
	};
	enum SKill_Cool_Ap {
		SCool_Ap_1, SCool_Ap_3, SCool_Ap_5, SCool_Ap_7, SCool_Ap_10, SCool_Ap_10_2, SCool_Ap_Total, SCool_Ap_End
	};
	enum ApSound {
		ApSound_AttackVoice, ApSound_Dead, ApSound_Groggy, ApSound_GroggyUp, ApSound_Skill01, ApSound_Skill03, ApSound_Skill05, ApSound_Skill07,
		ApSound_Start, ApSound_Circle, ApSound_Thunder, ApSound_End
	};
private:
	explicit CApostleOfGreed(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CApostleOfGreed(void);

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
	void		Set_StateToAnimation(State _state, Skill_Ap _skill = Skill_Ap_None);
	void		Pattern(_float fTimeDelta);
	virtual void	 Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);
	void		Event_Skill(float fTimeDelta, Engine::CNaviMesh* pNaviMeshCom, _vec3 vPos, _vec3 vDir, float playerTodisTance);
	bool		Random_Skill(float playerTodisTance);
	void		Emplace_DelayHpDec(float _attack);
	void		Update_DelayHpDec(float fTimeDelta);
	void		ApSound_RandomPlay(ApSound _voice, float _addVolume = 1.f);

	float						m_TimeCheck[TimeCheck::TimeCheck_End];
	
public:
	bool						m_bCheck[bCheck::bCheck_End];
	State						m_State =  State::State_Wait;
	Skill_Ap					m_SkillState = Skill_Ap::Skill_Ap_None;
	bool						m_StateReady = false;
	float						m_SkillCool[SCool_Ap_End];
	_vec3						m_PlayerPosSave;
	_vec3						m_PlayerDistanceSave;
	_vec3						m_SkillPosSave;
public:
	static CApostleOfGreed*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif // Player_h__
