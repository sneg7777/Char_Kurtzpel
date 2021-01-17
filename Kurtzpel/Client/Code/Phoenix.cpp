#include "stdafx.h"
#include "Phoenix.h"
#include "Export_Function.h"
#include "DynamicCamera.h"
#include "SphereCollider.h"
#include "Player.h"
#include "Stage.h"
#include "Random_Manager.h"

CPhoenix::CPhoenix(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit_D(pGraphicDev)
{
	m_LifeTime = 10.f;
	m_sStat.m_fInitSpeed = 25.f;
	m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
}

CPhoenix::~CPhoenix(void)
{

}

HRESULT Client::CPhoenix::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	CUnit_D::Add_Component();
	// Mesh
	pComponent = m_sComponent.m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Phoenix"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);
	m_sComponent.m_pMeshCom->Set_AniAngle(95.f);
	//m_pMeshCom->Set_AniAngle(275.f);
	//
	//m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	Engine::CGameObject::Update_Object(1.f);
	
	//m_pMeshCom->Play_Animation(1.f);

	Load_ColliderFile(L"../Bin/Resource/Mesh/DynamicMesh/Save/Phoenix.dat");
	for (auto& sphere : m_VecSphereCollider)
	{
		sphere->m_pDynamicThis = this;
		if (!sphere->m_FrameName.compare("Bone001")) {
			sphere->m_BonePart = CSphereCollider::BonePart_Weapon;
			//sphere->m_WeaponAttack = true;
		}
		sphere->m_BoneTeam = CSphereCollider::BoneTeam_Player;
	}
	//m_pTransformCom->Set_Pos(&_vec3(52.f, 0.f, 52.f));
	return S_OK;
}

HRESULT CPhoenix::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
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


CPhoenix* CPhoenix::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPhoenix*	pInstance = new CPhoenix(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);
	return pInstance;
}

void CPhoenix::Free(void)
{
	CUnit_D::Free();
}


HRESULT Client::CPhoenix::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_sComponent.m_pTransformCom->Set_Scale(0.018f, 0.018f, 0.018f);
	m_sComponent.m_pMeshCom->Set_AnimationSet(0);

	return S_OK;
}

Client::_int Client::CPhoenix::Update_Object(const _float& fTimeDelta)
{
	if (m_sStat.m_IsDead || m_LifeTime < 0.f) {
		return 1;
	}
	Calc_Time(fTimeDelta);
	m_LifeTime -= fTimeDelta;


	_vec3	vPos, vPosAfter;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	//SetUp_OnTerrain();
	Pattern(fTimeDelta, vPos);

	CUnit::Update_Object(fTimeDelta);
	m_sComponent.m_pMeshCom->Play_Animation(fTimeDelta * m_AniSpeed);

	vPosAfter = vPos + m_sStat.m_vDir * m_sStat.m_fSpeed * 5.f;// * fTimeDelta;
	//m_pTransformCom->Set_Pos(&vPosAfter, true);
	if(!m_AniClip)
		m_sComponent.m_pTransformCom->Chase_Target(&vPosAfter, 0.f, fTimeDelta);
	else
		m_sComponent.m_pTransformCom->Chase_Target(&vPosAfter, m_sStat.m_fSpeed, fTimeDelta);

	//m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(-30.f), true);
	m_sComponent.m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(265.f), true);

	m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);

	return 0;
}

Client::_int Client::CPhoenix::LateUpdate_Object(const _float& fTimeDelta)
{

	

	return 0;
}

void Client::CPhoenix::Render_Object(void)
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

void Client::CPhoenix::SetUp_OnTerrain(void)
{
	_vec3	vPosition;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	Engine::CTerrainTex*		pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_sComponent.m_pCalculatorCom->Compute_HeightOnTerrain(&vPosition, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_sComponent.m_pTransformCom->Move_Pos(vPosition.x, fHeight, vPosition.z);
}


void Client::CPhoenix::Calc_Time(_float fTimeDelta)
{
	m_LifeTime -= fTimeDelta;
	m_IntervalTime -= fTimeDelta;
}


void Client::CPhoenix::Pattern(_float fTimeDelta, _vec3 vPos)
{
	if (!m_AniClip && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.3f)) {
		m_AniClip = true;
		m_sComponent.m_pMeshCom->Set_AnimationSet(1);
		(*m_VecSphereCollider.begin())->m_WeaponAttack = true;
	}
	else if (!m_AniClip) {
		if (m_sComponent.m_pMeshCom->Get_AnimationTrackPos() > 1.f) {
			_vec3 vAfterPos = vPos + _vec3{ 0.f, -10.f, 0.f } *fTimeDelta;
			m_sComponent.m_pTransformCom->Set_Pos(&vAfterPos, true);
		}
	}
	if (m_IntervalTime < 0.f) {
		m_IntervalTime = 0.1f;
		(*m_VecSphereCollider.begin())->m_VecDamagedObject.clear();
	}
}

void CPhoenix::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{
	if (_mySphere->m_BoneTeam == _colSphere->m_BoneTeam)
		return;

	
}