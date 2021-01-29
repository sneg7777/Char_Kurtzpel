#include "stdafx.h"
#include "GH_RockIn.h"
#include "Export_Function.h"
#include "DynamicCamera.h"
#include "SphereCollider.h"
#include "Player.h"
#include "Stage.h"
#include "Random_Manager.h"
#include "UI_Manager.h"

CGH_RockIn::CGH_RockIn(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit_D(pGraphicDev)
{

}

CGH_RockIn::~CGH_RockIn(void)
{

}

HRESULT Client::CGH_RockIn::Add_Component(bool _inCheck)
{
	Engine::CComponent*		pComponent = nullptr;

	CUnit_D::Add_Component();
	// Mesh
	if(_inCheck)
		pComponent = m_sComponent.m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_GH_SkillF_StoneIn"));
	else
		pComponent = m_sComponent.m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_GH_SkillF_StoneOut"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);
	_vec3 vPos;
	CPlayer::GetInstance()->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	m_sComponent.m_pTransformCom->Set_Pos(&vPos);
	Engine::CGameObject::Update_Object(0.f);
	

	//Load_ColliderFile(L"../Bin/Resource/Mesh/DynamicMesh/Save/Monster1_TwoHand.dat");
	//for (auto& sphere : m_VecSphereCollider)
	//{
	//	sphere->m_pDynamicThis = this;
	//	if (!sphere->m_FrameName.compare("Bip001")) {
	//		sphere->m_BonePart = CSphereCollider::BonePart_CollBody;
	//	}
	//	else if (!sphere->m_FrameName.compare("Point_Weapon_R_Hand")) {
	//		sphere->m_BonePart = CSphereCollider::BonePart_Weapon;
	//	}
	//	sphere->m_BoneTeam = CSphereCollider::BoneTeam_Enemy;
	//}
	return S_OK;
}

HRESULT CGH_RockIn::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	CUnit::SetUp_ConstantTable(pEffect);

	return S_OK;
}


CGH_RockIn* CGH_RockIn::Create(LPDIRECT3DDEVICE9 pGraphicDev, bool _inCheck)
{
	CGH_RockIn*	pInstance = new CGH_RockIn(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(_inCheck)))
		Client::Safe_Release(pInstance);
	return pInstance;
}

void CGH_RockIn::Free(void)
{
	CUnit_D::Free();
}

HRESULT Client::CGH_RockIn::Ready_Object()
{
	FAILED_CHECK_RETURN(Add_Component(false), E_FAIL);
	m_sComponent.m_pMeshCom->Set_AnimationSet(0);
	m_sComponent.m_pTransformCom->Set_Scale(0.035f, 0.035f, 0.035f);
	dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_Layer(Engine::CLayer::LayerName::Layer_DynamicNoColl)->Add_GameObject(L"GH_Rock", this);
	return S_OK;
}

HRESULT Client::CGH_RockIn::Ready_Object(bool _inCheck)
{
	FAILED_CHECK_RETURN(Add_Component(_inCheck), E_FAIL);
	m_sComponent.m_pMeshCom->Set_AnimationSet(0);
	m_sComponent.m_pTransformCom->Set_Scale(0.035f, 0.035f, 0.035f);
	dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_Layer(Engine::CLayer::LayerName::Layer_DynamicNoColl)->Add_GameObject(L"GH_Rock", this);
	return S_OK;
}

Client::_int Client::CGH_RockIn::Update_Object(const _float& fTimeDelta)
{
	if (m_sStat.m_IsDead || m_sStat.m_IsDeadTime > 1.5f) {
		return 1;
	}

	Calc_Time(fTimeDelta);

	SetUp_OnTerrain();
	//Pattern(fTimeDelta);

	if(m_sComponent.m_pMeshCom->Get_AnimationTrackPos() <= m_sComponent.m_pMeshCom->Get_Period())
		m_sComponent.m_pMeshCom->Play_Animation(fTimeDelta * m_AniSpeed);
	CUnit_D::Update_Object(fTimeDelta);

	m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}

Client::_int Client::CGH_RockIn::LateUpdate_Object(const _float& fTimeDelta)
{

	

	return 0;
}

void Client::CGH_RockIn::Render_Object(void)
{
	m_sComponent.m_pMeshCom->Play_Animation(0.f);
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

	Engine::Safe_Release(pEffect);
	
}

void Client::CGH_RockIn::SetUp_OnTerrain(void)
{
	_vec3	vPosition;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_Terrain->Get_sComponent_Terrain()->m_pBufferCom;
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_sComponent.m_pCalculatorCom->Compute_HeightOnTerrain(&vPosition, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_sComponent.m_pTransformCom->Move_Pos(vPosition.x, fHeight, vPosition.z);
}


void Client::CGH_RockIn::Calc_Time(_float fTimeDelta)
{
	m_sStat.m_IsDeadTime += fTimeDelta;
}

void Client::CGH_RockIn::Pattern(_float fTimeDelta)
{
	
}

void CGH_RockIn::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{
	
}
