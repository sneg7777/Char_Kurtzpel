#include "stdafx.h"
#include "Stage_2.h"
#include "Stage_1.h"
#include "Export_Function.h"
#include "ApostleOfGreed.h"
#include "Unit_D.h"
#include "SphereCollider.h"
#include "Hammer.h"
#include "NaviTerrain.h"
#include "ArrowNaviTerrain.h"
#include "Player.h"
#include "StaticObject.h"
#include "UI_Manager.h"
#include "LongBow.h"
#include "Npc_01.h"
#include "Portal.h"
#include "EffectMesh.h"
#include "EffectMesh_GHSkillE.h"
#include "EffectRcTex.h"
#include "SoundManager.h"

CStage_2::CStage_2(LPDIRECT3DDEVICE9 pGraphicDev)
	: CStage(pGraphicDev)
{

}

CStage_2::~CStage_2(void)
{

}

HRESULT CStage_2::Ready_Scene(void)
{
	FAILED_CHECK_RETURN(Ready_Environment_Layer(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogic_Dynamic_Layer(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogic_DynamicNoColl_Layer(L"GameLogicNoColl"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogic_Static_Layer(L"GameLogic_Static"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogic_StaticNoColl_Layer(L"GameLogic_StaticNoColl"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogic_Decoration_Layer(L"GameLogic_Decoration"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_UI_Layer(L"UI"), E_FAIL);
	
	if (!m_LightCheck) {
		FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);
		CUI_Manager::Get_Instance()->Ready_CreateUI();
		m_LightCheck = true;
	}
	SoundManager::StopAll();
	SoundManager::PlayBGM(L"BGM_Stage2.ogg");

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	return S_OK;
}

Engine::_int CStage_2::Update_Scene(const _float& fTimeDelta)
{
	if (GetAsyncKeyState(VK_F6) & 0x8000)
	{
		CScene* pScene = nullptr;
		pScene = CStage_1::Create(m_pGraphicDev);

		FAILED_CHECK_RETURN(Engine::SetUp_Scene(pScene), E_FAIL);

		return 0;
	}
	int temp = CStage::Update_Scene(fTimeDelta);
	//
	//CameraControl(fTimeDelta);
	//
	return temp;
}

Engine::_int CStage_2::LateUpdate_Scene(const _float& fTimeDelta)
{
	CStage::LateUpdate_Scene(fTimeDelta);
	//int temp;
	//temp = Engine::CScene::LateUpdate_Scene(fTimeDelta);
	return 0;
}

void CStage_2::Render_Scene(void)
{

}

HRESULT CStage_2::Ready_Environment_Layer(const _tchar * pLayerTag)
{
	Engine::CLayer*			pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	
	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CSkyBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);

	pGameObject = m_Terrain = CTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);

	pGameObject = m_NaviTerrain = CNaviTerrain::Create(m_pGraphicDev, 2);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Navi", pGameObject), E_FAIL);

	pGameObject = m_ArrowNaviTerrain = CArrowNaviTerrain::Create(m_pGraphicDev, 2);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"ArrowNavi", pGameObject), E_FAIL);

	//pGameObject = CStone::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Stone", pGameObject), E_FAIL);

	//pGameObject = CDynamicCamera::Create(m_pGraphicDev, &_vec3(0.f, 10.f, -10.f),
	//													&_vec3(0.f, 0.f, 10.f),
	//													&_vec3(0.f, 1.f, 0.f));
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);

	pLayer->m_LayerName = Engine::CLayer::LayerName::Layer_Environment;
	m_mapLayer.emplace(pLayerTag, pLayer);
	
	return S_OK;
}

HRESULT CStage_2::Ready_GameLogic_Dynamic_Layer(const _tchar * pLayerTag)
{
	Engine::CLayer*			pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CPlayer::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player", pGameObject), E_FAIL);
	

	pGameObject = CNpc_01::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Npc", pGameObject), E_FAIL);

	/*for (_uint i = 0; i < 50; ++i)
	{
		pGameObject = CMonster::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Monster", pGameObject), E_FAIL);
	}*/

	//pGameObject = CTree::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tree", pGameObject), E_FAIL);

	//for (_uint i = 0; i < 100; ++i)
	//{
	//	pGameObject = CEffect::Create(m_pGraphicDev);
	//	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Effect", pGameObject), E_FAIL);
	//}

	pLayer->m_LayerName = Engine::CLayer::LayerName::Layer_Dynamic;
	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_2::Ready_GameLogic_DynamicNoColl_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pLayer->m_LayerName = Engine::CLayer::LayerName::Layer_DynamicNoColl;
	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_2::Ready_GameLogic_Static_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	if (CPlayer::Get_Weapon_Equip() == CPlayer::Weapon_Equip::Weapon_Hammer) {
		pGameObject = CHammer::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CHammer", pGameObject), E_FAIL);
	}
	else {
		pGameObject = CLongBow::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CLongBow", pGameObject), E_FAIL);
	}

	CPortal* pPortal;
	pGameObject = pPortal = CPortal::Create(m_pGraphicDev);
	pPortal->Set_Collider(_vec3{ 60.f, 1.f, 91.f }, 7.f, CSphereCollider::BoneTeam::BoneTeam_Portal, 1);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CPortal", pGameObject), E_FAIL);

	//CEffectRcTex* pEffect;
	//pGameObject = pEffect = CEffectRcTex::Create(m_pGraphicDev);
	//pEffect->Set_Collider(_vec3{ 65.f, 0.f, 81.f }, 7.f, CSphereCollider::BoneTeam::BoneTeam_Effect, 1);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CPortal2", pGameObject), E_FAIL);

	/*pGameObject = CLongBow::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CLongBow", pGameObject), E_FAIL);*/

	pLayer->m_LayerName = Engine::CLayer::LayerName::Layer_Static;
	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_2::Ready_GameLogic_StaticNoColl_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);


	pLayer->m_LayerName = Engine::CLayer::LayerName::Layer_StaticNoColl;
	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_2::Ready_GameLogic_Decoration_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Load_StaticObject(pLayer, pLayerTag);

	pLayer->m_LayerName = Engine::CLayer::LayerName::Layer_Decoration;
	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_2::Ready_UI_Layer(const _tchar * pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = m_Camera = CDynamicCamera::Create(m_pGraphicDev, &_vec3(0.f, 10.f, -10.f),
		&_vec3(0.f, 0.f, 10.f),
		&_vec3(0.f, 1.f, 0.f));
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);
	m_Camera->Set_pPlayer(CPlayer::GetInstance());

	pLayer->m_LayerName = Engine::CLayer::LayerName::Layer_UI;
	m_mapLayer.emplace(pLayerTag, pLayer);
	return S_OK;
}



HRESULT CStage_2::Ready_LightInfo(void)
{
	D3DLIGHT9		tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));
	/*
	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);

	tLightInfo.Direction = _vec3(1.f, -1.f, 1.f);

	if (FAILED(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0)))
		return E_FAIL;

	return S_OK;*/
	////////////////////////////////////////////////////
	//D3DLIGHT9		tLightInfo;
	//ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	//// 0번 조명
	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);

	tLightInfo.Direction = _vec3(0.2f, -1.f, 0.2f);

	if (FAILED(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0)))
		return E_FAIL;

	//TODO : 스포트라이트 이용도 해볼것
	// 1번 조명
	//tLightInfo.Type = D3DLIGHT_POINT;
	//
	//tLightInfo.Diffuse = D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.f);
	//tLightInfo.Specular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	//tLightInfo.Ambient = D3DXCOLOR(250.f/255.f, 242.f/255.f, 230.f/255.f, 0.4f);
	//tLightInfo.Position = _vec3(72.f, 42.f, 61.f);
	////tLightInfo.Position = _vec3(69.f, 62.f, 61.f);
	//tLightInfo.Range = 240.f;
	//
	//
	//if (FAILED(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0)))
	//	return E_FAIL;

	//// 1번 조명
	//tLightInfo.Type = D3DLIGHT_POINT;
	//tLightInfo.Diffuse = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	//tLightInfo.Specular = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	//tLightInfo.Ambient = D3DXCOLOR(0.2f, 0.f, 0.f, 1.f);
	//tLightInfo.Position = _vec3(5.f, 5.f, 5.f);
	//tLightInfo.Range = 10.f;

	//if (FAILED(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 1)))
	//	return E_FAIL;

	//// 2번 조명
	//tLightInfo.Type = D3DLIGHT_POINT;
	//tLightInfo.Diffuse = D3DXCOLOR(0.f, 0.f, 1.f, 1.f);
	//tLightInfo.Specular = D3DXCOLOR(0.f, 0.f, 1.f, 1.f);
	//tLightInfo.Ambient = D3DXCOLOR(0.f, 0.f, 0.2f, 1.f);
	//tLightInfo.Position = _vec3(10.f, 5.f, 10.f);
	//tLightInfo.Range = 10.f;

	//if (FAILED(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 2)))
	//	return E_FAIL;

	return S_OK;
}

CStage_2* CStage_2::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage_2*	pInstance = new CStage_2(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CStage_2::Free(void)
{
	CStage::Free();
}

HRESULT CStage_2::Load_StaticObject(Engine::CLayer* _layer, const _tchar* pLayerTag)
{
	TCHAR szDataPath[MAX_PATH] = L"../Bin/SaveObjectStage2_1.dat";

	HANDLE hFile = CreateFile(szDataPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;
	DWORD dwByte = 0;
	DWORD dwstrByte = 0;

	bool endCheck = false;
	while (true)
	{
		bool sphereOverlap = false;

		_vec3 vecPos, vecAng, vecScal;
		TCHAR meshName[MAX_PATH] = L"";
		_int meshNameSize;

		ReadFile(hFile, &meshNameSize, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &meshName, meshNameSize, &dwByte, nullptr);
		ReadFile(hFile, &vecPos, sizeof(_vec3), &dwByte, nullptr);
		ReadFile(hFile, &vecScal, sizeof(_vec3), &dwByte, nullptr);
		ReadFile(hFile, &vecAng, sizeof(_vec3), &dwByte, nullptr);


		if (0 == dwByte)
		{
			endCheck = true;
			break;
		}

		wstring text, temp2, num;
		text = meshName;



		Engine::CGameObject* pGameObject = nullptr;
		pGameObject = CStaticObject::Create(m_pGraphicDev, text);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(_layer->Add_GameObject(L"StaticObject", pGameObject), E_FAIL);
		Engine::CTransform* objTrans = dynamic_cast<Engine::CTransform*>(pGameObject->Get_Component(L"Com_Transform", Engine::COMPONENTID::ID_DYNAMIC));
		//objTrans->Set_Pos(&vecPos);
		objTrans->m_vInfo[Engine::INFO_POS] = vecPos;
		objTrans->Set_Scale(vecScal.x, vecScal.y, vecScal.z);
		objTrans->Rotation(Engine::ROTATION::ROT_X, vecAng.x);
		objTrans->Rotation(Engine::ROTATION::ROT_Y, vecAng.y);// vecAng.y);
		objTrans->Rotation(Engine::ROTATION::ROT_Z, vecAng.z);
		//pGameObject->Update_Object(0.f);
		//m_mapLayer.emplace(pLayerTag, _layer);

		if (0 == dwByte)
		{
			//Safe_Delete(pUnit);
			break;
		}



		if (endCheck) {
			break;
		}
	}
	CloseHandle(hFile);
}