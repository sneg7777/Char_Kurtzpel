#ifndef ApostleOfGreed_h__
#define ApostleOfGreed_h__

#include "Monster.h"


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


	float						m_TimeCheck[TimeCheck::TimeCheck_End];
	
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
