#include "stdafx.h"
#include "SphereCollider.h"
#include "Export_Function.h"
#include "Player.h"
#include "Monster.h"


CSphereCollider::CSphereCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CSphereCollider::~CSphereCollider(void)
{

}

HRESULT CSphereCollider::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

	// buffer
	//pComponent = m_pBufferCom = dynamic_cast<Engine::CCollSphere*>(Engine::Clone(Engine::RESOURCE_STATIC, L"Buffer_CollSphere"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

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

	//m_pBufferCom->Set_Color(D3DCOLOR_ARGB(255, 148, 155, 0));
	m_pTransformCom->m_vScale = { 30.f, 30.f, 30.f };
	return S_OK;
}

CSphereCollider* CSphereCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSphereCollider* pInstance = new CSphereCollider(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CSphereCollider::Free(void)
{
	Engine::CGameObject::Free();
}

HRESULT CSphereCollider::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	//Add_Vtx();

	return S_OK;
}

_int CSphereCollider::Update_Object(const _float& fTimeDelta)
{
	if (m_Dead)
		return 1;
	

	if (nullptr == m_pParentBoneMatrix)
	{
		if (m_pDynamicMesh == nullptr)
			return 0;
		if (!m_FrameNameCheck)
			return 0;

		//다이나믹을 안쓰고 C스타일의 강제 형변환을 써서 m_pMeshCom 를 참조해도 된다함.
		if(nullptr == dynamic_cast<CMonster*>(m_pDynamicMesh));
			return 0;
		const Engine::D3DXFRAME_DERIVED* pFrame = dynamic_cast<CMonster*>(m_pDynamicMesh)->m_pMeshCom->Get_FrameByName(m_FrameName.c_str());

		m_pParentBoneMatrix = &pFrame->CombinedTransformationMatrix;

		Engine::CTransform* pPlayerTransCom = dynamic_cast<CMonster*>(m_pDynamicMesh)->m_pTransformCom;
		NULL_CHECK_RETURN(pPlayerTransCom, 0);
		m_pParentWorldMatrix = pPlayerTransCom->Get_WorldMatrix();
	}

	Engine::CGameObject::Update_Object(fTimeDelta);

	m_pTransformCom->Set_ParentMatrix(&(*m_pParentBoneMatrix * *m_pParentWorldMatrix));


	//m_bColl = Collision_ToObject(L"GameLogic", L"Player");

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}
void CSphereCollider::Render_Object(void)
{
	m_pTransformCom->Set_Transform(m_pGraphicDev);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	//m_pBufferCom->Render_Buffer();
	//m_pTextureCom->Render_Texture(0);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}
