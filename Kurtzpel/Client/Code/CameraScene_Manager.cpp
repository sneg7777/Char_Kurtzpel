#include "stdafx.h"
#include "CameraScene_Manager.h"
#include "Export_Function.h"
#include "UI_Manager.h"
#include "Portal.h"
#include "Stage_1.h"
#include "Stage_2.h"

CCameraScene_Manager* CCameraScene_Manager::m_pInstance = nullptr;

void CCameraScene_Manager::Destroy_Instance()
{
	if (m_pInstance) {
		Engine::Safe_Release(m_pInstance->m_pGraphicDev);
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

CCameraScene_Manager::CCameraScene_Manager()
{

}

CCameraScene_Manager::~CCameraScene_Manager(void)
{

}



HRESULT Client::CCameraScene_Manager::Ready_Object(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;
	Engine::Safe_AddRef(m_pGraphicDev);
	return S_OK;
}

Client::_int Client::CCameraScene_Manager::Update_Object(const _float& fTimeDelta)
{
	if (m_CameraSceneNumber != 0) {
		m_Camera = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera();
		Update_CameraScene(fTimeDelta);
	}

	return 0;
}

void Client::CCameraScene_Manager::Render_Object(void)
{

}

void Client::CCameraScene_Manager::Update_CameraScene(const _float& fTimeDelta)
{
	if (m_CameraSceneNumber == 1) {
		Engine::CGameObject* pGameObject = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_LayerObject(Engine::CLayer::Layer_Dynamic, Engine::CGameObject::UnitName::Monster);
		CMonster* pMonster = dynamic_cast<CMonster*>(pGameObject);
		Engine::CTransform* pTrans = pMonster->Get_sComponent()->m_pTransformCom;
		_vec3 vPos, vDir, vAfterPos, vAfterAt;
		pTrans->Get_Info(Engine::INFO_POS, &vPos);
		pTrans->Get_Info(Engine::INFO_LOOK, &vDir);
		vAfterPos = vPos - vDir * 700.f;
		vAfterPos.y += 1.5f;
		_matrix sphereMat = *pMonster->Get_BonePartCollider(CSphereCollider::BonePart::BonePart_CollBody)->m_pTransformCom->Get_WorldMatrix();
		vAfterAt = { sphereMat._41, sphereMat._42, sphereMat._43 };
		m_Camera->Set_PosToAt(vAfterPos, vAfterAt);
		m_CameraSceneTime -= fTimeDelta;
		if (m_CameraSceneTime < 0.f) {
			m_CameraSceneTime = 0.f;
			m_CameraSceneNumber = 0;
			CPlayer* player = CPlayer::GetInstance();
			player->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
			player->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
			vAfterPos = vPos - (vDir * player->m_CameraDist);
			vAfterPos.y += 3.5f - player->m_LookAtY * 0.7f;
			vAfterAt = vPos + (vDir * 200.f);
			vAfterAt.y += player->m_LookAtY;
			m_Camera->Set_PosToAt(vAfterPos, vAfterAt);
		}
	}
}