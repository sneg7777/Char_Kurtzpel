#ifndef EffectMesh_GHSkillE_h__
#define EffectMesh_GHSkillE_h__

#include "EffectMesh.h"
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
class CEffectMesh_GHSkillE : public CEffectMesh {
private:
	explicit CEffectMesh_GHSkillE(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffectMesh_GHSkillE(void);

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

public:
	static CEffectMesh_GHSkillE* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;


};

END
#endif
