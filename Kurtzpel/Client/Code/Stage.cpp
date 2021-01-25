#include "stdafx.h"
#include "Stage.h"
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

bool CStage::m_LightCheck = false;

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{

}

CStage::~CStage(void)
{

}

HRESULT CStage::Ready_Scene(void)
{
	
	return S_OK;
}

Engine::_int CStage::Update_Scene(const _float& fTimeDelta)
{
	int temp;
	temp = Engine::CScene::Update_Scene(fTimeDelta);
	CUI_Manager::Get_Instance()->Update_Object(fTimeDelta);
	CNpcQuest_Manager::Get_Instance()->Update_Object(fTimeDelta);
	//
	//CameraControl(fTimeDelta);
	//
	return temp;
}

Engine::_int CStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	CNpcQuest_Manager::Get_NpcQuestInfo()->m_PlayerColl = false;
	Collision_OnlyDynamic_Object(fTimeDelta);
	Collision_StaticToDynamic_Object(fTimeDelta);
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
	player->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	player->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);

	*m_Camera->Get_pPos() = vPos - (vDir * player->m_CameraDist);
	m_Camera->Get_pPos()->y += 3.5f - player->m_LookAtY * 0.72f;
	*m_Camera->Get_pAt() = vPos + (vDir * 200.f);
	m_Camera->Get_pAt()->y += player->m_LookAtY;




}

HRESULT CStage::Ready_Environment_Layer(const _tchar * pLayerTag)
{

	
	return S_OK;
}

HRESULT CStage::Ready_GameLogic_Dynamic_Layer(const _tchar * pLayerTag)
{


	return S_OK;
}

HRESULT CStage::Ready_GameLogic_Static_Layer(const _tchar* pLayerTag)
{

	return S_OK;
}

HRESULT CStage::Ready_GameLogic_StaticNoColl_Layer(const _tchar* pLayerTag)
{

	return S_OK;
}

HRESULT CStage::Ready_GameLogic_Decoration_Layer(const _tchar* pLayerTag)
{

	return S_OK;
}

HRESULT CStage::Ready_UI_Layer(const _tchar * pLayerTag)
{

	return S_OK;
}



HRESULT CStage::Ready_LightInfo(void)
{


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

void CStage::Collision_OnlyDynamic_Object(const _float& fTimeDelta)
{
	Engine::CLayer* pDynamicLayer = Get_Layer(Engine::CLayer::Layer_Dynamic);
	if (pDynamicLayer == nullptr)
		return;
	/*auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), Engine::CTag_Finder(L"GameLogic"));
	if (iter == m_mapLayer.end())
		return;*/

	for (auto& obj_1 : *(pDynamicLayer)->Get_mapObject())
	{
		for (auto& obj_2 : *(pDynamicLayer)->Get_mapObject())
		{
			if (&obj_1 == &obj_2) {
				continue;
			}
			
			CUnit* obj_1Unit = dynamic_cast<CUnit*>(obj_1.second);
			CUnit* obj_2Unit = dynamic_cast<CUnit*>(obj_2.second);
			
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
					float Meshscale = (obj_1Unit->Get_sComponent()->m_pTransformCom->m_vScale.x + obj_2Unit->Get_sComponent()->m_pTransformCom->m_vScale.x) * 0.5f;
					if (dist < scale * Meshscale)
					{
						obj_1Unit->Collision(obj_1_Sphere, obj_2Unit, obj_2_Sphere, fTimeDelta);
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

void CStage::Collision_StaticToDynamic_Object(const _float& fTimeDelta)
{
	Engine::CLayer* pStaticLayer = Get_Layer(Engine::CLayer::Layer_Static);
	if (pStaticLayer == nullptr)
		return;
	Engine::CLayer* pDynamicLayer = Get_Layer(Engine::CLayer::Layer_Dynamic);
	if (pDynamicLayer == nullptr)
		return;

	for (auto& obj_1 : *(pStaticLayer)->Get_mapObject())
	{
		CUnit* obj_1Unit = dynamic_cast<CUnit*>(obj_1.second);
		if (obj_1Unit->m_VecSphereCollider.size() == 0)
			continue;

		for (auto& obj_2 : *(pDynamicLayer)->Get_mapObject())
		{

			CUnit* obj_2Unit = dynamic_cast<CUnit*>(obj_2.second);

			bool collCheck = false;
			if (obj_1Unit->m_VecSphereCollider.size() > 100)
				continue;
			if (obj_1Unit->m_VecSphereCollider.begin() == obj_1Unit->m_VecSphereCollider.end())
				continue;
			CSphereCollider* obj_1_Sphere = *obj_1Unit->m_VecSphereCollider.begin();
			
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
				float scale = obj_1_Sphere->m_pTransformCom->m_vScale.x + (obj_2_Sphere->m_pTransformCom->m_vScale.x * obj_2Unit->Get_sComponent()->m_pTransformCom->m_vScale.x);
				
				if (dist < scale)
				{
					obj_1Unit->Collision(obj_1_Sphere, obj_2Unit, obj_2_Sphere, fTimeDelta);
					obj_2Unit->Collision(obj_2_Sphere, obj_1Unit, obj_1_Sphere, fTimeDelta);
					/*collCheck = true;
					break;*/
				}
			}
			/*if (collCheck)
					break;*/

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
	TCHAR szDataPath[MAX_PATH] = L"../Bin/SaveObjectStage_1.dat";

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