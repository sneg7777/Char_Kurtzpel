#include "stdafx.h"
#include "CameraScene_Manager.h"
#include "Export_Function.h"
#include "UI_Manager.h"
#include "Portal.h"
#include "Stage_1.h"
#include "Stage_2.h"
#include "NpcQuest_Manager.h"

#define QuestNuber_7 1
#define PlayerSkillZ_GH 2
#define PlayerSkillZ_LB 3
#define QuestNumber_7_Time 5.f
#define PlayerSkillZ_GH_Time 2.f
#define PlayerSkillZ_GH_TimeHalf 1.f
#define PlayerSkillZ_LB_Time 2.8f
#define PlayerSkillZ_LB_Time_1 1.8f
#define PlayerSkillZ_LB_Time_2 1.2f
#define PlayerSkillZ_LB_Time_3 0.6f
#define PlayerSkillZ_LB_Time_4 0.2f
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
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (m_CameraSceneNumber == QuestNuber_7) {
		Update_CameraScene_QuestNumber_7(fTimeDelta);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if (m_CameraSceneNumber == PlayerSkillZ_GH) {
		Update_CameraScene_SkillZ_GH(fTimeDelta);
	}
	else if (m_CameraSceneNumber == PlayerSkillZ_LB) {
		Update_CameraScene_SkillZ_LB(fTimeDelta);
	}
	


	m_CameraSceneTime -= fTimeDelta;
	if (m_CameraSceneTime < 0.f) {
		_vec3 vPos, vDir, vAfterPos, vAfterAt;
		if (m_CameraSceneNumber == QuestNuber_7) {
			CPlayer* player = CPlayer::GetInstance();
			player->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
			player->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
			vAfterPos = vPos - (vDir * player->m_CameraDist);
			vAfterPos.y += 3.5f - player->m_LookAtY * 0.7f;
			vAfterAt = vPos + (vDir * 200.f);
			vAfterAt.y += player->m_LookAtY;
			m_Camera->Set_PosToAt(vAfterPos, vAfterAt);
		}
		else if (m_CameraSceneNumber == PlayerSkillZ_GH) {
			CNpcQuest_Manager::Get_NpcQuestInfo()->m_TalkEnd = true;
		}
		m_CameraSceneTime = 0.f;
		m_CameraSceneNumber = 0;
	}
}

void CCameraScene_Manager::Set_CameraScene(float _number)
{
	m_CameraSceneNumber = _number;
	m_Camera = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera();
	switch (m_CameraSceneNumber)
	{
		case QuestNuber_7:
			m_CameraSceneTime = QuestNumber_7_Time;
			break;
		case PlayerSkillZ_GH: {
			_vec3 vPos, vDir, vAfterPos;
			Engine::CTransform* pTrans = CPlayer::GetInstance()->Get_sComponent()->m_pTransformCom;
			pTrans->Get_Info(Engine::INFO_POS, &vPos);
			pTrans->Get_Info(Engine::INFO_LOOK, &vDir);
			m_CameraPosInit = vDir * 250.f;
			vAfterPos = vPos + m_CameraPosInit;
			vPos.y += 1.f;
			vAfterPos.y += 2.5f;
			m_CameraBeforeNomal = vAfterPos;
			m_Camera->Set_PosToAt(vAfterPos, vPos);
			m_CameraSceneTime = PlayerSkillZ_GH_Time;
			break;
		}
		case PlayerSkillZ_LB: {
			_vec3 vPos, vDir, vAfterPos;
			Engine::CTransform* pTrans = CPlayer::GetInstance()->Get_sComponent()->m_pTransformCom;
			pTrans->Get_Info(Engine::INFO_POS, &vPos);
			pTrans->Get_Info(Engine::INFO_LOOK, &vDir);
			m_CameraPosInit = vDir * 250.f;
			vAfterPos = vPos + m_CameraPosInit;
			vPos.y += 1.f;
			vAfterPos.y += 2.5f;
			m_CameraBeforeNomal = vAfterPos;
			m_Camera->Set_PosToAt(vAfterPos, vPos);
			m_CameraSceneTime = PlayerSkillZ_LB_Time;
			break;
		}
		default:
			break;
	}


}

void Client::CCameraScene_Manager::Update_CameraScene_QuestNumber_7(const _float& fTimeDelta)
{
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
}

void Client::CCameraScene_Manager::Update_CameraScene_SkillZ_GH(const _float& fTimeDelta)
{
	_vec3* pCameraPos = m_Camera->Get_pPos();
	_vec3 pPlayerPos;
	CPlayer::GetInstance()->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_POS, &pPlayerPos);
	if (m_CameraSceneTime > PlayerSkillZ_GH_TimeHalf) {
		_vec3 vTemp = _vec3{ m_CameraPosInit.z , m_CameraPosInit.y, -m_CameraPosInit.x };
		_vec3 vDistance = vTemp - m_CameraPosInit;
		m_CameraBeforeNomal += vDistance * (fTimeDelta / PlayerSkillZ_GH_TimeHalf);
		_vec3 vAfterPos = m_CameraBeforeNomal - pPlayerPos;
		D3DXVec3Normalize(&vAfterPos, &vAfterPos);
		*pCameraPos = pPlayerPos + vAfterPos * 3.5f;
		pCameraPos->y = 2.5f;
		if (m_CameraSceneTime - fTimeDelta < PlayerSkillZ_GH_TimeHalf)
			m_CameraPosInit = _vec3{ m_CameraPosInit.z , m_CameraPosInit.y, -m_CameraPosInit.x };
	}
	else {
		_vec3 vTemp = _vec3{ m_CameraPosInit.z , m_CameraPosInit.y, -m_CameraPosInit.x };
		_vec3 vDistance = vTemp - m_CameraPosInit;
		m_CameraBeforeNomal += vDistance * (fTimeDelta / PlayerSkillZ_GH_TimeHalf);
		_vec3 vAfterPos = m_CameraBeforeNomal - pPlayerPos;
		D3DXVec3Normalize(&vAfterPos, &vAfterPos);
		*pCameraPos = pPlayerPos + vAfterPos * 3.5f;
		pCameraPos->y = 2.5f;
	}
	m_Camera->Shake_CameraMove(fTimeDelta);

}

void Client::CCameraScene_Manager::Update_CameraScene_SkillZ_LB(const _float& fTimeDelta)
{
	_vec3* pCameraPos = m_Camera->Get_pPos();
	_vec3 playerPos, playerDir;
	CPlayer::GetInstance()->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_POS, &playerPos);
	CPlayer::GetInstance()->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_LOOK, &playerDir);
	if (m_CameraSceneTime > PlayerSkillZ_LB_Time_1) {
		_vec3 vTemp = _vec3{ m_CameraPosInit.z , m_CameraPosInit.y, -m_CameraPosInit.x };
		_vec3 vDistance = vTemp - m_CameraPosInit;
		m_CameraBeforeNomal += vDistance * (fTimeDelta / PlayerSkillZ_LB_Time_1);
		_vec3 vAfterPos = m_CameraBeforeNomal - playerPos;
		D3DXVec3Normalize(&vAfterPos, &vAfterPos);
		*pCameraPos = playerPos + vAfterPos * 3.5f;
		pCameraPos->y = 3.5f;
		if (m_CameraSceneTime - fTimeDelta < PlayerSkillZ_LB_Time_1) {
			playerDir.y = 0.005f;
			D3DXVec3Normalize(&playerDir, &playerDir);
			*pCameraPos = playerPos + playerDir * 0.2f;
			pCameraPos->y += 0.5f;
			m_CameraBeforeNomal = m_CameraPosInit = *pCameraPos;
			m_CameraPosInit += playerDir * 2.5f;
			m_CameraPosInit.y -= 0.7f;
			m_CameraMoveDir = playerDir;
			//m_CameraPosInit = _vec3{ m_CameraPosInit.z , m_CameraPosInit.y, -m_CameraPosInit.x };
		}
	}
	else if (m_CameraSceneTime > PlayerSkillZ_LB_Time_2) {
		_vec3 vMoveDir;
		vMoveDir = m_CameraPosInit - m_CameraBeforeNomal;
		*pCameraPos += vMoveDir * fTimeDelta / (PlayerSkillZ_LB_Time_2 - PlayerSkillZ_LB_Time_3);
		//TODO
	}
	else if (m_CameraSceneTime > PlayerSkillZ_LB_Time_3) {

	}
	else if (m_CameraSceneTime > PlayerSkillZ_LB_Time_4) {
		*pCameraPos += m_CameraMoveDir * 25.f * fTimeDelta;
	}

}