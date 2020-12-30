#include "stdafx.h"
#include "NaviTerrain.h"
#include "Export_Function.h"
#include "NaviMesh.h"

CNaviTerrain::CNaviTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CNaviTerrain::~CNaviTerrain(void)
{

}

HRESULT Client::CNaviTerrain::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// NaviMesh
	pComponent = m_pNaviMeshCom = dynamic_cast<Engine::CNaviMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Navi"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Navi", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Safe_AddRef(pComponent);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	return S_OK;
}

CNaviTerrain* CNaviTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CNaviTerrain*	pInstance = new CNaviTerrain(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);
	return pInstance;
}

void CNaviTerrain::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CNaviTerrain::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pNaviMeshCom->Set_NaviIndex(0);

	return S_OK;
}
Client::_int Client::CNaviTerrain::Update_Object(const _float& fTimeDelta)
{
	CGameObject::Update_Object(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}
void Client::CNaviTerrain::Render_Object(void)
{
	m_pNaviMeshCom->Render_NaviMeshes();
}