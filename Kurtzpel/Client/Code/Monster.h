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
	void		Set_PlayerTowardAngle();
	void		Rocate_PlayerToWardAngle(float fTimeDelta, float _speed = 120.f);
	void		Effect_Damaged(CUnit* _col, CSphereCollider* _colSphere);
	virtual HRESULT SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
public:

	//_bool						m_bColl = false;
	//_bool						m_bDraw = false;

	Engine::CTransform* m_pPlayerTrans = nullptr;
	bool						m_isSearch = false;
	float						m_TimeGroggy = 0.f;

	//
	bool						m_AngleOfSame = false;
	float						m_TowardAngle1 = 0.f;
	float						m_TowardAngle2 = 0.f;

protected:
	virtual void Free(void) override;

};

END
#endif // Stone_h__
