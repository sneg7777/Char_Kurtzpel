#include "stdafx.h"
#include "UI_Text.h"
#include "Export_Function.h"
#include "UI_Manager.h"
#include "UI.h"

CUI_Text::CUI_Text(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUI(pGraphicDev)
{
	m_EffectTime = 0.f;
}

CUI_Text::~CUI_Text(void)
{

}


HRESULT Client::CUI_Text::Add_Component(void)
{
	CUI::Add_Component();

	return S_OK;
}

CUI_Text* CUI_Text::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CUI_Text*	pInstance = new CUI_Text(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CUI_Text::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CUI_Text::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}
Client::_int Client::CUI_Text::Update_Object(const _float& fTimeDelta)
{
	if (m_UIKind == UIKind::UIK_TipText) {
		if (m_EffectTime < 10.f) {
			m_EffectTime += fTimeDelta * 2.f;

		}
		else {
			return 1;
		}

		if (m_EffectTime >= 1.f) {
			//m_fSizeX = 307.2f;
			//m_fSizeX = 327.68f;
			m_fSizeX = 384.f;
			m_fSizeY = 76.8;
		}
		else if (m_EffectTime < 0.5f) {
			m_fSizeX = 384.f * m_EffectTime * 2.f;
			m_fSizeY = 76.8 * 0.1f;
		}
		else {
			m_fSizeX = 384.f;
			m_fSizeY = 76.8 * (m_EffectTime - 0.5f) * 2.f;
		}
	}
	Engine::CGameObject::Update_Object(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	return 0;
}