#include "stdafx.h"
#include "SkillCollider.h"
#include "Export_Function.h"
#include "Player.h"


CSkillCollider::CSkillCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit(pGraphicDev)
{

}

CSkillCollider::~CSkillCollider(void)
{

}

HRESULT Client::CSkillCollider::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	
	CUnit::Add_Component();
	// Mesh
	//pComponent = m_pStaticMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Arrow"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	//m_pTransformCom->m_vScale = { 0.01f, 0.01f, 0.01f };

	return S_OK;
}


CSkillCollider* CSkillCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSkillCollider*	pInstance = new CSkillCollider(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CSkillCollider::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CSkillCollider::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}
Client::_int Client::CSkillCollider::Update_Object(const _float& fTimeDelta)
{
	if (m_IsDead || m_LifeTime < 0.f) {
		return 1;
	}
	m_LifeTime -= fTimeDelta;

	if (m_StartTime <= 0.f) {
		CSphereCollider* sphereCol = (*m_VecSphereCollider.begin());
		if (m_Interval != 0.f) {
			m_IntervalTime -= fTimeDelta;
			if (m_IntervalTime <= 0.f) {
				m_IntervalTime = m_Interval;
				m_Interval_AttackTime = m_Interval_Attack;
				sphereCol->m_WeaponAttack = true;
				sphereCol->m_VecDamagedObject.clear();
			}
		}

		if (m_Interval_Attack != -1.f && sphereCol->m_WeaponAttack) {
			m_Interval_AttackTime -= fTimeDelta;
			if (m_Interval_AttackTime <= 0.f) {
				m_Interval_AttackTime = m_Interval_Attack;
				sphereCol->m_WeaponAttack = false;
			}
		}
	}
	else {
		m_StartTime -= fTimeDelta;
	}

	CUnit::Update_Object(fTimeDelta);
	//m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);

	return 0;
}
void Client::CSkillCollider::Render_Object(void)
{
	LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
	pEffect->BeginPass(0);

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	//m_pStaticMeshCom->Render_Meshes(pEffect);

	pEffect->EndPass();
	pEffect->End();


	Engine::Safe_Release(pEffect);
	/*_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pColliderCom->Render_Collider(Engine::COL_TRUE, &matWorld);*/
}

HRESULT Client::CSkillCollider::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	return S_OK;
}

void Client::CSkillCollider::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{
	//if (_mySphere->m_BoneTeam == _colSphere->m_BoneTeam)
	//	return;

	//if(_colSphere->m_BonePart == CSphereCollider::BonePart::BonePart_CollBody)
	//	m_IsDead = true;
}

void Client::CSkillCollider::Set_Collider(_vec3 _pos, float _scale, CSphereCollider::BoneTeam _team, float _attack, float _lifeTime, float _interval, float _interval_AttackTime, float _startTime) {
	m_pTransformCom->m_vInfo[Engine::INFO_POS] = _pos;
	CUnit::Update_Object(0.f);
	CSphereCollider* sphereCol = CSphereCollider::Create(m_pGraphicDev);
	sphereCol->Set_Transform(_scale);
	sphereCol->m_pStaticThis = this;
	sphereCol->m_WeaponAttack = false;
	sphereCol->m_BoneTeam = _team;
	sphereCol->m_BonePart = CSphereCollider::BonePart::BonePart_Static;
	m_fAttack = _attack;
	m_Interval = _interval;
	m_LifeTime = _lifeTime;
	m_Interval_Attack = _interval_AttackTime;
	m_StartTime = _startTime;
	
	sphereCol->Engine::CGameObject::Update_Object(0.f);
	m_VecSphereCollider.emplace_back(sphereCol);

	Engine::CGameObject* pGameObject = this;
	Engine::CLayer* pLayer = Engine::CManagement::GetInstance()->m_pScene->Get_Layer(Engine::CLayer::LayerName::Layer_Static);
	pLayer->Add_GameObject(L"SkillCollider", pGameObject);
}