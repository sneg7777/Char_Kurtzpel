#include "stdafx.h"
#include "Arrow.h"
#include "Export_Function.h"
#include "Player.h"


CArrow::CArrow(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit(pGraphicDev)
{
	m_fSpeed = 20.f;
	m_LifeTime = 4.5f;
}

CArrow::~CArrow(void)
{

}

HRESULT Client::CArrow::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	
	CUnit::Add_Component();
	// Mesh
	pComponent = m_pStaticMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Arrow"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);


	//m_pTransformCom->m_vInfo[Engine::INFO_POS] = { 5.f, -15.f, 10.f };
	m_pTransformCom->m_vScale = { 0.01f, 0.01f, 0.01f };

	return S_OK;
}


CArrow* CArrow::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CArrow*	pInstance = new CArrow(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CArrow::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CArrow::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	CSphereCollider* sphereCol = CSphereCollider::Create(m_pGraphicDev);
	sphereCol->Set_Transform(1.f);
	sphereCol->m_pStaticThis = this;
	sphereCol->m_WeaponAttack = true;
	sphereCol->m_BoneTeam = CSphereCollider::BoneTeam::BoneTeam_Player;
	sphereCol->m_BonePart = CSphereCollider::BonePart::BonePart_Static;
	sphereCol->Engine::CGameObject::Update_Object(0.f);
	m_VecSphereCollider.emplace_back(sphereCol);

	return S_OK;
}
Client::_int Client::CArrow::Update_Object(const _float& fTimeDelta)
{
	if (m_IsDead || m_LifeTime < 0.f) {
		return 1;
	}
	m_LifeTime -= fTimeDelta;
	//
	if (Engine::Get_DIKeyState(DIK_X) & 0x80) {
		m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(-m_RocationX));
		m_RocationX++;
		m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(m_RocationX));
		if (m_RocationX > 360.f)
			m_RocationX -= 360.f;
	}
	if (Engine::Get_DIKeyState(DIK_C) & 0x80) {
		m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(-m_RocationY));
		m_RocationY++;
		m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(m_RocationY));
		if (m_RocationY > 360.f)
			m_RocationY -= 360.f;
	}
	if (Engine::Get_DIKeyState(DIK_V) & 0x80) {
		m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(-m_RocationZ));
		m_RocationZ++;
		m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(m_RocationZ));
		if (m_RocationZ > 360.f)
			m_RocationZ -= 360.f;
	}
	//
	_vec3	vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	vPos += m_vDir * m_fSpeed * fTimeDelta;
	m_pTransformCom->Set_Pos(&vPos, true);
	
	CUnit::Update_Object(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);

	return 0;
}
void Client::CArrow::Render_Object(void)
{
	LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
	pEffect->BeginPass(0);

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	m_pStaticMeshCom->Render_Meshes(pEffect);

	pEffect->EndPass();
	pEffect->End();


	Engine::Safe_Release(pEffect);
	/*_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pColliderCom->Render_Collider(Engine::COL_TRUE, &matWorld);*/
}

HRESULT Client::CArrow::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
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

void Client::CArrow::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{
	if (_mySphere->m_BoneTeam == _colSphere->m_BoneTeam)
		return;
	if(_colSphere->m_BonePart == CSphereCollider::BonePart::BonePart_CollBody)
		m_IsDead = true;
}