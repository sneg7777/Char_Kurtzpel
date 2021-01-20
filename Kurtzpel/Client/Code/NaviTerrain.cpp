#include "stdafx.h"
#include "NaviTerrain.h"
#include "Export_Function.h"
#include "NaviMesh.h"
#include "Player.h"

CNaviTerrain::CNaviTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CNaviTerrain::~CNaviTerrain(void)
{

}

HRESULT Client::CNaviTerrain::Add_Component(int number)
{
	Engine::CComponent*		pComponent = nullptr;

	// NaviMesh
	if(number == 2)
		pComponent = m_pNaviMeshCom = dynamic_cast<Engine::CNaviMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Navi2"));
	else
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

CNaviTerrain* CNaviTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev, int number)
{
	CNaviTerrain*	pInstance = new CNaviTerrain(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(number)))
		Client::Safe_Release(pInstance);
	return pInstance;
}

void CNaviTerrain::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CNaviTerrain::Ready_Object()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pNaviMeshCom->Set_NaviIndex(0);

	return S_OK;
}

HRESULT Client::CNaviTerrain::Ready_Object(int number)
{
	FAILED_CHECK_RETURN(Add_Component(number), E_FAIL);

	m_pNaviMeshCom->Set_NaviIndex(0);

	return S_OK;
}

Client::_int Client::CNaviTerrain::Update_Object(const _float& fTimeDelta)
{
	//CGameObject::Update_Object(fTimeDelta);

	if(CPlayer::GetInstance()->m_bCheck[CPlayer::bCheck::bCheck_RenderSphere])
		m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}
void Client::CNaviTerrain::Render_Object(void)
{
	m_pNaviMeshCom->Render_NaviMeshes();
}