#include "stdafx.h"
#include "Equip_Top_01.h"
#include "Export_Function.h"

CEquip_Top_01::CEquip_Top_01(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CEquip_Top_01::~CEquip_Top_01(void)
{

}

HRESULT Client::CEquip_Top_01::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	
	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Equip_Top_01"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Clone(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Safe_AddRef(pComponent);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>(Engine::Clone(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Calculator", pComponent);

	// Collider 
	pComponent = m_pColliderCom = Engine::CCollider::Create(m_pGraphicDev, m_pMeshCom->Get_VtxPos(), m_pMeshCom->Get_NumVtx(), m_pMeshCom->Get_Stride());
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);

	return S_OK;
}


CEquip_Top_01* CEquip_Top_01::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEquip_Top_01*	pInstance = new CEquip_Top_01(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CEquip_Top_01::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CEquip_Top_01::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	//m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(180.f));
	m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(145.f));
	//m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(265.f));
	return S_OK;
}
Client::_int Client::CEquip_Top_01::Update_Object(const _float& fTimeDelta)
{
	if (nullptr == m_pParentBoneMatrix)
	{
		Engine::CDynamicMesh*	pPlayerMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Mesh", Engine::ID_STATIC));
		NULL_CHECK_RETURN(pPlayerMeshCom, 0);

		const Engine::D3DXFRAME_DERIVED* pFrame = pPlayerMeshCom->Get_FrameByName("Root");

		m_pParentBoneMatrix = &pFrame->CombinedTransformationMatrix;

		Engine::CTransform*		pPlayerTransCom = dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", Engine::ID_DYNAMIC));
		NULL_CHECK_RETURN(pPlayerTransCom, 0);
		m_pParentWorldMatrix = pPlayerTransCom->Get_WorldMatrix();
	}

	Engine::CGameObject::Update_Object(fTimeDelta);
		
	m_pTransformCom->Set_ParentMatrix(&(*m_pParentBoneMatrix * *m_pParentWorldMatrix));
	

	//m_bColl = Collision_ToObject(L"GameLogic", L"Player");

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}
void Client::CEquip_Top_01::Render_Object(void)
{
	m_pTransformCom->Set_Transform(m_pGraphicDev);

	m_pMeshCom->Render_Meshes();

	_matrix matWorld;
		m_pTransformCom->Get_WorldMatrix(&matWorld);
		//m_pTransformCom->Get_NRotWorldMatrix(&matWorld);

	m_pColliderCom->Render_Collider(Engine::COLLTYPE(m_bColl), &matWorld);


}


_bool CEquip_Top_01::Collision_ToObject(const _tchar * pLayerTag, const _tchar * pObjTag)
{
	Engine::CCollider*	pPlayerColliderCom = dynamic_cast<Engine::CCollider*>(Engine::Get_Component(pLayerTag, pObjTag, L"Com_Collider", Engine::ID_STATIC));
	NULL_CHECK_RETURN(pPlayerColliderCom, false);
	
	/*return m_pCalculatorCom->Collision_AABB(pPlayerColliderCom->Get_Min(),
											pPlayerColliderCom->Get_Max(),
											pPlayerColliderCom->Get_CollMatrix(),
											m_pColliderCom->Get_Min(), 
											m_pColliderCom->Get_Max(),
											m_pColliderCom->Get_CollMatrix());*/

	return m_pCalculatorCom->Collision_OBB(pPlayerColliderCom->Get_Min(),
		pPlayerColliderCom->Get_Max(),
		pPlayerColliderCom->Get_CollMatrix(),
		m_pColliderCom->Get_Min(),
		m_pColliderCom->Get_Max(),
		m_pColliderCom->Get_CollMatrix());
}
