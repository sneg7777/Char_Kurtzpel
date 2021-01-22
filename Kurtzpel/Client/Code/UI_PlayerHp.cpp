#include "stdafx.h"
#include "UI_PlayerHp.h"
#include "Export_Function.h"
#include "UI_Manager.h"
#include "UI.h"

CUI_PlayerHp::CUI_PlayerHp(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
{

}

CUI_PlayerHp::~CUI_PlayerHp(void)
{

}


HRESULT Client::CUI_PlayerHp::Add_Component(void)
{
	CUI::Add_Component();

	return S_OK;
}

CUI_PlayerHp* CUI_PlayerHp::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CUI_PlayerHp*	pInstance = new CUI_PlayerHp(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CUI_PlayerHp::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CUI_PlayerHp::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	/*m_fX = 90.f;
	m_fY = 510.f;
	m_fSizeX = 81.6f;
	m_fSizeY = 97.2f;*/

	return S_OK;
}
Client::_int Client::CUI_PlayerHp::Update_Object(const _float& fTimeDelta)
{
	CPlayer* pPlayer = CPlayer::GetInstance();
	if (m_UIKind == UIKind::UIK_Player_Hp1) {
		float hpPer = (pPlayer->Get_sStat()->m_fHp / pPlayer->Get_sStat()->m_fMaxHp);
		m_fX = 390.8f + 121.2f * hpPer;
		m_fSizeX = (pPlayer->Get_sStat()->m_fHp / pPlayer->Get_sStat()->m_fMaxHp) * 242.4f;
	}
	else if (m_UIKind == UIKind::UIK_Player_Hp2) {
		float dashPer = (pPlayer->m_DashGauge / pPlayer->m_MaxDashGauge);
		m_fX = 390.8f + 121.2f * dashPer;
		m_fSizeX = (pPlayer->m_DashGauge / pPlayer->m_MaxDashGauge) * 242.4f;
	}
	else if (m_UIKind == UIKind::UIK_Player_Hp3) {
		float mpPer = (pPlayer->Get_sStat()->m_fMp / pPlayer->Get_sStat()->m_fMaxMp);
		m_fX = 390.8f + 121.2f * mpPer;
		m_fSizeX = (pPlayer->Get_sStat()->m_fMp / pPlayer->Get_sStat()->m_fMaxMp) * 242.4f;
	}

	CUI::Update_Object(fTimeDelta);

	return 0;
}