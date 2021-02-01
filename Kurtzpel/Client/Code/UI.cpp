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
	return S_OK;
}
Client::_int Client::CUI::Update_Object(const _float& fTimeDelta)
{
	if (CNpcQuest_Manager::Get_NpcQuestInfo()->m_PlayerTalk)
		return 0;
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
	//m_pTextureCom->Render_Texture();
	//m_pBufferCom->Render_Buffer();
	////////////////
	
	if (m_ShaderCheck) {
		LPD3DXEFFECT    pEffect = m_pShaderCom->Get_EffectHandle();
		NULL_CHECK(pEffect);
		Engine::Safe_AddRef(pEffect);

		FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

		pEffect->Begin(NULL, 0);
		pEffect->BeginPass(1);

		//m_pTextureCom->Render_Texture();
		m_pBufferCom->Render_Buffer();

		pEffect->EndPass();
		pEffect->End();

		Engine::Safe_Release(pEffect);
	}
	else {
		m_pTextureCom->Render_Texture();
		m_pBufferCom->Render_Buffer();
	}

	////////////////
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOriginView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOriginProj);
}

HRESULT Client::CUI::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("fTexCX", 68.f);
	pEffect->SetFloat("fTexCY", 81.f);
	pEffect->SetFloat("fDrawX", 0.f);
	pEffect->SetFloat("fDrawY", 0.f);
	pEffect->SetFloat("fDrawCX", 34.f);
	pEffect->SetFloat("fDrawCY", 40.5f);
	pEffect->SetFloat("fAlpha", 1.f);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture");

	return S_OK;
}