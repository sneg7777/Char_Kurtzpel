#ifndef ApostleOfGreed_h__
#define ApostleOfGreed_h__

#include "GameObject.h"
#include "Monster.h"
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

class CApostleOfGreed : public CMonster
{
public:
	enum State {
		State_Idle, State_Move, State_Dash, State_Attack1,
		State_JumpEnd,
		State_End
	};
	enum bCheck {
		bCheck_Start, bCheck_End
	};
	enum TimeCheck {
		TimeCheck_Dash, TimeCheck_End
	};
private:
	explicit CApostleOfGreed(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CApostleOfGreed(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

public:

private:
	HRESULT		Add_Component(void);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	void		SetUp_OnTerrain(void);
	void		Calc_Time(_float fTimeDelta);
	_vec3		PickUp_OnTerrain(void);

private:
	Engine::CDynamicMesh*		m_pMeshCom = nullptr;
	Engine::CNaviMesh*			m_pNaviMeshCom = nullptr; 
	Engine::CTransform*			m_pTransformCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CCalculator*		m_pCalculatorCom = nullptr;
	Engine::CCollider*			m_pColliderCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	_vec3						m_vDir;

	_float						m_fInitSpeed = 10.f;
	_float						m_fSpeed = 7.5f;
	_float						m_LookAtY = 2.5f;
	_float						m_AniSpeed = 1.f;
	_float						m_CameraDist = 300.f;
	float						m_TimeCheck[TimeCheck::TimeCheck_End];
	//มกวม
	_float						m_fJumpPower = 0.2f;
	_float						m_fJumpAccel = 0.f;
	
	_float						m_AniTime = 0.f;
public:
	bool						m_bCheck[bCheck::bCheck_End];
	State						m_State =  State::State_Idle;
public:
	static CApostleOfGreed*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;


	//Char
private:
	bool m_AniEnd = true;
};

END
#endif // Player_h__
