#ifndef Equip_Top_01_h__
#define Equip_Top_01_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CStaticMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;

END

BEGIN(Client)
class CEquip_Top_01 : public Engine::CGameObject
{
private:
	explicit CEquip_Top_01(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEquip_Top_01(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		Add_Component(void);

private:

	Engine::CStaticMesh*		m_pMeshCom = nullptr;
	Engine::CTransform*			m_pTransformCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CCalculator*		m_pCalculatorCom = nullptr;
	Engine::CCollider*			m_pColliderCom = nullptr;
	_bool						m_bColl = false;

	const	_matrix*			m_pParentBoneMatrix = nullptr;
	const	_matrix*			m_pParentWorldMatrix = nullptr;

public:
	static CEquip_Top_01*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif // Sword_h__
