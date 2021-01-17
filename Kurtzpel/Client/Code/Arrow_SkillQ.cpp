#include "stdafx.h"
#include "Arrow_SkillQ.h"
#include "Export_Function.h"
#include "Player.h"
#include "Stage.h"
#include "ArrowNaviTerrain.h"


CArrow_SkillQ::CArrow_SkillQ(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit(pGraphicDev)
{
	m_sStat.m_fSpeed = 30.f;
	m_LifeTime = 7.5f;
}

CArrow_SkillQ::CArrow_SkillQ(CArrow_SkillQ* _copy)
	: CUnit(_copy->m_pGraphicDev)
{
	m_sStat.m_fSpeed = _copy->m_sStat.m_fSpeed;
	m_sStat.m_fAttack = _copy->m_sStat.m_fAttack;
	m_LifeTime = 7.5f;
	m_SplitCount = _copy->m_SplitCount - 1;
	Create_Coll();
	Engine::CLayer* pLayer = Engine::CManagement::GetInstance()->m_pScene->Get_Layer(Engine::CLayer::LayerName::Layer_Static);
	pLayer->Add_GameObject(L"Arrow_SkillQ", this);
}

CArrow_SkillQ::~CArrow_SkillQ(void)
{

}

HRESULT Client::CArrow_SkillQ::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	
	CUnit::Add_Component();
	// Mesh
	pComponent = m_sComponent.m_pStaticMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Arrow"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);


	//m_pTransformCom->m_vInfo[Engine::INFO_POS] = { 5.f, -15.f, 10.f };
	m_sComponent.m_pTransformCom->m_vScale = { 0.015f, 0.015f, 0.015f };

	return S_OK;
}


CArrow_SkillQ* CArrow_SkillQ::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CArrow_SkillQ* pInstance = new CArrow_SkillQ(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

CArrow_SkillQ* CArrow_SkillQ::Create(CArrow_SkillQ* _copy)
{
	CArrow_SkillQ* pInstance = new CArrow_SkillQ(_copy);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CArrow_SkillQ::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CArrow_SkillQ::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}
Client::_int Client::CArrow_SkillQ::Update_Object(const _float& fTimeDelta)
{
	if (m_sStat.m_IsDead || m_LifeTime < 0.f) {
		return 1;
	}
	m_LifeTime -= fTimeDelta;
	Engine::CArrowNaviMesh* pNaviMeshCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_ArrowNaviTerrain->m_pArrowNaviMeshCom;

	_vec3	vPos, vPosAfter, vPosMove;
	bool	NaviCheck = false;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	vPosAfter = vPos + m_sStat.m_vDir * m_sStat.m_fSpeed * 5.f;// * fTimeDelta;
	
	vPosMove = (pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_sStat.m_vDir * m_sStat.m_fSpeed * fTimeDelta), &m_sStat.m_dwNaviIndex, &NaviCheck));
	
	if (!NaviCheck) {
		m_sComponent.m_pTransformCom->Set_Pos(&vPosMove, true);
		CUnit::Update_Object(fTimeDelta);

		//m_pTransformCom->Set_Pos(&vPosAfter, true);
		m_sComponent.m_pTransformCom->Chase_Target(&vPosAfter, m_sStat.m_fSpeed, fTimeDelta);

		m_sComponent.m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(90.f), true);
		m_sComponent.m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(180.f), true);

		m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);
	}
	else {
		m_sStat.m_IsDead = true;
		if (m_SplitCount == 0)
			return 0;
		_vec3 vReverseDir = { m_sStat.m_vDir.x, m_sStat.m_vDir.y, -m_sStat.m_vDir.z };
		pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vReverseDir * m_sStat.m_fSpeed * fTimeDelta), &m_sStat.m_dwNaviIndex, &NaviCheck);
		for (int i = 0; i < 3; i++)
		{
			CArrow_SkillQ* copyArrow = CArrow_SkillQ::Create(this);
			if (NaviCheck) {
				vReverseDir.x *= -1.f;
				vReverseDir.z *= -1.f;
			}
			vReverseDir.x += vReverseDir.x * 0.2f * (float(CRandom_Manager::Random() % 8) - 3.5f);
			vReverseDir.z += vReverseDir.z * 0.2f * (float(CRandom_Manager::Random() % 8) - 3.5f);
			copyArrow->m_sStat.m_vDir = vReverseDir;
			if (vPos.y < 0.f)
				copyArrow->m_sStat.m_vDir.y *= -1.f;
			else if (vPos.y > 10.f)
				copyArrow->m_sStat.m_vDir.y *= -1.f;
			copyArrow->m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS] = vPos + vReverseDir * 2.f;
			copyArrow->Engine::CGameObject::Update_Object(0.f);
		}
	}

	return 0;
}
void Client::CArrow_SkillQ::Render_Object(void)
{
	
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	LPD3DXEFFECT	 pEffect = m_sComponent.m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);
	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
	pEffect->BeginPass(0);

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );
	
	m_sComponent.m_pStaticMeshCom->Render_Meshes(pEffect);

	pEffect->EndPass();
	pEffect->End();


	Engine::Safe_Release(pEffect);
	/*_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pColliderCom->Render_Collider(Engine::COL_TRUE, &matWorld);*/
}

HRESULT Client::CArrow_SkillQ::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
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

void Client::CArrow_SkillQ::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{
	if (_mySphere->m_BoneTeam == _colSphere->m_BoneTeam)
		return;
	if(_colSphere->m_BonePart == CSphereCollider::BonePart::BonePart_CollBody)
		m_sStat.m_IsDead = true;
	
}

void Client::CArrow_SkillQ::Create_Coll()
{
	CSphereCollider* sphereCol = CSphereCollider::Create(m_pGraphicDev);
	sphereCol->Set_Transform(1.f);
	sphereCol->m_pStaticThis = this;
	sphereCol->m_WeaponAttack = true;
	sphereCol->m_BoneTeam = CSphereCollider::BoneTeam::BoneTeam_Player;
	sphereCol->m_BonePart = CSphereCollider::BonePart::BonePart_Static;
	sphereCol->Engine::CGameObject::Update_Object(0.f);
	m_VecSphereCollider.emplace_back(sphereCol);
}

void Client::CArrow_SkillQ::Set_SpeedToLife(float _speed, float _life)
{
	m_sStat.m_fSpeed = _speed;
	if (_life > 0)
		m_LifeTime = _life;
}