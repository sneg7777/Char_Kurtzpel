#include "stdafx.h"
#include "UI_Manager.h"
#include "Export_Function.h"
//#include "UI.h"

CUI_Manager* CUI_Manager::m_pInstance = nullptr;

void CUI_Manager::Destroy_Instance()
{
	if (m_pInstance) {
		for (auto& _ui : m_pInstance->m_ListUI) {
			_ui->Release();
		}
		m_pInstance->m_ListUI.clear();
		Engine::Safe_Release(m_pInstance->m_pGraphicDev);
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

CUI_Manager::CUI_Manager()
{

}

CUI_Manager::~CUI_Manager(void)
{

}



HRESULT Client::CUI_Manager::Ready_Object(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;
	Engine::Safe_AddRef(m_pGraphicDev);
	return S_OK;
}
Client::_int Client::CUI_Manager::Update_Object(const _float& fTimeDelta)
{
	for (auto& _ui : m_ListUI)
	{
		_ui->Update_Object(fTimeDelta);
	}

	return 0;
}
void Client::CUI_Manager::Render_Object(void)
{

}

void Client::CUI_Manager::Ready_SkillIcon() {
	for (int i = 0; i < 22; i++)
	{
		CUI* pUI = CUI::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pUI, );
		Init_SkillIcon(pUI, m_ListUI.size());
		m_ListUI.emplace_back(pUI);
	}
}

void Client::CUI_Manager::Init_SkillIcon(CUI* pUI, int number) {
	Engine::CComponent* pComponent = nullptr;
	Engine::CTexture** pTextureCom = &pUI->m_pTextureCom;
	
	switch (number)
	{
	case 0: {
		//pUI->Set_PosToSize(90.f, 510.f, 86.6f, 100.f);
		pUI->Set_PosToSize(90.f, 506.f, 104.f, 120.f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_RageSkillFrame"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillFrameZ;
		break;
	}
	case 1: {
		//pUI->Set_PosToSize(180.f, 490.f, 45.3f, 54.f);
		pUI->Set_PosToSize(184.f, 484.f, 54.4f, 64.8f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_SkillFrame"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillFrameQ;
		break;
	}
	case 2: {
		pUI->Set_PosToSize(242.f, 484.f, 54.4f, 64.8f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_SkillFrame"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillFrameE;
		break;
	}
	case 3: {
		pUI->Set_PosToSize(300.f, 484.f, 54.4f, 64.8f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_SkillFrame"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillFrameF;
		break;
	}
	case 4: {
		pUI->Set_PosToSize(213.f, 534.f, 54.4f, 64.8f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_SkillFrame"));
		pUI->m_UIKind = CUI::UIKind::UIK_FrameLShift;
		break;
	}
	case 5: {
		pUI->Set_PosToSize(271.f, 534.f, 54.4f, 64.8f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_SkillFrame"));
		pUI->m_UIKind = CUI::UIKind::UIK_FrameTab;
		break;
	}
	case 6: {
		pUI->Set_PosToSize(90.f, 506.f, 102.4f, 102.4f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillZ"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillZ_GH;
		pUI->m_Weapon = CPlayer::Weapon_Hammer;
		break;
	}
	case 7: {
		pUI->Set_PosToSize(184.f, 484.f, 65.6f, 65.6f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillQ"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillQ_GH;
		pUI->m_Weapon = CPlayer::Weapon_Hammer;
		break;
	}
	case 8: {
		pUI->Set_PosToSize(242.f, 484.f, 65.6f, 65.6f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillE"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillE_GH;
		pUI->m_Weapon = CPlayer::Weapon_Hammer;
		break;
	}
	case 9: {
		pUI->Set_PosToSize(300.f, 484.f, 65.6f, 65.6f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillF"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillF_GH;
		pUI->m_Weapon = CPlayer::Weapon_Hammer;
		break;
	}
	case 10: {
		pUI->Set_PosToSize(213.f, 534.f, 65.6f, 65.6f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_SkillAvoid"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillLShift;
		break;
	}
	case 11: {
		pUI->Set_PosToSize(271.f, 534.f, 65.6f, 65.6f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_SkillWeaponChange"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillTab;
		break;
	}
	case 12: {
		pUI->Set_PosToSize(165.f, 446.f, 19.2f, 19.2f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_KeyQ"));
		pUI->m_UIKind = CUI::UIKind::UIK_KeyQ;
		break;
	}
	case 13: {
		pUI->Set_PosToSize(223.f, 446.f, 19.2f, 19.2f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_KeyE"));
		pUI->m_UIKind = CUI::UIKind::UIK_KeyE;
		break;
	}
	case 14: {
		pUI->Set_PosToSize(281.f, 446.f, 19.2f, 19.2f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_KeyF"));
		pUI->m_UIKind = CUI::UIKind::UIK_KeyF;
		break;
	}
	case 15: {
		pUI->Set_PosToSize(35.f, 446.f, 19.2f, 19.2f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_KeyZ"));
		pUI->m_UIKind = CUI::UIKind::UIK_KeyZ;
		break;
	}
	case 16: {
		pUI->Set_PosToSize(212.f, 580.f, 34.3f, 19.2f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_KeyLShift"));
		pUI->m_UIKind = CUI::UIKind::UIK_KeyLShift;
		break;
	}
	case 17: {
		pUI->Set_PosToSize(271.f, 580.f, 22.4f, 19.2f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_KeyTab"));
		pUI->m_UIKind = CUI::UIKind::UIK_KeyTab;
		break;
	}
	case 18: {
		pUI->Set_PosToSize(90.f, 506.f, 102.4f, 102.4f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_LBSkillZ"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillZ_LB;
		pUI->m_Weapon = CPlayer::Weapon_LongBow;
		break;
	}
	case 19: {
		pUI->Set_PosToSize(184.f, 484.f, 65.6f, 65.6f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_LBSkillQ"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillQ_LB;
		pUI->m_Weapon = CPlayer::Weapon_LongBow;
		break;
	}
	case 20: {
		pUI->Set_PosToSize(242.f, 484.f, 65.6f, 65.6f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_LBSkillE"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillE_LB;
		pUI->m_Weapon = CPlayer::Weapon_LongBow;
		break;
	}
	case 21: {
		pUI->Set_PosToSize(300.f, 484.f, 65.6f, 65.6f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_LBSkillF"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillF_LB;
		pUI->m_Weapon = CPlayer::Weapon_LongBow;
		break;
	}
	default:
		break;
	}
	pUI->Emplace_mapComponent(Engine::ID_STATIC, L"Com_Texture", pComponent);
}

CUI* Client::CUI_Manager::Get_UI(CUI::UIKind _uiKind)
{
	for (auto& _ui : m_ListUI)
	{
		if (_ui->m_UIKind == _uiKind) {
			return _ui;
		}
	}
	return nullptr;
}
