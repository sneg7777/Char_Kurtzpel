#include "stdafx.h"
#include "SkyBox.h"
#include "Export_Function.h"

CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CSkyBox::~CSkyBox(void)
{

}

HRESULT Client::CSkyBox::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CCubeTex*>(Engine::Clone(Engine::RESOURCE_STATIC, L"Buffer_CubeTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_SkyBox"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

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
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Proto_Shader_SkyBox"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	return S_OK;
}

HRESULT CSkyBox::SetUp_ConstantTable(LPD3DXEFFECT & pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture", 3);

	return S_OK;
}

CSkyBox* CSkyBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSkyBox*	pInstance = new CSkyBox(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CSkyBox::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CSkyBox::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(40.f, 40.f, 40.f);

	return S_OK;
}
Client::_int Client::CSkyBox::Update_Object(const _float& fTimeDelta)
{
	_matrix	matCamWorld;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);
	// 카메라 월드 행렬
	D3DXMatrixInverse(&matCamWorld, NULL, &matCamWorld);

	m_pTransformCom->Move_Pos(matCamWorld._41, matCamWorld._42 + 3.f, matCamWorld._43);


	Engine::CGameObject::Update_Object(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_PRIORITY, this);

	return 0;
}
void Client::CSkyBox::Render_Object(void)
{
	LPD3DXEFFECT		pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(0);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);
}
