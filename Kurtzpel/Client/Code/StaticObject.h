#ifndef StaticObject_h__
#define StaticObject_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CStaticMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;
class COptimization;
class CShader;

END

BEGIN(Client)
class CStaticObject : public Engine::CGameObject
{	
private:
	explicit CStaticObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStaticObject(void);

public:
	HRESULT Ready_Object(bool check = false, wstring _wstring = L"");
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		Add_Component(bool check = false, wstring _wstring = L"");
	_bool		Collision_ToObject(const _tchar* pLayerTag, const _tchar* pObjTag);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
private:

	Engine::CStaticMesh* m_pMeshCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CRenderer* m_pRendererCom = nullptr;
	Engine::CCalculator* m_pCalculatorCom = nullptr;
	Engine::CCollider* m_pColliderCom = nullptr;
	Engine::COptimization* m_pOptimizationCom = nullptr;
	Engine::CShader* m_pShaderCom = nullptr;
	_bool						m_bColl = false;
	_bool						m_bDraw = false;

public:
	static CStaticObject* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CStaticObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, wstring _wstring);


private:
	virtual void Free(void) override;

};

END
#endif // Stone_h__
