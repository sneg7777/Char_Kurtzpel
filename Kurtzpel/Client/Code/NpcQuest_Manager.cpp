#include "stdafx.h"
#include "NpcQuest_Manager.h"
#include "Export_Function.h"
#include "UI_Manager.h"
#include "Portal.h"
#include "Stage_1.h"
#include "Stage_2.h"

CNpcQuest_Manager* CNpcQuest_Manager::m_pInstance = nullptr;

void CNpcQuest_Manager::Destroy_Instance()
{
	if (m_pInstance) {
		Engine::Safe_Release(m_pInstance->m_pGraphicDev);
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

CNpcQuest_Manager::CNpcQuest_Manager()
{

}

CNpcQuest_Manager::~CNpcQuest_Manager(void)
{

}



HRESULT Client::CNpcQuest_Manager::Ready_Object(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;
	Engine::Safe_AddRef(m_pGraphicDev);
	return S_OK;
}

Client::_int Client::CNpcQuest_Manager::Update_Object(const _float& fTimeDelta)
{
	QusetProgress(fTimeDelta);
	m_NpcQuestInfo.m_fTimeDelta = fTimeDelta;
	return 0;
}

void Client::CNpcQuest_Manager::Render_Object(void)
{

}

void Client::CNpcQuest_Manager::QusetProgress(const _float& fTimeDelta)
{
	if (m_NpcQuestInfo.m_QuestNumber == 1) {
		if (m_NpcQuestInfo.m_AttackCount > 2) {
			m_NpcQuestInfo.m_QuestNumber++;
			CUI_Manager::Get_Instance()->Create_QuestClear();
		}
	}
	else if (m_NpcQuestInfo.m_QuestNumber == 3) {
		if (m_NpcQuestInfo.m_SkillQCount > 0 && m_NpcQuestInfo.m_SkillECount > 0 && m_NpcQuestInfo.m_SkillFCount > 0) {
			m_NpcQuestInfo.m_QuestNumber++;
			CUI_Manager::Get_Instance()->Create_QuestClear();
		}
	}
	else if (m_NpcQuestInfo.m_QuestNumber == 5) {
		if (m_NpcQuestInfo.m_RollingCount > 0) {
			m_NpcQuestInfo.m_QuestNumber++;
			CUI_Manager::Get_Instance()->Create_QuestClear();
		}
	}
	else if (m_NpcQuestInfo.m_QuestNumber == 7) {
		if (m_NpcQuestInfo.m_DeadTwoHand > 0) {
			m_NpcQuestInfo.m_QuestNumber++;
			CUI_Manager::Get_Instance()->Create_QuestClear();
		}
	}
	else if (m_NpcQuestInfo.m_QuestNumber == 9) {
		if (m_NpcQuestInfo.m_DeadTwoHand > 3 && m_NpcQuestInfo.m_DeadTwoHand > 2) {
			m_NpcQuestInfo.m_QuestNumber++;
			CUI_Manager::Get_Instance()->Create_QuestClear();
		}
	}
	else if (m_NpcQuestInfo.m_QuestNumber == 11) {
		if (m_NpcQuestInfo.m_DeadApostle > 0) {
			m_NpcQuestInfo.m_QuestNumber++;
			CUI_Manager::Get_Instance()->Create_QuestClear();
			m_NpcQuestInfo.m_WeaponChange = true;
		}
	}
}

void Client::CNpcQuest_Manager::Portal_MapMove() {
	CNpcQuest_Manager::Get_Instance()->Get_NpcQuestInfo()->m_MapMove = false;
	CPortal* pPortal = dynamic_cast<CPortal*>(Engine::CManagement::GetInstance()->m_pScene->Get_LayerObject(Engine::CLayer::Layer_Static, Engine::CGameObject::UnitName::Portal));
	int portalMapNumber = pPortal->Get_PortalMapNumber();
	if (portalMapNumber == 1) {
		Engine::CScene* pScene = nullptr;
		pScene = CStage_1::Create(m_pGraphicDev);

		FAILED_CHECK_RETURN(Engine::SetUp_Scene(pScene), );
	}
	else if (portalMapNumber == 2) {
		Engine::CScene* pScene = nullptr;
		pScene = CStage_2::Create(m_pGraphicDev);

		FAILED_CHECK_RETURN(Engine::SetUp_Scene(pScene), );
	}
}

float Client::CNpcQuest_Manager::Get_TimeDelta() {
	return m_pInstance->m_NpcQuestInfo.m_fTimeDelta;
}