#ifndef Monster2_Arrow_h__
#define Monster2_Arrow_h__

#include "Unit.h"
#include "Define.h"

BEGIN(Engine)

class CStaticMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;
class CShader;

END

BEGIN(Client)
class CMonster2_Arrow : public CUnit {
private:
	explicit CMonster2_Arrow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster2_Arrow(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;
	//
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	virtual void Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);
	void Create_Coll();
	void Set_SpeedToLife(float _speed, float _life = -1.f);
	//
private:
	HRESULT		Add_Component(void);

private:
	float		m_LifeTime = 0.f;

public:
	static CMonster2_Arrow*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif // Sword_h__
