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
		State_Idle, State_Move, State_MoveSA, State_MoveSD, State_Dash, State_Attack,
		State_JumpEnd,
		State_Damaged,
		State_End
	};
	enum State_Attack {
		StateA_Basic1, StateA_Basic2, StateA_Basic3, StateA_End
	};
	enum JumpIdleAni {
		JumpIdle_None, JumpIdle_JumpStart, JumpIdle_JumpUp, JumpIdle_JumpDown, JumpIdle_End
	};
	enum bCheck {
		bCheck_DBKeyW, bCheck_DBKeyA, bCheck_DBKeyS, bCheck_DBKeyD,
		bCheck_KeyW, bCheck_KeyA, bCheck_KeyS, bCheck_KeyD, bCheck_End
	};
	enum TimeCheck {
		TimeCheck_Dash, TimeCheck_KeyW, TimeCheck_KeyA, TimeCheck_KeyS, TimeCheck_KeyD,
		TimeCheck_Invin,
		TimeCheck_End
	};
	enum Weapon_Equip {
		Weapon_Hammer, Weapon_Bow, Weapon_End
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
	void		Set_StateToAnimation(State _state);
	void		Key_Input(const _float& fTimeDelta);
	void		Key_InputOfJump(const _float& fTimeDelta);
	void		Calc_Time(_float fTimeDelta);
	void		Key_DoubleInput(const _float& fTimeDelta);
	void		Jump_Control(const _float& fTimeDelta);
	void		Collision(Engine::CGameObject* _col);
	_vec3		PickUp_OnTerrain(void);

private:
	float						m_TimeCheck[TimeCheck::TimeCheck_End];
	//점프
	_float						m_fJumpPower = 0.12f;
	_float						m_fJumpAccel = 0.f;
public:
	_float						m_LookAtY = 2.5f;
	_float						m_CameraDist = 300.f;
	bool						m_bCheck[bCheck::bCheck_End];
	State						m_State =  State::State_Idle;
	JumpIdleAni					m_JumpIdleState = JumpIdleAni::JumpIdle_None;

	//무기
	Weapon_Equip				m_WeaponEquip;
	CHammer*					m_Hammer;
public:
	static CPlayer*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;


	//Char
};

END
#endif // Player_h__
