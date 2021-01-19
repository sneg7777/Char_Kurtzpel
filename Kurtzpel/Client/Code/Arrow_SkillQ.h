#ifndef Arrow_SkillQ_h__
#define Arrow_SkillQ_h__

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
class CArrow_SkillQ : public CUnit {
private:
	explicit CArrow_SkillQ(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CArrow_SkillQ(CArrow_SkillQ* _copy);
	virtual ~CArrow_SkillQ(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;
	//
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	virtual void Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);
	void Create_Coll();
	void Set_SpeedToLife(float _speed, float _life = -1.f);
	void Set_SplitCount(int _count) { m_SplitCount = _count; }
	//
private:
	HRESULT		Add_Component(void);

private:
	float		m_LifeTime = 0.f;
	int			m_SplitCount = 0;
public:
	static CArrow_SkillQ*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CArrow_SkillQ*	Create(CArrow_SkillQ* _copy);


private:
	virtual void Free(void) override;

};

END
#endif // Sword_h__
