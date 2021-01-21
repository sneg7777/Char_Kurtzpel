#include "stdafx.h"
#include "NpcQuest_Manager.h"
#include "Export_Function.h"

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

	return 0;
}

void Client::CNpcQuest_Manager::Render_Object(void)
{

}