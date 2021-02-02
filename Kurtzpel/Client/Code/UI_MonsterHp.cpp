#include "stdafx.h"
#include "UI_MonsterHp.h"
#include "Export_Function.h"
#include "UI_Manager.h"
#include "UI.h"

CUI_MonsterHp::CUI_MonsterHp(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
{

}

CUI_MonsterHp::~CUI_MonsterHp(void)
{

}


HRESULT Client::CUI_MonsterHp::Add_Component(void)
{
	CUI::Add_Component();

	return S_OK;
}

CUI_MonsterHp* CUI_MonsterHp::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CUI_MonsterHp*	pInstance = new CUI_MonsterHp(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CUI_MonsterHp::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CUI_MonsterHp::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	/*m_fX = 90.f;
	m_fY = 510.f;
	m_fSizeX = 81.6f;
	m_fSizeY = 97.2f;*/

	return S_OK;
}

_int CUI_MonsterHp::Update_Object(const _float & fTimeDelta)
{
	CUnit* pEnemy = CUI_Manager::Get_Instance()->Get_DamagedEnemy();

	if (pEnemy == nullptr && m_UIKind != UIKind::UIK_Monster_Hp0)
		return 0;
	if (CUI_Manager::Get_Instance()->Get_DamagedTime() <= 0.f)
		return 0;

	if (m_UIKind == UIKind::UIK_Monster_Hp1) {
		float hpPer = (pEnemy->Get_sStat()->m_fHp / pEnemy->Get_sStat()->m_fMaxHp);
		m_fX = 468.5f + 171.5f * hpPer;
		m_fSizeX = (pEnemy->Get_sStat()->m_fHp / pEnemy->Get_sStat()->m_fMaxHp) * 343.f;
	}
	else if (m_UIKind == UIKind::UIK_Monster_Hp2) {
		float hpPer = (pEnemy->Get_sStat()->m_fDelayHp / pEnemy->Get_sStat()->m_fMaxHp);
		m_fX = 468.5f + 171.5f * hpPer;
		m_fSizeX = (pEnemy->Get_sStat()->m_fDelayHp / pEnemy->Get_sStat()->m_fMaxHp) * 343.f;
	}
	else if (m_UIKind == UIKind::UIK_Monster_Hp3) {
		float knockPer = (pEnemy->Get_sStat()->m_fKnockBackHp / pEnemy->Get_sStat()->m_fMaxKnockBackHp);
		m_fX = 468.5f + 171.5f * knockPer;
		m_fSizeX = (pEnemy->Get_sStat()->m_fKnockBackHp / pEnemy->Get_sStat()->m_fMaxKnockBackHp) * 343.f;
	}
	
	CUI::Update_Object(fTimeDelta);

	return 0;
}