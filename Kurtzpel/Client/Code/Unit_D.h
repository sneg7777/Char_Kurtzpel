#ifndef Unit_D_h__
#define Unit_D_h__

#include "Unit.h"
#include "Define.h"

BEGIN(Engine)

class CDynamicMesh;

END
#include "SphereCollider.h"
//class CSphereCollider;
//class CSphereCollider::BonePart;
BEGIN(Client)
class CUnit_D : public CUnit
{
public:
	explicit CUnit_D(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUnit_D(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;
	virtual void Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);

	void Init_BoneAttack(bool _attack = false);


protected:
	HRESULT		Add_Component(void);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
public:

	//_bool						m_bColl = false;
	//_bool						m_bDraw = false;
	_float						m_AniSpeed = 1.f;

public:
	_int Load_ColliderFile(_tchar* pFilePath);


	
	//static CUnit_D* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	
protected:
	virtual void Free(void) override;

};

END
#endif // Stone_h__
