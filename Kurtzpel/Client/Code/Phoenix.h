#ifndef Phoenix_h__
#define Phoenix_h__

#include "Unit_D.h"


BEGIN(Client)
class CPhoenix : public CUnit_D
{
private:
	explicit CPhoenix(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPhoenix(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual _int LateUpdate_Object(const _float& fTimeDelta);
	virtual void Render_Object(void) override;

public:

private:
	HRESULT		Add_Component(void);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	void		Calc_Time(_float fTimeDelta);
	void		Pattern(_float fTimeDelta, _vec3 vPos);
	virtual void	 Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);
	
public:
	static CPhoenix*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

	float	m_LifeTime = 0.f;

	float m_Attack = 0.f;
	float m_IntervalTime = 0.f;

private:
	virtual void Free(void) override;

	bool	m_AniClip = false;
};

END
#endif // Player_h__
