#ifndef EffectMesh_h__
#define EffectMesh_h__

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
class CEffectMesh : public CUnit {
protected:
	explicit CEffectMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffectMesh(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;
	//
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	virtual void Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);
	//_pos : À§Ä¡
	//_scale : Å©±â(float)
	//_team : ÆÀ
	void		Set_Collider(_vec3 _pos, float _scale, CSphereCollider::BoneTeam _team, int _portalMapNumber);
	int			Get_PortalMapNumber() { return m_PortalMapNumber; }
	//
private:
	HRESULT		Add_Component(void);

public:
	static CEffectMesh* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free(void) override;
	int			m_PortalMapNumber = 0;
	float		m_EffectTime = 0;
	Engine::CRcTex* m_pBufferCom = nullptr;

};

END
#endif
