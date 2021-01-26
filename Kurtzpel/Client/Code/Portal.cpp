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

	CUnit::Add_Component();
	// Mesh
	//pComponent = m_pStaticMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Arrow"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	//m_pTransformCom->m_vScale = { 0.01f, 0.01f, 0.01f };

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
	return S_OK;
}
Client::_int Client::CPortal::Update_Object(const _float& fTimeDelta)
{
	if (m_sStat.m_IsDead) {
		return 1;
	}



	CUnit::Update_Object(fTimeDelta);
	//m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);

	return 0;
}
void Client::CPortal::Render_Object(void)
{
	//LPD3DXEFFECT	 pEffect = m_sComponent.m_pShaderCom->Get_EffectHandle();
	//NULL_CHECK(pEffect);
	//Engine::Safe_AddRef(pEffect);

	//_uint	iMaxPass = 0;

	//pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
	//pEffect->BeginPass(0);

	//FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	////m_pStaticMeshCom->Render_Meshes(pEffect);

	//pEffect->EndPass();
	//pEffect->End();


	//Engine::Safe_Release(pEffect);
	///*_matrix matWorld;
	//m_pTransformCom->Get_WorldMatrix(&matWorld);

	//m_pColliderCom->Render_Collider(Engine::COL_TRUE, &matWorld);*/
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
