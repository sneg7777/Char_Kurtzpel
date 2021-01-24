#include "stdafx.h"
#include "NpcQuest_Manager.h"
#include "Export_Function.h"
#include "UI_Manager.h"

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
		if (m_NpcQuestInfo.m_DeadTwoHand > 1) {
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
