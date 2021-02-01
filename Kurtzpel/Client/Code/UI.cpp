#include "stdafx.h"
#include "UI.h"
#include "Export_Function.h"
#include "UI_Manager.h"
#include "NpcQuest_Manager.h"

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
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Proto_Shader_Texture"));
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
	m_pTransformCom->Set_Scale(0.4f, 0.1f, 0.1f);
	m_fAlpha = 0.f;
	return S_OK;
}
Client::_int Client::CUI::Update_Object(const _float& fTimeDelta)
{
	if (CNpcQuest_Manager::Get_NpcQuestInfo()->m_PlayerTalk)
		return 0;
	Engine::CGameObject::Update_Object(fTimeDelta);
		
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);

	if (m_fAlpha < 1.f) {
		m_fAlpha += fTimeDelta * 0.5f;
		if (m_fAlpha > 1.f)
			m_fAlpha = 1.f;
	}

	if (m_UIKind == CUI::UIKind::UIK_SkillFrameQCool) {
		m_RenderYPer = CPlayer::GetInstance()->m_TimeCheck[CPlayer::TimeCheck_Cool_Q] / COOLTIME_LB_Q;
	}
	else if (m_UIKind == CUI::UIKind::UIK_SkillFrameECool) {
		m_RenderYPer = CPlayer::GetInstance()->m_TimeCheck[CPlayer::TimeCheck_Cool_E] / COOLTIME_LB_E;
	}
	else if (m_UIKind == CUI::UIKind::UIK_SkillFrameFCool) {
		m_RenderYPer = CPlayer::GetInstance()->m_TimeCheck[CPlayer::TimeCheck_Cool_F] / COOLTIME_LB_F;
	}
	else if (m_UIKind == CUI::UIKind::UIK_FrameLShiftCool) {
		m_RenderYPer = CPlayer::GetInstance()->m_TimeCheck[CPlayer::TimeCheck_Cool_LSHIFT] / COOLTIME_LSHIFT;
	}
	else if (m_UIKind == CUI::UIKind::UIK_FrameTabCool) {
		m_RenderYPer = CPlayer::GetInstance()->m_TimeCheck[CPlayer::TimeCheck_Cool_Tab] / COOLTIME_TAB;
	}
	//D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	return 0;
}
void Client::CUI::Render_Object(void)
{


	//_matrix		matWorld, matView, matOriginView, matOriginProj;

	//m_pGraphicDev->GetTransform(D3DTS_VIEW, &matOriginView);
	//m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOriginProj);

	//D3DXMatrixIdentity(&matWorld);
	//D3DXMatrixIdentity(&matView);

	//matWorld._11 = m_fSizeX;
	//matWorld._22 = m_fSizeY;
	//matWorld._33 = 1.f;
	//matWorld._41 = m_fX - WINCX * 0.5f;
	//matWorld._42 = -m_fY + WINCY * 0.5f;

	//m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	//m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	//m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
	////m_pTextureCom->Render_Texture();
	////m_pBufferCom->Render_Buffer();
	//////////////////
	//
	//if (m_ShaderCheck) {
	//	LPD3DXEFFECT    pEffect = m_pShaderCom->Get_EffectHandle();
	//	NULL_CHECK(pEffect);
	//	Engine::Safe_AddRef(pEffect);

	//	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	//	pEffect->Begin(NULL, 0);
	//	pEffect->BeginPass(1);

	//	//m_pTextureCom->Render_Texture();
	//	m_pBufferCom->Render_Buffer();

	//	pEffect->EndPass();
	//	pEffect->End();

	//	Engine::Safe_Release(pEffect);
	//}
	//else {
	//	m_pTextureCom->Render_Texture();
	//	m_pBufferCom->Render_Buffer();
	//}

	//////////////////
	//m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOriginView);
	//m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOriginProj);

	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);
	LPD3DXEFFECT    pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(0);
	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	pEffect->CommitChanges();

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);
	//m_pGraphicDev->set

}

HRESULT Client::CUI::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_vec3 vPos = { m_fX, m_fY, 0.f };
	_matrix matWorld, matView;

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);

	_matrix matScale, matRotZ, matTrans;
	D3DXMatrixScaling(&matScale, m_fSizeX, m_fSizeY, 1.f);
	//D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(m_fAngle));
	D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(0.f));
	D3DXMatrixTranslation(&matTrans, vPos.x - WINCX * 0.5f, -vPos.y + WINCY * 0.5f, 0.f);

	matWorld = matScale * matRotZ * matTrans;

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &m_matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);
	if (m_RenderYPer != 1.f) {
		pEffect->SetFloat("g_fRatioY", m_RenderYPer);
	}
	else {
		pEffect->SetFloat("g_fRatioY", 1.f);
	}


	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture");

	return S_OK;
}