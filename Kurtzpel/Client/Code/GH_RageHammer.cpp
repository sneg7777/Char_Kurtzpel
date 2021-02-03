#include "stdafx.h"
#include "GH_RageHammer.h"
#include "Export_Function.h"
#include "DynamicCamera.h"
#include "SphereCollider.h"
#include "Player.h"
#include "Stage.h"
#include "Random_Manager.h"
#include "UI_Manager.h"
#include "GH_RockIn.h"
#include "SkillCollider.h"

CGH_RageHammer::CGH_RageHammer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit_D(pGraphicDev)
{

}

CGH_RageHammer::~CGH_RageHammer(void)
{

}

HRESULT Client::CGH_RageHammer::Add_Component()
{
	Engine::CComponent*		pComponent = nullptr;

	CUnit_D::Add_Component();
	// Mesh
	
	pComponent = m_sComponent.m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_GH_RageHammer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	_vec3 vPos, vDir;
	CPlayer::GetInstance()->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	CPlayer::GetInstance()->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
	D3DXVec3Normalize(&vDir, &vDir);
	vPos += vDir * 10.f;
	m_sComponent.m_pTransformCom->Set_Pos(&vPos);
	Engine::CGameObject::Update_Object(0.f);
	
	
	pComponent = m_sComponent.m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_DissolveTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
	
	return S_OK;
}

HRESULT CGH_RageHammer::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_sComponent.m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	_vec4 vColor = { 0.f, 0.f, 0.f, 1.f };
	pEffect->SetVector("g_vColor", &vColor);
	pEffect->SetFloat("g_fBoldSize", 0.005f);

	if (m_Dissolve != 0.f) {
		pEffect->SetFloat("g_fTimeDelta", m_Dissolve);
		pEffect->SetBool("g_bIsDissolve", true);
		m_sComponent.m_pTextureCom->Set_Texture(pEffect, "g_DissolveTexture");
	}
	else {
		pEffect->SetFloat("g_fTimeDelta", 0.f);
		pEffect->SetBool("g_bIsDissolve", false);
	}
	
	

	return S_OK;
}


CGH_RageHammer* CGH_RageHammer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CGH_RageHammer*	pInstance = new CGH_RageHammer(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);
	return pInstance;
}

void CGH_RageHammer::Free(void)
{
	CUnit_D::Free();
}

HRESULT Client::CGH_RageHammer::Ready_Object()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_sComponent.m_pMeshCom->Set_AnimationSet(0);
	m_sComponent.m_pTransformCom->Set_Scale(0.06f, 0.06f, 0.06f);
	dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_Layer(Engine::CLayer::LayerName::Layer_DynamicNoColl)->Add_GameObject(L"GH_Rock", this);
	Sound_RandomPlay(UnitSound::Sound_GHAttack, 1.8f);
	return S_OK;
}


Client::_int Client::CGH_RageHammer::Update_Object(const _float& fTimeDelta)
{
	if (m_sStat.m_IsDead || m_sStat.m_IsDeadTime > 10.f) {
		return 1;
	}

	Calc_Time(fTimeDelta);

	SetUp_OnTerrain();
	//Pattern(fTimeDelta);

	if(m_sComponent.m_pMeshCom->Get_AnimationTrackPos() <= m_sComponent.m_pMeshCom->Get_Period() - 0.05f)
		m_sComponent.m_pMeshCom->Play_Animation(fTimeDelta * m_AniSpeed);
	else
		m_sComponent.m_pMeshCom->Play_Animation(0.f);

	if (!m_RockHit && m_sComponent.m_pMeshCom->Get_AnimationTrackPos() > 0.28f) {
		_vec3 vPos;
		m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
		CGH_RockIn::Create(m_pGraphicDev, false, vPos);
		vPos.y += 2.f;
		m_RockHit = true;
		CSkillCollider::Create(m_pGraphicDev)->Set_Collider(vPos, 8.f, CSphereCollider::BoneTeam_Player, CPlayer::GetInstance()->Get_sStat()->m_fAttack * 0.3f, 2.f, 0.1f);
	}
	CUnit_D::Update_Object(fTimeDelta);

	m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}

Client::_int Client::CGH_RageHammer::LateUpdate_Object(const _float& fTimeDelta)
{

	

	return 0;
}

void Client::CGH_RageHammer::Render_Object(void)
{
	m_sComponent.m_pMeshCom->Play_Animation(0.f);
	LPD3DXEFFECT	 pEffect = m_sComponent.m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;
	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
	for (_int i = 0; i < iMaxPass; i++)
	{
		pEffect->BeginPass(i);

		FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

		m_sComponent.m_pMeshCom->Render_Meshes(pEffect);

		pEffect->EndPass();
	}
	pEffect->End();

	Engine::Safe_Release(pEffect);
}

void Client::CGH_RageHammer::SetUp_OnTerrain(void)
{
	_vec3	vPosition;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_Terrain->Get_sComponent_Terrain()->m_pBufferCom;
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_sComponent.m_pCalculatorCom->Compute_HeightOnTerrain(&vPosition, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_sComponent.m_pTransformCom->Move_Pos(vPosition.x, fHeight, vPosition.z);
}


void Client::CGH_RageHammer::Calc_Time(_float fTimeDelta)
{
	m_sStat.m_IsDeadTime += fTimeDelta;
	if (m_Dissolve < 1.f && m_sStat.m_IsDeadTime > 0.9f) {
		m_Dissolve += fTimeDelta * 0.8f;
		if (m_Dissolve > 1.f)
			m_Dissolve = 1.f;
	}
}

void Client::CGH_RageHammer::Pattern(_float fTimeDelta)
{
	
}

void CGH_RageHammer::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{
	
}
