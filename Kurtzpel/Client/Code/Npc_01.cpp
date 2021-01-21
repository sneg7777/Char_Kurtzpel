#include "stdafx.h"
#include "Npc_01.h"
#include "Export_Function.h"
#include "DynamicCamera.h"
#include "SphereCollider.h"
#include "Player.h"
#include "Stage.h"
#include "Unit_D.h"
#include "Random_Manager.h"
#include "UI_Manager.h"


CNpc_01::CNpc_01(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit_D(pGraphicDev)
{
	//m_sStat.m_dwNaviIndex = 0;
}

CNpc_01::~CNpc_01(void)
{

}

HRESULT Client::CNpc_01::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	CUnit_D::Add_Component();
	// Mesh
	pComponent = m_sComponent.m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Player"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);
	m_sComponent.m_pMeshCom->Set_AniAngle(95.f);
	//m_pMeshCom->Set_AniAngle(275.f);
	//
	m_sComponent.m_pTransformCom->Set_Pos(&_vec3(64.f, 0.f, 72.f));
	//m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	Engine::CGameObject::Update_Object(0.f);
	
	//m_pMeshCom->Play_Animation(1.f);

	Load_ColliderFile(L"../Bin/Resource/Mesh/DynamicMesh/Save/Player.dat");
	for (auto& sphere : m_VecSphereCollider)
	{
		sphere->m_pDynamicThis = this;
		if (!sphere->m_FrameName.compare("Core")) {
			sphere->m_BonePart = CSphereCollider::BonePart_CollBody;
		}
		else if (!sphere->m_FrameName.compare("Weapon_Hand_R")) {
			sphere->m_BonePart = CSphereCollider::BonePart_NpcTalk;
		}
		sphere->m_BoneTeam = CSphereCollider::BoneTeam_Npc;
	}
	//m_pTransformCom->Set_Pos(&_vec3(52.f, 0.f, 52.f));
	return S_OK;
}

HRESULT CNpc_01::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_sComponent.m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	return S_OK;
}


CNpc_01* CNpc_01::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CNpc_01*	pInstance = new CNpc_01(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);
	return pInstance;
}

void CNpc_01::Free(void)
{
	for (auto& hpdec : m_VecDelayHpDec) 
		Engine::Safe_Delete(hpdec);
	m_VecDelayHpDec.clear();

	CUnit_D::Free();
}


HRESULT Client::CNpc_01::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_sComponent.m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);
	m_sComponent.m_pMeshCom->Set_AnimationSet(243);
	m_AniSpeed = 1.f;
	m_UnitName = UnitName::Npc;

	return S_OK;
}

Client::_int Client::CNpc_01::Update_Object(const _float& fTimeDelta)
{
	if (m_sStat.m_IsDead)
		return 1;

	Calc_Time(fTimeDelta);

	SetUp_OnTerrain();

	CUnit_D::Update_Object(fTimeDelta);
	m_sComponent.m_pMeshCom->Play_Animation(fTimeDelta * m_AniSpeed);

	m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}

Client::_int Client::CNpc_01::LateUpdate_Object(const _float& fTimeDelta)
{

	

	return 0;
}

void Client::CNpc_01::Render_Object(void)
{
	LPD3DXEFFECT	 pEffect = m_sComponent.m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
	pEffect->BeginPass(0);

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	m_sComponent.m_pMeshCom->Render_Meshes(pEffect);

	pEffect->EndPass();
	pEffect->End();

	//m_pNaviMeshCom->Render_NaviMeshes();


	Engine::Safe_Release(pEffect);
	/*_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pColliderCom->Render_Collider(Engine::COL_TRUE, &matWorld);*/
}

void Client::CNpc_01::SetUp_OnTerrain(void)
{
	_vec3	vPosition;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_Terrain->Get_sComponent_Terrain()->m_pBufferCom;
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_sComponent.m_pCalculatorCom->Compute_HeightOnTerrain(&vPosition, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_sComponent.m_pTransformCom->Move_Pos(vPosition.x, fHeight, vPosition.z);
}
 

void Client::CNpc_01::Calc_Time(_float fTimeDelta)
{

}



void CNpc_01::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{
	if (CSphereCollider::BoneTeam_Player != _colSphere->m_BoneTeam)
		return;

	if (_mySphere->m_BonePart == CSphereCollider::BonePart_CollBody) {

	}

	if (_mySphere->m_BonePart == CSphereCollider::BonePart_NpcTalk) {
		if (_colSphere->m_BonePart == CSphereCollider::BonePart_CollBody) {
			CNpcQuest_Manager::Get_Instance()->Get_NpcQuestInfo()->m_PlayerColl = true;
		}
	}
	
}
