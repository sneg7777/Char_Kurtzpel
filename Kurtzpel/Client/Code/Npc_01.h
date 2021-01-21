#ifndef Npc_01_h__
#define Npc_01_h__

#include "Unit_D.h"


BEGIN(Client)
class CNpc_01 : public CUnit_D
{
public:
	enum State_Npc {
		State_Wait, State_Move, State_Rocate, State_Skill,
		State_JumpEnd,
		State_KnockBack, State_Groggy, State_GroggyUp,
		State_End
	};
private:
	explicit CNpc_01(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CNpc_01(void);

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
	virtual void	 Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);

public:
	static CNpc_01*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

	

	//Char
private:
	bool m_AniEnd = true;
};

END
#endif
