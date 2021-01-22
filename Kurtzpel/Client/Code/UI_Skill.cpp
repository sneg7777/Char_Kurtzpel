#include "stdafx.h"
#include "UI_Skill.h"
#include "Export_Function.h"
#include "UI_Manager.h"
#include "UI.h"

CUI_Skill::CUI_Skill(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
{

}

CUI_Skill::~CUI_Skill(void)
{

}


HRESULT Client::CUI_Skill::Add_Component(void)
{
	CUI::Add_Component();

	return S_OK;
}

CUI_Skill* CUI_Skill::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CUI_Skill*	pInstance = new CUI_Skill(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CUI_Skill::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CUI_Skill::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	/*m_fX = 90.f;
	m_fY = 510.f;
	m_fSizeX = 81.6f;
	m_fSizeY = 97.2f;*/

	return S_OK;
}
Client::_int Client::CUI_Skill::Update_Object(const _float& fTimeDelta)
{
	if (m_Weapon != CPlayer::Weapon_None && m_Weapon != CPlayer::GetInstance()->m_WeaponEquip) {
		return 0;
	}

	CUI::Update_Object(fTimeDelta);
	
	return 0;
}