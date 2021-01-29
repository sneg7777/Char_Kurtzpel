#ifndef GH_RockIn_h__
#define GH_RockIn_h__

#include "Unit_D.h"


BEGIN(Client)
class CGH_RockIn : public CUnit_D
{
private:
	explicit CGH_RockIn(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGH_RockIn(void);

public:
	virtual HRESULT Ready_Object(void) override;
	HRESULT Ready_Object(bool _inCheck);
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual _int LateUpdate_Object(const _float& fTimeDelta);
	virtual void Render_Object(void) override;

public:

private:
	HRESULT		Add_Component(bool _inCheck);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	void		SetUp_OnTerrain(void);
	void		Calc_Time(_float fTimeDelta);
	void		Pattern(_float fTimeDelta);
	virtual void	 Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);

public:
	static CGH_RockIn*	Create(LPDIRECT3DDEVICE9 pGraphicDev, bool _inCheck);

private:
	virtual void Free(void) override;

};

END
#endif // Player_h__
