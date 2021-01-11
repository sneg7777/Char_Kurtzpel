#ifndef Arrow_h__
#define Arrow_h__

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
class CArrow : public CUnit {
private:
	explicit CArrow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CArrow(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;
	//
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	virtual void Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);
	//
private:
	HRESULT		Add_Component(void);

private:
	float		m_LifeTime = 0.f;
	float m_RocationX = 0.f;
	float m_RocationY = 0.f;
	float m_RocationZ = 0.f;
public:
	static CArrow*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif // Sword_h__
