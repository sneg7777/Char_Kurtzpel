#ifndef Player_h__
#define Player_h__

#include "Unit_D.h"
#include "Define.h"

BEGIN(Engine)

class CDynamicMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;
class CNaviMesh;
class CShader;

END
BEGIN(Client)
class CHammer;
class CLongBow;
class CPlayer : public CUnit_D
{
public:
	static CPlayer* CPlayer::GetInstance(void) {
		if (nullptr == m_pInstance) {
			return nullptr;
		}
		return m_pInstance;	
	}
	static void CPlayer::DestroyInstance(void) {
		if (nullptr != m_pInstance) {
			delete m_pInstance;
			m_pInstance = NULL;
		}					
	}
	static CPlayer* m_pInstance;
public:
	enum State {
		State_Idle, State_Move, State_MoveSA, State_MoveSD, State_Dash, State_Attack, State_Skill,
		State_JumpEnd, State_JumpComboEnd,
		State_Damaged, State_DamagedDown, State_Rolling,
		State_End
	};
	enum Attack_State {
		StateA_None, StateA_Basic1, StateA_Basic2, StateA_Basic3, StateA_Basic4,
		StateA_SkillQ, StateA_SkillE, StateA_SkillF, StateA_SkillZ,
		StateA_End
	};
	enum JumpIdleAni {
		JumpIdle_None, JumpIdle_JumpStart, JumpIdle_JumpUp, JumpIdle_JumpDown, JumpIdle_End
	};
	enum bCheck {
		bCheck_DBKeyW, bCheck_DBKeyA, bCheck_DBKeyS, bCheck_DBKeyD,
		bCheck_KeyW, bCheck_KeyA, bCheck_KeyS, bCheck_KeyD, bCheck_KeyF2, bCheck_RenderSphere,
		bCheck_MouseL, bCheck_MouseR, bCheck_MouseL_Already, bCheck_MouseR_Already,
		bCheck_Skill_F1, bCheck_Skill_F2, bCheck_LB_SkillE, bCheck_LB_Phoenix_SkillZ,
		bCheck_DamagedUp, bCheck_JumpToDamagedUp/*약한충격에도 점프상태에서 다운이 되고 그대로 착지후에도 호완성을 위해*/,
		bCheck_End
	};
	enum TimeCheck {
		TimeCheck_Dash, TimeCheck_KeyW, TimeCheck_KeyA, TimeCheck_KeyS, TimeCheck_KeyD,
		TimeCheck_Cool_Q, TimeCheck_Cool_E, TimeCheck_Cool_F, TimeCheck_Cool_Tab,
		TimeCheck_LB_SkillE_Ready, TimeCheck_LB_SkillE_Attack/*마법진시전*/, TimeCheck_LB_SkillE_Attack_Arrow/*위로쏘는캐릭터모션*/, TimeCheck_LB_SkillE_Attack_Arrow_Start/*화살내려앉는*/, TimeCheck_LB_SkillE_Attack_Arrow_Start_Total,
		TimeCheck_Invin,
		TimeCheck_End
	};
	enum Weapon_Equip {
		Weapon_None, Weapon_Hammer, Weapon_LongBow, Weapon_End
	};
	enum AnimationClip {
		Ani_1, Ani_2, Ani_3, Ani_4, Ani_5, Ani_End
	};
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

public:
	void		Add_LookAtY(float lookat);

private:
	HRESULT		Add_Component(void);
	void		Set_StateToAnimation(State _state, _vec3 _vPos = { 0.f, 0.f, 0.f }, _vec3 _vDir = { 0.f, 0.f, 0.f }, float fTimeDelta = 0.f);
	void		Set_StateToAnimation_Jump(State _state, _vec3 _vPos = { 0.f, 0.f, 0.f }, _vec3 _vDir = { 0.f, 0.f, 0.f }, float fTimeDelta = 0.f);
	void		Hammer_Key_Input(const _float& fTimeDelta);
	void		LongBow_Key_Input(const _float& fTimeDelta);
	void		Hammer_Key_InputOfJump(const _float& fTimeDelta);
	void		LongBow_Key_InputOfJump(const _float& fTimeDelta);
	void		Calc_Time(_float fTimeDelta);
	void		Key_DoubleInput(const _float& fTimeDelta);
	void		Jump_Control(const _float& fTimeDelta);
	void		Event_Skill(float fTimeDelta, Engine::CNaviMesh* pNaviMeshCom, _vec3 vPos, _vec3 vDir);
	virtual void		Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);
	void				Set_RenderCollSphere();
	void		Change_Weapon();
	void		Create_ArrowShot(_vec3 _vPos, _vec3 _vDir, float fTimeDelta);
	void		Create_ArrowShot_SkillF(_vec3 _vPos, _vec3 _vDir, float fTimeDelta);
	void		Create_ArrowShot_SkillE_Ready(_vec3 _vPos, _vec3 _vDir);
	void		Create_ArrowShot_SkillE_Start();
	void		Create_ArrowShot_SkillQ(_vec3 _vPos, _vec3 _vDir);
	void		Create_Phoenix_SkillZ(_vec3 _vPos, _vec3 _vDir);
	void		Move_AccelSpeed(_vec3 vPos, _vec3 vDir, float fTimeDelta, Engine::CNaviMesh* pNaviMeshCom);
	_vec3		PickUp_OnTerrain(void);

private:
	//점프
	_float						m_fJumpPower = 0.12f;
	_float						m_fJumpAccel = 0.f;
public:
	_float						m_LookAtY = 2.5f;
	_float						m_CameraDist = 320.f;
	float						m_TimeCheck[TimeCheck::TimeCheck_End];
	bool						m_bCheck[bCheck::bCheck_End];
	State						m_State =  State::State_Idle;
	Attack_State				m_Attack_State = Attack_State::StateA_None;
	JumpIdleAni					m_JumpIdleState = JumpIdleAni::JumpIdle_None;
	//애니메이션 Start Loop End 로 나누어져있는거 순서로 이용할려고
	AnimationClip				m_AniClip = AnimationClip::Ani_End;
	//무기
	Weapon_Equip				m_WeaponEquip;
	CHammer*					m_Hammer = nullptr;
	CLongBow*					m_LongBow =  nullptr;
	float						m_RocateY = 0.f;
	_vec3						m_LB_SkillE_Pos = { 0.f, 0.f, 0.f };
	//_vec3						m_SkillPosSave = { 0.f, 0.f, 0.f };
	int							m_LB_Arrow_Count = 0;
	float						m_AccelSpeed = 0.f;
	float						m_AccelSpeedInit = 0.f;
	float						m_MaxDashGauge = 0.f;
	float						m_DashGauge = 0.f;
public:
	static CPlayer*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;


	//Char
};

END
#endif // Player_h__
