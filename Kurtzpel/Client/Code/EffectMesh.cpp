#include "stdafx.h"
#include "EffectMesh.h"
#include "Export_Function.h"
#include "Player.h"
#include "Stage_1.h"
#include "Stage_2.h"


CEffectMesh::CEffectMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit(pGraphicDev)
{

}

CEffectMesh::~CEffectMesh(void)
{

}

HRESULT Client::CEffectMesh::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

	//CUnit::Add_Component();

	//Mesh
	pComponent = m_sComponent.m_pStaticMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_GH_Trace03"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(Engine::RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

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

	m_sComponent.m_pTransformCom->m_vScale = { 0.1f, 0.1f, 0.1f };
	Engine::CGameObject::Update_Object(0.f);
	return S_OK;
}


CEffectMesh* CEffectMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffectMesh* pInstance = new CEffectMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CEffectMesh::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CEffectMesh::Ready_Object(void)
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
Client::_int Client::CEffectMesh::Update_Object(const _float& fTimeDelta)
{
	if (m_sStat.m_IsDead) {
		return 1;
	}
	m_sComponent.m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(1.f));
	m_EffectTime += fTimeDelta * 40.f;
	if (m_EffectTime > 16)
		m_EffectTime -= 16.f;
	float effectSize =  0.01f + m_EffectTime * 0.001f;
	m_sComponent.m_pTransformCom->m_vScale = { effectSize, effectSize, effectSize };
	//SetUp_OnTerrain();
	CUnit::Update_Object(fTimeDelta);
	m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}
void Client::CEffectMesh::Render_Object(void)
{
	LPD3DXEFFECT	 pEffect = m_sComponent.m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// ���� ���̴� ������ ���� �ִ� �ִ� �н��� ������ ����, ����ϴ� ���
	pEffect->BeginPass(0);

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	//m_pBufferCom->Render_Buffer();
	//m_sComponent.m_pStaticMeshCom->Render_Meshes(pEffect);
	m_sComponent.m_pStaticMeshCom->Render_MeshesEffect(pEffect, m_sComponent.m_pTextureCom);

	pEffect->EndPass();
	pEffect->End();


	Engine::Safe_Release(pEffect);
	/*_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pColliderCom->Render_Collider(Engine::COL_TRUE, &matWorld);*/
}

HRESULT Client::CEffectMesh::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
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
	//pEffect->SetFloat("g_uOffset", 0.5f);
	//pEffect->SetFloat("g_vOffset", 0.33f);
	//pEffect->SetFloat("g_uStep", (int)m_EffectTime % 4);
	//pEffect->SetFloat("g_vStep", (int)m_EffectTime / 4);
	
	return S_OK;
}

void Client::CEffectMesh::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{

}

void Client::CEffectMesh::Set_Collider(_vec3 _pos, float _scale, CSphereCollider::BoneTeam _team, int _portalMapNumber) {
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
