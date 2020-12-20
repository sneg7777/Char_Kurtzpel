#include "stdafx.h"
#include "Stage.h"
#include "Export_Function.h"
#include "ApostleOfGreed.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{

}

CStage::~CStage(void)
{

}

HRESULT CStage::Ready_Scene(void)
{
	
	FAILED_CHECK_RETURN(Ready_Environment_Layer(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogic_Layer(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);
	
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

Engine::_int CStage::Update_Scene(const _float& fTimeDelta)
{
	int temp;
	temp = Engine::CScene::Update_Scene(fTimeDelta);
	//
	//CameraControl(fTimeDelta);
	//
	return temp;
}

void CStage::Render_Scene(void)
{

}

void CStage::CameraControl(_float fTimeDelta)
{
	CPlayer* player = dynamic_cast<CPlayer*>(Engine::CManagement::GetInstance()->Get_GameObject(L"GameLogic", L"Player"));

	if (player->m_State == player->State::State_Dash && player->m_CameraDist > 240.f) {
		player->m_CameraDist -= fTimeDelta * 1200.f;
	}
	else if (player->m_State != player->State::State_Dash && player->m_CameraDist < 300.f) {
		player->m_CameraDist += fTimeDelta * 1200.f;
	}
	_vec3	vPos, vDir;
	dynamic_cast<Engine::CTransform*>(player->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC))->Get_Info(Engine::INFO_POS, &vPos);
	dynamic_cast<Engine::CTransform*>(player->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC))->Get_Info(Engine::INFO_LOOK, &vDir);

	CDynamicCamera* dCamera = dynamic_cast<CDynamicCamera*>(Engine::CManagement::GetInstance()->Get_GameObject(L"Environment", L"DynamicCamera"));
	*dCamera->Get_pPos() = vPos;
	dCamera->Get_pPos()->y += 3.f;
	dCamera->Get_pPos()->z -= 3.f;
	*dCamera->Get_pAt() = vPos;

}

HRESULT CStage::Ready_Environment_Layer(const _tchar * pLayerTag)
{
	Engine::CLayer*			pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	
	Engine::CGameObject*		pGameObject = nullptr;

	/*pGameObject = CSkyBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);*/

	pGameObject = CTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);

	pGameObject = CDynamicCamera::Create(m_pGraphicDev, &_vec3(0.f, 10.f, -10.f),
														&_vec3(0.f, 0.f, 10.f),
														&_vec3(0.f, 1.f, 0.f));
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);

	
	m_mapLayer.emplace(pLayerTag, pLayer);
	
	return S_OK;
}

HRESULT CStage::Ready_GameLogic_Layer(const _tchar * pLayerTag)
{
	Engine::CLayer*			pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CPlayer::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player", pGameObject), E_FAIL);
	
	pGameObject = CApostleOfGreed::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"B_ApostleOfGreed", pGameObject), E_FAIL);

	/*pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword", pGameObject), E_FAIL);*/
	/*pGameObject = CEquip_Top_01::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Equip_Top_01", pGameObject), E_FAIL);*/

	/*for (_uint i = 0; i < 50; ++i)
	{
		pGameObject = CMonster::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Monster", pGameObject), E_FAIL);
	}*/

	pGameObject = CStone::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Stone", pGameObject), E_FAIL);

	pGameObject = CTree::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tree", pGameObject), E_FAIL);

	for (_uint i = 0; i < 100; ++i)
	{
		pGameObject = CEffect::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Effect", pGameObject), E_FAIL);
	}

	pGameObject = CUI::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI", pGameObject), E_FAIL);


	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage::Ready_UI_Layer(const _tchar * pLayerTag)
{
	return S_OK;
}



HRESULT CStage::Ready_LightInfo(void)
{
	D3DLIGHT9		tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);

	tLightInfo.Direction = _vec3(1.f, -1.f, 1.f);

	if (FAILED(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0)))
		return E_FAIL;

	return S_OK;
}

CStage* CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage*	pInstance = new CStage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CStage::Free(void)
{
	Engine::CScene::Free();
}

