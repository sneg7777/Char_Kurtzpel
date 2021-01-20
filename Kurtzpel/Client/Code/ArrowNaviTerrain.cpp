#include "stdafx.h"
#include "ArrowNaviTerrain.h"
#include "Export_Function.h"
#include "Player.h"

CArrowNaviTerrain::CArrowNaviTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CArrowNaviTerrain::~CArrowNaviTerrain(void)
{

}

HRESULT Client::CArrowNaviTerrain::Add_Component(int number)
{
	Engine::CComponent*		pComponent = nullptr;

	// NaviMesh
	if(number == 2)
		pComponent = m_pArrowNaviMeshCom = dynamic_cast<Engine::CArrowNaviMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_ArrowNavi2"));
	else
		pComponent = m_pArrowNaviMeshCom = dynamic_cast<Engine::CArrowNaviMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_ArrowNavi"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Navi", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Safe_AddRef(pComponent);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	return S_OK;
}

CArrowNaviTerrain* CArrowNaviTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev, int number)
{
	CArrowNaviTerrain*	pInstance = new CArrowNaviTerrain(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(number)))
		Client::Safe_Release(pInstance);
	return pInstance;
}

void CArrowNaviTerrain::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CArrowNaviTerrain::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pArrowNaviMeshCom->Set_NaviIndex(0);

	return S_OK;
}

HRESULT Client::CArrowNaviTerrain::Ready_Object(int number)
{
	FAILED_CHECK_RETURN(Add_Component(number), E_FAIL);

	m_pArrowNaviMeshCom->Set_NaviIndex(0);

	return S_OK;
}

Client::_int Client::CArrowNaviTerrain::Update_Object(const _float& fTimeDelta)
{
	return 0;
	CGameObject::Update_Object(fTimeDelta);

	if(CPlayer::GetInstance()->m_bCheck[CPlayer::bCheck::bCheck_RenderSphere])
		m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}
void Client::CArrowNaviTerrain::Render_Object(void)
{
	m_pArrowNaviMeshCom->Render_NaviMeshes();
}