#ifndef Monster_h__
#define Monster_h__

#include "Unit_D.h"
#include "Define.h"

BEGIN(Engine)

class CDynamicMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;
//class COptimization;
class CNaviMesh;
class CShader;

END
class CUnit_D;
class CSphereCollider;
BEGIN(Client)
class CMonster : public CUnit_D
{
public:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

protected:
	HRESULT		Add_Component(void);
	_bool		Collision_ToObject(const _tchar* pLayerTag, const _tchar* pObjTag);
	float		PlayerSearchDistance();
public:

	//_bool						m_bColl = false;
	//_bool						m_bDraw = false;

	Engine::CTransform* m_pPlayerTrans = nullptr;
	bool						m_isSearch = false;

public:
	vector<CSphereCollider*>		m_VecSphereCollider;
protected:
	virtual void Free(void) override;

};

END
#endif // Stone_h__
