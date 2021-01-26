#include "stdafx.h"
#include "Terrain.h"
#include "Export_Function.h"
#include "Player.h"
#include "Portal.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CTerrain::~CTerrain(void)
{

}

HRESULT Client::CTerrain::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

	// buffer
	pComponent = m_sComponent.m_pBufferCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Clone(Engine::RESOURCE_STATIC, L"Buffer_TerrainTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// texture
	pComponent = m_sComponent.m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_Terrain"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// filtertexture
	pComponent = m_sComponent.m_pFilterCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_Filter"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Filter", pComponent);

	// Auratexture
	pComponent = m_sComponent.m_pAuraCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_SkillE_Terrain"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Aura", pComponent);

	// Renderer
	pComponent = m_sComponent.m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Safe_AddRef(pComponent);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Transform
	pComponent = m_sComponent.m_pTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Clone(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Optimization
	//pComponent = m_pOptimizationCom = dynamic_cast<Engine::COptimization*>(Engine::Clone(L"Proto_Optimization"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Optimization", pComponent);

	// Shader
	pComponent = m_sComponent.m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Proto_Shader_Terrain"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);


	return S_OK;
}

CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTerrain* pInstance = new CTerrain(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CTerrain::Free(void)
{
	Engine::Safe_Delete_Array(m_pIndex);

	Engine::CGameObject::Free();
}


HRESULT CTerrain::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pIndex = new Engine::INDEX32[m_sComponent.m_pBufferCom->Get_TriCnt()];

	return S_OK;
}
_int Client::CTerrain::Update_Object(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);

	//m_pOptimizationCom->Is_InFrustumForTerrain(m_pBufferCom->Get_VtxPos(),
	//	m_pBufferCom->Get_VtxCntX(),
	//	m_pBufferCom->Get_VtxCntZ(),
	//	m_pIndex,
	//	&m_dwTriCnt);

	m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}
void CTerrain::Render_Object(void)
{
	// m_pBufferCom->Copy_Indices(m_pIndex, m_dwTriCnt);

	LPD3DXEFFECT	 pEffect = m_sComponent.m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
	pEffect->BeginPass(0);

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	m_sComponent.m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Engine::Safe_Release(pEffect);

}

HRESULT CTerrain::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_sComponent.m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_sComponent.m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture");
	m_sComponent.m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture1", 1);
	m_sComponent.m_pFilterCom->Set_Texture(pEffect, "g_FilterTexture");

	m_sComponent.m_pAuraCom->Set_Texture(pEffect, "g_AuraTexture");

	const D3DLIGHT9* pLightInfo = Engine::Get_Light(0);

	pEffect->SetVector("g_vLightDir", &_vec4(pLightInfo->Direction, 0.f));

	pEffect->SetVector("g_LightDiffuse", (_vec4*)&pLightInfo->Diffuse);
	pEffect->SetVector("g_LightSpecular", (_vec4*)&pLightInfo->Specular);
	pEffect->SetVector("g_LightAmbient", (_vec4*)&pLightInfo->Ambient);

	D3DMATERIAL9			tMtrlInfo;
	ZeroMemory(&tMtrlInfo, sizeof(D3DMATERIAL9));

	tMtrlInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrlInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrlInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrlInfo.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrlInfo.Power = 20.f;

	pEffect->SetVector("g_MtrlDiffuse", (_vec4*)&tMtrlInfo.Diffuse);
	pEffect->SetVector("g_MtrlSpecular", (_vec4*)&tMtrlInfo.Specular);
	pEffect->SetVector("g_MtrlAmbient", (_vec4*)&tMtrlInfo.Ambient);

	pEffect->SetFloat("g_fPower", tMtrlInfo.Power);

	D3DXMatrixInverse(&matView, NULL, &matView);

	_vec4	vCamPos;
	memcpy(&vCamPos, &matView.m[3][0], sizeof(_vec4));

	pEffect->SetVector("g_vCamPos", &vCamPos);

	_float	fDetail = 20.f;

	pEffect->SetFloat("g_fDetail", fDetail);

#pragma region Aura
	CPlayer* pPlayer = CPlayer::GetInstance();
	
	pPlayer = CPlayer::GetInstance();
	//Engine::CTransform* pPlayerTransform = pPlayer->m_pTransformCom;//dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", Engine::ID_DYNAMIC));
	//NULL_CHECK_RETURN(pPlayerTransform, E_FAIL);

	//_vec3	vPlayerPos;
	//pPlayerTransform->Get_Info(Engine::INFO_POS, &vPlayerPos);

	//float lookatY = pPlayer->m_LookAtY;
	//_vec3 dir = pPlayer->m_sStat.m_vDir;
	if (pPlayer->m_bCheck[CPlayer::bCheck::bCheck_LB_SkillE] || (pPlayer->m_TimeCheck[CPlayer::TimeCheck_LB_SkillE_Attack_Arrow_Start] > 0.f))
		pEffect->SetVector("g_vPlayerPos", &_vec4(pPlayer->m_LB_SkillE_Pos, 1.f));
	else
		pEffect->SetVector("g_vPlayerPos", &_vec4(_vec3{0.f, 0.f, 0.f}, 1.f));
	_float	fRange = 6.f;
	pEffect->SetFloat("g_fRange", fRange);
	///////////////////////////////////////////////////////////////////////////////////////////
	CPortal* pPortal = dynamic_cast<CPortal*>(Engine::CManagement::GetInstance()->m_pScene->Get_LayerObject(Engine::CLayer::Layer_Static, Engine::CGameObject::UnitName::Portal));
	if (pPortal == nullptr)
		return S_OK;

	///////////////////////////////////////////////////////////////////////////////////////////
	
#pragma endregion Aura

	return S_OK;
}
