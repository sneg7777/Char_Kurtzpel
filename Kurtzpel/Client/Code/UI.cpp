#include "stdafx.h"
#include "UI.h"
#include "Export_Function.h"
#include "UI_Manager.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CUI::~CUI(void)
{

}


HRESULT Client::CUI::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(Engine::RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// texture
	//pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_UISkill"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Safe_AddRef(pComponent);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Clone(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Proto_Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);
	return S_OK;
}

CUI* CUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CUI*	pInstance = new CUI(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CUI::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CUI::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	/*m_fX = 90.f;
	m_fY = 510.f;
	m_fSizeX = 81.6f;
	m_fSizeY = 97.2f;*/

	return S_OK;
}
Client::_int Client::CUI::Update_Object(const _float& fTimeDelta)
{
	if (m_Weapon != CPlayer::Weapon_None && m_Weapon != CPlayer::GetInstance()->m_WeaponEquip) {
		return 0;
	}
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
	//////
	if (m_UIKind == UIKind::UIK_Monster_Hp0 || m_UIKind == UIKind::UIK_Monster_Hp1 || m_UIKind == UIKind::UIK_Monster_Hp2 || m_UIKind == UIKind::UIK_Monster_Hp3)
	{
		CUnit* pEnemy = CUI_Manager::Get_Instance()->Get_DamagedEnemy();
		if (pEnemy == nullptr && m_UIKind != UIKind::UIK_Monster_Hp0)
			return 0;
		if (CUI_Manager::Get_Instance()->Get_DamagedTime() <= 0.f)
			return 0;

		if (m_UIKind == UIKind::UIK_Monster_Hp1) {
			float hpPer = (pEnemy->Get_sStat()->m_fHp / pEnemy->Get_sStat()->m_fMaxHp);
			m_fX = 340.5f + 171.5f * hpPer;
			m_fSizeX = (pEnemy->Get_sStat()->m_fHp / pEnemy->Get_sStat()->m_fMaxHp) * 343.f;
		}
		else if (m_UIKind == UIKind::UIK_Monster_Hp2) {
			float hpPer = (pEnemy->Get_sStat()->m_fDelayHp / pEnemy->Get_sStat()->m_fMaxHp);
			m_fX = 340.5f + 171.5f * hpPer;
			m_fSizeX = (pEnemy->Get_sStat()->m_fDelayHp / pEnemy->Get_sStat()->m_fMaxHp) * 343.f;
		}
		else if (m_UIKind == UIKind::UIK_Monster_Hp3) {
			float knockPer = (pEnemy->Get_sStat()->m_fKnockBackHp / pEnemy->Get_sStat()->m_fMaxKnockBackHp);
			m_fX = 340.5f + 171.5f * knockPer;
			m_fSizeX = (pEnemy->Get_sStat()->m_fKnockBackHp / pEnemy->Get_sStat()->m_fMaxKnockBackHp) * 343.f;
		}
	}

	Engine::CGameObject::Update_Object(fTimeDelta);
		
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	return 0;
}
void Client::CUI::Render_Object(void)
{
	_matrix		matWorld, matView, matOriginView, matOriginProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matOriginView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOriginProj);

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);

	matWorld._11 = m_fSizeX;
	matWorld._22 = m_fSizeY;
	matWorld._33 = 1.f;
	matWorld._41 = m_fX - WINCX * 0.5f;
	matWorld._42 = -m_fY + WINCY * 0.5f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOriginView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOriginProj);
}
