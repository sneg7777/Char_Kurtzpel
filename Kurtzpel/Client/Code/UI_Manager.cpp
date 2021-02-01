#include "stdafx.h"
#include "UI_Manager.h"
#include "Export_Function.h"
#include "UI_MonsterHp.h"
#include "UI_PlayerHp.h"
#include "UI_Skill.h"
#include "UI_Text.h"
//#include "UI.h"

CUI_Manager* CUI_Manager::m_pInstance = nullptr;

void CUI_Manager::Destroy_Instance()
{
	if (m_pInstance) {
		for (auto& _ui : m_pInstance->m_sListUI.m_ListUI) {
			Engine::Safe_Release(_ui);
		}
		m_pInstance->m_sListUI.m_ListUI.clear();
		for (auto& _ui : m_pInstance->m_sListUI.m_ListUI_MonsterHp) {
			Engine::Safe_Release(_ui);
		}
		m_pInstance->m_sListUI.m_ListUI_MonsterHp.clear();
		for (auto& _ui : m_pInstance->m_sListUI.m_ListUI_PlayerHp) {
			Engine::Safe_Release(_ui);
		}
		m_pInstance->m_sListUI.m_ListUI_PlayerHp.clear();
		for (auto& _ui : m_pInstance->m_sListUI.m_ListUI_Skill_ICon) {
			Engine::Safe_Release(_ui);
		}
		m_pInstance->m_sListUI.m_ListUI_Skill_ICon.clear();
		for (auto& _ui : m_pInstance->m_sListUI.m_ListUI_Text) {
			Engine::Safe_Release(_ui);
		}
		m_pInstance->m_sListUI.m_ListUI_Text.clear();

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
	//일반적인Ui
	for (auto& _ui : m_sListUI.m_ListUI)
	{
		_ui->Update_Object(fTimeDelta);
	}
	//플레이어체력Ui
	for (auto& _ui : m_sListUI.m_ListUI_PlayerHp)
	{
		_ui->Update_Object(fTimeDelta);
	}
	//몬스터체력Ui
	if (m_DamagedInitTime > 0.f) {
		for (auto& _ui : m_sListUI.m_ListUI_MonsterHp)
		{
			_ui->Update_Object(fTimeDelta);
		}
	}
	//스킬_Icon Ui
	for (auto& _ui : m_sListUI.m_ListUI_Skill_ICon)
	{
		_ui->Update_Object(fTimeDelta);
	}
	//Text
	for (auto& iter = m_sListUI.m_ListUI_Text.begin(); iter != m_sListUI.m_ListUI_Text.end();) {
		if (1 == (*iter)->Update_Object(fTimeDelta)) {
			Engine::Safe_Release(*iter);
			iter = m_sListUI.m_ListUI_Text.erase(iter);
		}
		else {
			iter++;
		}

	}

	if (m_DamagedInitTime > 0.f) {
		m_DamagedInitTime -= fTimeDelta;
		if (m_DamagedInitTime <= 0.f) {
			m_DamagedInitTime = 0.f;
			m_pDamagedEnemy = nullptr;
		}
	}
	return 0;
}
void Client::CUI_Manager::Render_Object(void)
{

}

void Client::CUI_Manager::Ready_CreateUI() {
	for (int i = 0; i < 18; i++)
	{
		CUI* pUI = CUI::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pUI, );
		Init_SkillIFrame(pUI, m_sListUI.m_ListUI.size());
		m_sListUI.m_ListUI.emplace_back(pUI);
	}
	for (int i = 0; i < 4; i++)
	{
		CUI_MonsterHp* pUI = CUI_MonsterHp::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pUI, );
		Init_MonsterHpBar(pUI, m_sListUI.m_ListUI_MonsterHp.size());
		m_sListUI.m_ListUI_MonsterHp.emplace_back(pUI);
	}
	for (int i = 0; i < 4; i++)
	{
		CUI_PlayerHp* pUI = CUI_PlayerHp::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pUI, );
		Init_PlayerHpBar(pUI, m_sListUI.m_ListUI_PlayerHp.size());
		m_sListUI.m_ListUI_PlayerHp.emplace_back(pUI);
	}
	for (int i = 0; i < 9; i++)
	{
		CUI_Skill* pUI = CUI_Skill::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pUI, );
		Init_Skill_ICon(pUI, m_sListUI.m_ListUI_Skill_ICon.size());
		m_sListUI.m_ListUI_Skill_ICon.emplace_back(pUI);
	}
	CUI_Manager::Get_Instance()->Create_Text(CUI::UIKind::UIK_TipText, L"Texture_Text_dncmrdp", true);
	CUI_Manager::Get_Instance()->Create_Text(CUI::UIKind::UIK_TipNpcTalk, L"Texture_Text_npcdhk");
	CUI_Manager::Get_Instance()->Create_Text(CUI::UIKind::UIK_TipPortal, L"Texture_Text_vhxkf");

}

void Client::CUI_Manager::Create_Text(CUI::UIKind _uiName, _tchar* _texTag, bool _alpha) {
	CUI_Text* pUI = CUI_Text::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pUI, );
	//
	Engine::CComponent* pComponent = nullptr;
	Engine::CTexture** pTextureCom = &pUI->m_pTextureCom;
	if (_uiName == CUI::UIKind::UIK_TipText)
		pUI->Set_PosToSize(512.f, 120.f, 384.f, 76.8f);
	else if (_uiName == CUI::UIKind::UIK_TipNpcTalk || _uiName == CUI::UIKind::UIK_TipPortal)
		pUI->Set_PosToSize(512.f, 150.f, 384.f, 76.8f);
	else
		pUI->Set_PosToSize(512.f, 500.f, 384.f, 76.8f);
	if (!_alpha)
		pUI->Set_Alpha(1.f);
	else
		pUI->Set_Alpha(0.f);
	pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, _texTag));
	pUI->m_UIKind = _uiName;
	pUI->Emplace_mapComponent(Engine::ID_STATIC, L"Com_Texture", pComponent);
	//
	m_sListUI.m_ListUI_Text.emplace_back(pUI);
}

void Client::CUI_Manager::Create_Art(CUI::UIKind _uiName, _tchar* _texTag) {
	CUI_Text* pUI = CUI_Text::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pUI, );
	//
	Engine::CComponent* pComponent = nullptr;
	Engine::CTexture** pTextureCom = &pUI->m_pTextureCom;
	pUI->Set_PosToSize(60.f, 310.f, 128.f, 256.f);
	pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, _texTag));
	pUI->m_UIKind = _uiName;
	pUI->Emplace_mapComponent(Engine::ID_STATIC, L"Com_Texture", pComponent);
	//
	m_sListUI.m_ListUI_Text.emplace_back(pUI);
}

void Client::CUI_Manager::Create_QuestClear() {
	Engine::CComponent* pComponent = nullptr;

	CUI_Text* pUI = CUI_Text::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pUI, );
	Engine::CTexture** pTextureCom = &pUI->m_pTextureCom;

	pUI->Set_PosToSize(60.f, 310.f, 128.f, 256.f);
	pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_LireArt"));
	pUI->m_UIKind = CUI::UIKind::UIK_Art;
	pUI->Set_Alpha(0.f);
	pUI->Emplace_mapComponent(Engine::ID_STATIC, L"Com_Texture", pComponent);
	m_sListUI.m_ListUI_Text.emplace_back(pUI);
	//
	pUI = CUI_Text::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pUI, );
	pTextureCom = &pUI->m_pTextureCom;

	pUI->Set_PosToSize(256.f, 310.f, 384.f, 76.8f);
	pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_Text_99wkfgotdj"));
	pUI->m_UIKind = CUI::UIKind::UIK_Art;
	pUI->Set_Alpha(0.f);
	pUI->Emplace_mapComponent(Engine::ID_STATIC, L"Com_Texture", pComponent);
	m_sListUI.m_ListUI_Text.emplace_back(pUI);

}

void Client::CUI_Manager::Init_MonsterHpBar(CUI_MonsterHp* pUI, int number) {
	Engine::CComponent* pComponent = nullptr;
	Engine::CTexture** pTextureCom = &pUI->m_pTextureCom;

	switch (number) {
		case 0: {
			pUI->Set_PosToSize(512.f, 95.f, 350.f, 24.5f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_Monster_Hp0"));
			pUI->m_UIKind = CUI::UIKind::UIK_Monster_Hp0;
			break;
		}
		case 1: {
			pUI->Set_PosToSize(512.f, 90.f, 343.f, 9.1f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_Monster_Hp2"));
			pUI->m_UIKind = CUI::UIKind::UIK_Monster_Hp2;
			break;
		}
		case 2: {
			pUI->Set_PosToSize(512.f, 90.f, 343.f, 9.1f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_Monster_Hp1"));
			pUI->m_UIKind = CUI::UIKind::UIK_Monster_Hp1;
			break;
		}
		case 3: {
			pUI->Set_PosToSize(512.f, 102.f, 343.f, 4.9f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_Monster_Hp3"));
			pUI->m_UIKind = CUI::UIKind::UIK_Monster_Hp3;
			break;
		}
	}
	pUI->Emplace_mapComponent(Engine::ID_STATIC, L"Com_Texture", pComponent);
}

void Client::CUI_Manager::Init_PlayerHpBar(CUI_PlayerHp* pUI, int number) {
	Engine::CComponent* pComponent = nullptr;
	Engine::CTexture** pTextureCom = &pUI->m_pTextureCom;
	switch (number) {
		case 0: {
			pUI->Set_PosToSize(512.f, 565.f, 248.8f, 33.6f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_Player_Hp0"));
			pUI->m_UIKind = CUI::UIKind::UIK_Player_Hp0;
			break;
		}
		case 1: {
			pUI->Set_PosToSize(512.f, 555.6f, 242.4f, 8.f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_Player_Hp1"));
			pUI->m_UIKind = CUI::UIKind::UIK_Player_Hp1;
			break;
		}
		case 2: {
			pUI->Set_PosToSize(512.f, 569.f, 242.4f, 4.8f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_Player_Hp2"));
			pUI->m_UIKind = CUI::UIKind::UIK_Player_Hp2;
			break;
		}
		case 3: {
			pUI->Set_PosToSize(512.f, 577.f, 242.4f, 4.8f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_Player_Hp3"));
			pUI->m_UIKind = CUI::UIKind::UIK_Player_Hp3;
			break;
		}
	}
	pUI->Emplace_mapComponent(Engine::ID_STATIC, L"Com_Texture", pComponent);
}

void Client::CUI_Manager::Init_Skill_ICon(CUI_Skill* pUI, int number) {
	Engine::CComponent* pComponent = nullptr;
	Engine::CTexture** pTextureCom = &pUI->m_pTextureCom;

	switch (number) {
		case 0: {
			pUI->Set_PosToSize(90.f, 506.f, 102.4f, 102.4f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_LBSkillZ"));
			pUI->m_UIKind = CUI::UIKind::UIK_SkillZ_LB;
			pUI->m_Weapon = CPlayer::Weapon_LongBow;
			break;
		}
		case 1: {
			pUI->Set_PosToSize(184.f, 484.f, 65.6f, 65.6f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_LBSkillQ"));
			pUI->m_UIKind = CUI::UIKind::UIK_SkillQ_LB;
			pUI->m_Weapon = CPlayer::Weapon_LongBow;
			break;
		}
		case 2: {
			pUI->Set_PosToSize(242.f, 484.f, 65.6f, 65.6f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_LBSkillE"));
			pUI->m_UIKind = CUI::UIKind::UIK_SkillE_LB;
			pUI->m_Weapon = CPlayer::Weapon_LongBow;
			break;
		}
		case 3: {
			pUI->Set_PosToSize(300.f, 484.f, 65.6f, 65.6f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_LBSkillF"));
			pUI->m_UIKind = CUI::UIKind::UIK_SkillF_LB;
			pUI->m_Weapon = CPlayer::Weapon_LongBow;
			break;
		}
		case 4: {
			pUI->Set_PosToSize(90.f, 506.f, 102.4f, 102.4f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillZ"));
			pUI->m_UIKind = CUI::UIKind::UIK_SkillZ_GH;
			pUI->m_Weapon = CPlayer::Weapon_Hammer;
			break;
		}
		case 5: {
			pUI->Set_PosToSize(184.f, 484.f, 65.6f, 65.6f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillQ"));
			pUI->m_UIKind = CUI::UIKind::UIK_SkillQ_GH;
			pUI->m_Weapon = CPlayer::Weapon_Hammer;
			break;
		}
		case 6: {
			pUI->Set_PosToSize(242.f, 484.f, 65.6f, 65.6f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillE"));
			pUI->m_UIKind = CUI::UIKind::UIK_SkillE_GH;
			pUI->m_Weapon = CPlayer::Weapon_Hammer;
			break;
		}
		case 7: {
			pUI->Set_PosToSize(300.f, 484.f, 65.6f, 65.6f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillF"));
			pUI->m_UIKind = CUI::UIKind::UIK_SkillF_GH;
			pUI->m_Weapon = CPlayer::Weapon_Hammer;
			break;
		}
		case 8: {
			pUI->Set_PosToSize(271.f, 534.f, 65.6f, 65.6f);
			pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_SkillWeaponChange"));
			pUI->m_UIKind = CUI::UIKind::UIK_SkillTab;
			break;
		}
	}
	pUI->Emplace_mapComponent(Engine::ID_STATIC, L"Com_Texture", pComponent);
}

void Client::CUI_Manager::Init_SkillIFrame(CUI* pUI, int number) {
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
		//pUI->Set_PosToSize(180.f, 490.f, 45.3f, 54.f);
		pUI->Set_PosToSize(184.f, 484.f, 54.4f, 64.8f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_SkillFrameBlue"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillFrameQCool;
		break;
	}
	case 7: {
		pUI->Set_PosToSize(242.f, 484.f, 54.4f, 64.8f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_SkillFrameBlue"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillFrameECool;
		break;
	}
	case 8: {
		pUI->Set_PosToSize(300.f, 484.f, 54.4f, 64.8f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_SkillFrameBlue"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillFrameFCool;
		break;
	}
	case 9: {
		pUI->Set_PosToSize(213.f, 534.f, 54.4f, 64.8f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_SkillFrameOrange"));
		pUI->m_UIKind = CUI::UIKind::UIK_FrameLShiftCool;
		break;
	}
	case 10: {
		pUI->Set_PosToSize(271.f, 534.f, 54.4f, 64.8f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_SkillFrameGreen"));
		pUI->m_UIKind = CUI::UIKind::UIK_FrameTabCool;
		break;
	}
	case 11: {
		pUI->Set_PosToSize(213.f, 534.f, 65.6f, 65.6f);
		pComponent = *pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UI_SkillAvoid"));
		pUI->m_UIKind = CUI::UIKind::UIK_SkillLShift;
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
	default:
		break;
	}
	pUI->Emplace_mapComponent(Engine::ID_STATIC, L"Com_Texture", pComponent);
}

CUI* Client::CUI_Manager::Get_UI(CUI::UIKind _uiKind)
{
	if (_uiKind == CUI::UIKind::UIK_TalkText || _uiKind == CUI::UIKind::UIK_TipText)
	{
		for (auto& _ui : m_sListUI.m_ListUI_Text)
		{
			if (_ui->m_UIKind == _uiKind) {
				return _ui;
			}
		}
	}
	else {
		for (auto& _ui : m_sListUI.m_ListUI)
		{
			if (_ui->m_UIKind == _uiKind) {
				return _ui;
			}
		}
	}

	return nullptr;
}

void CUI_Manager::Delete_TalkText()
{
	for (auto& iter = m_sListUI.m_ListUI_Text.begin(); iter != m_sListUI.m_ListUI_Text.end();)
	{
		if ((*iter)->m_UIKind == CUI::UIKind::UIK_TalkText) {
			Engine::Safe_Release(*iter);
			m_sListUI.m_ListUI_Text.erase(iter);
			return;
		}
		iter++;
	}
}

void CUI_Manager::Delete_QuestClearText()
{
	for (auto& iter = m_sListUI.m_ListUI_Text.begin(); iter != m_sListUI.m_ListUI_Text.end();)
	{
		if ((*iter)->m_UIKind == CUI::UIKind::UIK_Art) {
			Engine::Safe_Release(*iter);
			iter = m_sListUI.m_ListUI_Text.erase(iter);
		}
		else {
			iter++;
		}
	}
}
