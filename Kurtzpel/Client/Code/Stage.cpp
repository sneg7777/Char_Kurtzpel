#include "stdafx.h"
#include "Stage.h"
#include "Export_Function.h"
#include "ApostleOfGreed.h"
#include "Unit_D.h"
#include "SphereCollider.h"
#include "Hammer.h"
#include "NaviTerrain.h"
#include "Player.h"
#include "StaticObject.h"

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
	FAILED_CHECK_RETURN(Ready_GameLogic_Dynamic_Layer(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_GameLogic_Static_Layer(L"GameLogic_Static"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_UI_Layer(L"UI"), E_FAIL);
	
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

Engine::_int CStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	Collision_Object(fTimeDelta);
	//int temp;
	//temp = Engine::CScene::LateUpdate_Scene(fTimeDelta);
	return 0;
}

void CStage::Render_Scene(void)
{

}

void CStage::CameraControl(_float fTimeDelta)
{
	CPlayer* player = CPlayer::GetInstance();

	if (player->m_State == player->State::State_Dash && player->m_CameraDist > 240.f) {
		player->m_CameraDist -= fTimeDelta * 1200.f;
	}
	else if (player->m_State != player->State::State_Dash && player->m_CameraDist < 300.f) {
		player->m_CameraDist += fTimeDelta * 1200.f;
	}
	_vec3	vPos, vDir;
	player->m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	player->m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);

	*m_Camera->Get_pPos() = vPos - (vDir * player->m_CameraDist);
	m_Camera->Get_pPos()->y += 3.5f - player->m_LookAtY * 0.72f;
	*m_Camera->Get_pAt() = vPos + (vDir * 200.f);
	m_Camera->Get_pAt()->y += player->m_LookAtY;




}

HRESULT CStage::Ready_Environment_Layer(const _tchar * pLayerTag)
{
	Engine::CLayer*			pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	
	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CSkyBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);

	pGameObject = CTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);

	pGameObject = m_NaviTerrain = CNaviTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Navi", pGameObject), E_FAIL);

	pGameObject = CStone::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Stone", pGameObject), E_FAIL);

	//pGameObject = CDynamicCamera::Create(m_pGraphicDev, &_vec3(0.f, 10.f, -10.f),
	//													&_vec3(0.f, 0.f, 10.f),
	//													&_vec3(0.f, 1.f, 0.f));
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);

	
	m_mapLayer.emplace(pLayerTag, pLayer);
	
	return S_OK;
}

HRESULT CStage::Ready_GameLogic_Dynamic_Layer(const _tchar * pLayerTag)
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

	/*pGameObject = CUI::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI", pGameObject), E_FAIL);*/


	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage::Ready_GameLogic_Static_Layer(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CHammer::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CHammer", pGameObject), E_FAIL);

	Load_StaticObject(pLayer, pLayerTag);

	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;
}

HRESULT CStage::Ready_UI_Layer(const _tchar * pLayerTag)
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

	m_mapLayer.emplace(pLayerTag, pLayer);
	return S_OK;
}



HRESULT CStage::Ready_LightInfo(void)
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

	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.25f, 0.25f, 0.25f, 1.f);

	tLightInfo.Direction = _vec3(1.f, -1.f, 1.f);

	if (FAILED(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0)))
		return E_FAIL;

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

void CStage::Collision_Object(const _float& fTimeDelta)
{
	auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), Engine::CTag_Finder(L"GameLogic"));

	if (iter == m_mapLayer.end())
		return;

	for (auto& obj_1 : *(iter->second)->Get_mapObject())
	{
		for (auto& obj_2 : *(iter->second)->Get_mapObject())
		{
			if (&obj_1 == &obj_2) {
				continue;
			}
			
			CUnit_D* obj_1Unit = dynamic_cast<CUnit_D*>(obj_1.second);
			CUnit_D* obj_2Unit = dynamic_cast<CUnit_D*>(obj_2.second);
			
			bool collCheck = false;
			for (auto& obj_1_Sphere : obj_1Unit->m_VecSphereCollider)
			{
				for (auto& obj_2_Sphere : obj_2Unit->m_VecSphereCollider)
				{
					// 거리계산
					_matrix obj_1_worldM = *obj_1_Sphere->m_pTransformCom->Get_WorldMatrix();
					_matrix obj_2_worldM = *obj_2_Sphere->m_pTransformCom->Get_WorldMatrix();
					_vec3 obj_1_Pos = { obj_1_worldM._41, obj_1_worldM._42, obj_1_worldM._43 };
					_vec3 obj_2_Pos = { obj_2_worldM._41, obj_2_worldM._42, obj_2_worldM._43 };

					_vec3 dir = obj_1_Pos - obj_2_Pos;
					float dist = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
					// 거리가 반지름의 합보다 작으면 충돌
					float scale = obj_1_Sphere->m_pTransformCom->m_vScale.x + obj_2_Sphere->m_pTransformCom->m_vScale.x;
					float Meshscale = (obj_1Unit->m_pTransformCom->m_vScale.x + obj_2Unit->m_pTransformCom->m_vScale.x) * 0.5f;
					if (dist < scale * Meshscale)
					{
						obj_1Unit->Collision(obj_1_Sphere, obj_2.second, obj_2_Sphere, fTimeDelta);
						/*collCheck = true;
						break;*/
					}
				}
				/*if (collCheck)
					break;*/
			}


			//// 거리계산
			//_vec3 dir = obj_1Unit->m_pTransformCom->m_vInfo[Engine::INFO_POS] - obj_2Unit->m_pTransformCom->m_vInfo[Engine::INFO_POS];
			//float dist = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
			//// 거리가 반지름의 합보다 작으면 충돌
			//if (dist < (obj_1Unit->m_pTransformCom->m_vScale.x + obj_2Unit->m_pTransformCom->m_vScale.x))
			//{
			//	obj_1.second->Collision(obj_2.second);
			//}
			continue;
			

		}
	}
}

HRESULT CStage::Load_StaticObject(Engine::CLayer* _layer, const _tchar* pLayerTag)
{
	TCHAR szDataPath[MAX_PATH] = L"../Bin/SaveObject.dat";

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
		objTrans->Rotation(Engine::ROTATION::ROT_Y, vecAng.y);
		objTrans->Rotation(Engine::ROTATION::ROT_Z, vecAng.z);
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