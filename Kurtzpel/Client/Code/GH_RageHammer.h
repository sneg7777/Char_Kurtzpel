#ifndef GH_RageHammer_h__
#define GH_RageHammer_h__

#include "Unit_D.h"


BEGIN(Client)
class CGH_RageHammer : public CUnit_D
{
private:
	explicit CGH_RageHammer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGH_RageHammer(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual _int LateUpdate_Object(const _float& fTimeDelta);
	virtual void Render_Object(void) override;

public:

private:
	HRESULT		Add_Component();
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	void		SetUp_OnTerrain(void);
	void		Calc_Time(_float fTimeDelta);
	void		Pattern(_float fTimeDelta);
	virtual void	 Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);

public:
	static CGH_RageHammer*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;
	float		m_Dissolve = 0.f;
	bool		m_RockHit = false;
};

END
#endif // Player_h__
