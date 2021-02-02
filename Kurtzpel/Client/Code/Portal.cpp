#include "stdafx.h"
#include "Portal.h"
#include "Export_Function.h"
#include "Player.h"
#include "Stage_1.h"
#include "Stage_2.h"


CPortal::CPortal(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit(pGraphicDev)
{

}

CPortal::~CPortal(void)
{

}

HRESULT Client::CPortal::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

	//CUnit::Add_Component();

	//Mesh
	pComponent = m_sComponent.m_pStaticMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_GH_Trace01"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	//Texture
	pComponent = m_sComponent.m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_Effect_Portal"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Transform
	pComponent = m_sComponent.m_pTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Clone(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_sComponent.m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Safe_AddRef(pComponent);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	//pComponent = m_sComponent.m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Proto_Shader_Mesh"));
	pComponent = m_sComponent.m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Proto_Shader_EffectMesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	m_sComponent.m_pTransformCom->m_vScale = { 0.015f, 0.015f, 0.015f };
	Engine::CGameObject::Update_Object(0.f);
	return S_OK;
}


CPortal* CPortal::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPortal* pInstance = new CPortal(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CPortal::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CPortal::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_UnitName = UnitName::Portal;
	//
	//_vec3 TempPos;
	//m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &TempPos);
	//TempPos.y = 0.5f;
	//m_sComponent.m_pTransformCom->Set_Pos(&TempPos, true);
	//
	return S_OK;
}
Client::_int Client::CPortal::Update_Object(const _float& fTimeDelta)
{
	if (m_sStat.m_IsDead) {
		return 1;
	}

	m_EffectTime += fTimeDelta * 20.f;
	if (m_EffectTime > 16)
		m_EffectTime -= 16.f;
	//SetUp_OnTerrain();
	CUnit::Update_Object(fTimeDelta);
	m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}
void Client::CPortal::Render_Object(void)
{
	LPD3DXEFFECT	 pEffect = m_sComponent.m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
	pEffect->BeginPass(0);

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	//m_sComponent.m_pStaticMeshCom->Render_Meshes(pEffect);
	//m_sComponent.m_pStaticMeshCom->Render_MeshesEffect(pEffect, m_sComponent.m_pTextureCom);

	pEffect->EndPass();
	pEffect->End();


	Engine::Safe_Release(pEffect);
	/*_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pColliderCom->Render_Collider(Engine::COL_TRUE, &matWorld);*/
}

HRESULT Client::CPortal::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_sComponent.m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_bIsDissolve", false);
	pEffect->SetFloat("g_uOffset", 0.25f);
	pEffect->SetFloat("g_vOffset", 0.25f);
	pEffect->SetFloat("g_uStep", (int)m_EffectTime % 4);
	pEffect->SetFloat("g_vStep", (int)m_EffectTime / 4);
	
	return S_OK;
}

void Client::CPortal::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{
	if (CSphereCollider::BoneTeam_Player != _colSphere->m_BoneTeam)
		return;

	//if (_mySphere->m_BonePart == CSphereCollider::BonePart_CollBody) {

	//}

	if (_mySphere->m_BonePart == CSphereCollider::BonePart_Portal) {
		if (_colSphere->m_BonePart == CSphereCollider::BonePart_CollBody) {
			CNpcQuest_Manager::Get_NpcQuestInfo()->m_PortalColl = true;
		}
	}
}

void Client::CPortal::Set_Collider(_vec3 _pos, float _scale, CSphereCollider::BoneTeam _team, int _portalMapNumber) {
	m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS] = _pos;
	m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS].y += 2.f;
	CUnit::Update_Object(0.f);
	CSphereCollider* sphereCol = CSphereCollider::Create(m_pGraphicDev);
	sphereCol->Set_Transform(_scale);
	sphereCol->m_pStaticThis = this;
	sphereCol->m_WeaponAttack = false;
	sphereCol->m_BoneTeam = _team;
	sphereCol->m_BonePart = CSphereCollider::BonePart::BonePart_Portal;
	m_PortalMapNumber = _portalMapNumber;
	sphereCol->Engine::CGameObject::Update_Object(0.f);
	m_VecSphereCollider.emplace_back(sphereCol);

	//Engine::CGameObject* pGameObject = this;
	//Engine::CLayer* pLayer = Engine::CManagement::GetInstance()->m_pScene->Get_Layer(Engine::CLayer::LayerName::Layer_Static);
	//pLayer->Add_GameObject(L"PortalCollider", pGameObject);
}
