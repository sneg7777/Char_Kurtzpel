#include "stdafx.h"
#include "Stage_1.h"
#include "Stage_2.h"
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
#include "NpcQuest_Manager.h"
#include "Monster1_TwoHand.h"

CStage_1::CStage_1(LPDIRECT3DDEVICE9 pGraphicDev)
	: CStage(pGraphicDev)
{

}

CStage_1::~CStage_1(void)
{

}

HRESULT CStage_1::Ready_Scene(void)
{
	FAILED_CHECK_RETURN(Ready_Environment_Layer(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogic_Dynamic_Layer(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogic_Static_Layer(L"GameLogic_Static"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogic_StaticNoColl_Layer(L"GameLogic_StaticNoColl"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogic_Decoration_Layer(L"GameLogic_Decoration"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_UI_Layer(L"UI"), E_FAIL);
	
	if (!m_LightCheck) {
		FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);
		CUI_Manager::Get_Instance()->Ready_CreateUI();
		m_LightCheck = true;
	}
	
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	return S_OK;
}

Engine::_int CStage_1::Update_Scene(const _float& fTimeDelta)
{
	if (GetAsyncKeyState(VK_F5) & 0x8000)
	{
		CScene* pScene = nullptr;
		pScene = CStage_2::Create(m_pGraphicDev);

		FAILED_CHECK_RETURN(Engine::SetUp_Scene(pScene), E_FAIL);

		return 0;
	}
	int temp = CStage::Update_Scene(fTimeDelta);
	//
	//CameraControl(fTimeDelta);
	//
	return temp;
}

Engine::_int CStage_1::LateUpdate_Scene(const _float& fTimeDelta)
{
	CStage::LateUpdate_Scene(fTimeDelta);
	//int temp;
	//temp = Engine::CScene::LateUpdate_Scene(fTimeDelta);
	return 0;
}

void CStage_1::Render_Scene(void)
{

}

HRESULT CStage_1::Ready_Environment_Layer(const _tchar * pLayerTag)
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

	pGameObject = m_NaviTerrain = CNaviTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Navi", pGameObject), E_FAIL);

	pGameObject = m_ArrowNaviTerrain = CArrowNaviTerrain::Create(m_pGraphicDev);
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

HRESULT CStage_1::Ready_GameLogic_Dynamic_Layer(const _tchar * pLayerTag)
{
	Engine::CLayer*			pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CPlayer::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player", pGameObject), E_FAIL);
	
	int questNumber = CNpcQuest_Manager::Get_Instance()->Get_NpcQuestInfo()->m_QuestNumber;
	if (questNumber == 7 || questNumber == 9) {
		pGameObject = CMonster1_TwoHand::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"M_Monster1_TwoHand", pGameObject), E_FAIL);
		//pGameObject = CMonster1_TwoHand::Create(m_pGraphicDev);
		//NULL_CHECK_RETURN(pGameObject, E_FAIL);
		//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"M_Monster1_TwoHand", pGameObject), E_FAIL);
		//dynamic_cast<CMonster1_TwoHand*>(pGameObject)->Get_sComponent()->m_pTransformCom->Set_Pos(75.f, 75.f, 75.f);
		//pGameObject->Engine::CGameObject::Update_Object(0.f);
	}
	else if(questNumber == 11){
		pGameObject = CApostleOfGreed::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"B_ApostleOfGreed", pGameObject), E_FAIL);

	}

	/*pGameObject = CEquip_Top_01::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Equip_Top_01", pGameObject), E_FAIL);*/

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

HRESULT CStage_1::Ready_GameLogic_Static_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CHammer::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CHammer", pGameObject), E_FAIL);
	/*pGameObject = CLongBow::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CLongBow", pGameObject), E_FAIL);*/

	pLayer->m_LayerName = Engine::CLayer::LayerName::Layer_Static;
	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_1::Ready_GameLogic_StaticNoColl_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);


	pLayer->m_LayerName = Engine::CLayer::LayerName::Layer_StaticNoColl;
	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_1::Ready_GameLogic_Decoration_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Load_StaticObject(pLayer, pLayerTag);

	pLayer->m_LayerName = Engine::CLayer::LayerName::Layer_Decoration;
	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage_1::Ready_UI_Layer(const _tchar * pLayerTag)
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



HRESULT CStage_1::Ready_LightInfo(void)
{
	/*D3DLIGHT9		tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);

	tLightInfo.Direction = _vec3(1.f, -1.f, 1.f);

	if (FAILED(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0)))
		return E_FAIL;

	return S_OK;*/
	////////////////////////////////////////////////////
	D3DLIGHT9		tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	// 0번 조명
	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;
	
	tLightInfo.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR( 0.7f, 0.7f, 0.7f, 1.f);
	
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

CStage_1* CStage_1::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage_1*	pInstance = new CStage_1(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CStage_1::Free(void)
{
	CStage::Free();
}

HRESULT CStage_1::Load_StaticObject(Engine::CLayer* _layer, const _tchar* pLayerTag)
{
	TCHAR szDataPath[MAX_PATH] = L"../Bin/SaveObject3.dat";

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
		pGameObject->Update_Object(0.f);
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