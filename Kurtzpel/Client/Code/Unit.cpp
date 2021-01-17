#include "stdafx.h"
#include "Unit.h"
#include "Export_Function.h"
#include "SphereCollider.h"
#include "Player.h"

CUnit::CUnit(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CUnit::~CUnit(void)
{
	for (auto& sphere : m_VecSphereCollider)
	{
		Engine::Safe_Release(sphere);
	}
	m_VecSphereCollider.clear();
}

HRESULT Client::CUnit::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

	// Transform
	pComponent = m_sComponent.m_pTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Clone(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_sComponent.m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Safe_AddRef(pComponent);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Calculator
	pComponent = m_sComponent.m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>(Engine::Clone(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Calculator", pComponent);

	// Collider 
	//pComponent = m_pColliderCom = Engine::CCollider::Create(m_pGraphicDev, m_pMeshCom->Get_VtxPos(), m_pMeshCom->Get_NumVtx(), m_pMeshCom->Get_Stride());
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);

	// Optimization
	//pComponent = m_pOptimizationCom = dynamic_cast<Engine::COptimization*>(Engine::Clone(L"Proto_Optimization"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Optimization", pComponent);

	// Shader
	pComponent = m_sComponent.m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Proto_Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);


	return S_OK;
}


CSphereCollider* CUnit::Get_BonePartCollider(CSphereCollider::BonePart _bonePart)
{
	for (auto& col : m_VecSphereCollider)
	{
		if (col->m_BonePart == _bonePart)
			return col;
	}
	return nullptr;
}

void CUnit::Set_BonePartColliderAttack(CSphereCollider::BonePart _bonePart, float _fattack, bool _battack, float _power)
{
	for (auto& col : m_VecSphereCollider)
	{
		if (col->m_BonePart == _bonePart) {
			col->m_WeaponAttack = _battack;
			col->m_WeaponPower = _power;
			col->Get_sStat()->m_fAttack = _fattack;
			return;
		}
	}
	return;
}

void CUnit::Set_StaticSphere()
{
	if (m_VecSphereCollider.size() == 0)
		return;
	_vec3	vPos;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	(*m_VecSphereCollider.begin())->m_pTransformCom->m_vInfo[Engine::INFO_POS] = vPos;
}

void CUnit::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CUnit::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_sComponent.m_pTransformCom->Set_Pos(&_vec3(5.f, 0.f, 5.f));
	m_sComponent.m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(45.f));

	return S_OK;
}
Client::_int Client::CUnit::Update_Object(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);
	if (m_sComponent.m_pStaticMeshCom != nullptr)
		Set_StaticSphere();

	for (auto& sphere : m_VecSphereCollider)
	{
		//sphere->m_pDynamicMesh = this;
		sphere->Update_Object(fTimeDelta);
	}
	return 0;
}
void Client::CUnit::Render_Object(void)
{
	LPD3DXEFFECT	 pEffect = m_sComponent.m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	m_sComponent.m_pStaticMeshCom->Render_Meshes(pEffect);

	pEffect->End();

	Engine::Safe_Release(pEffect);
}

void Client::CUnit::SetUp_OnTerrain(void)
{
	_vec3	vPosition;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_sComponent.m_pCalculatorCom->Compute_HeightOnTerrain(&vPosition, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_sComponent.m_pTransformCom->Move_Pos(vPosition.x, fHeight, vPosition.z);
}

HRESULT Client::CUnit::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_sComponent.m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	//////////////////////////////////////
	//const D3DLIGHT9* pLightInfo = Engine::Get_Light(0);

	//pEffect->SetVector("g_vLightDir", &_vec4(pLightInfo->Direction, 0.f));
	//D3DMATERIAL9			tMtrlInfo;
	//ZeroMemory(&tMtrlInfo, sizeof(D3DMATERIAL9));

	//tMtrlInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Power = 20.f;

	//pEffect->SetVector("Color", (_vec4*)&tMtrlInfo.Diffuse);

	return S_OK;
}

_bool CUnit::Collision_ToObject(const _tchar* pLayerTag, const _tchar* pObjTag)
{
	Engine::CCollider* pPlayerColliderCom = dynamic_cast<Engine::CCollider*>(Engine::Get_Component(pLayerTag, pObjTag, L"Com_Collider", Engine::ID_STATIC));
	NULL_CHECK_RETURN(pPlayerColliderCom, false);

	/*return m_pCalculatorCom->Collision_AABB(pPlayerColliderCom->Get_Min(),
											pPlayerColliderCom->Get_Max(),
											pPlayerColliderCom->Get_CollMatrix(),
											m_pColliderCom->Get_Min(),
											m_pColliderCom->Get_Max(),
											m_pColliderCom->Get_CollMatrix());*/

	return m_sComponent.m_pCalculatorCom->Collision_OBB(pPlayerColliderCom->Get_Min(),
		pPlayerColliderCom->Get_Max(),
		pPlayerColliderCom->Get_CollMatrix(),
		m_sComponent.m_pColliderCom->Get_Min(),
		m_sComponent.m_pColliderCom->Get_Max(),
		m_sComponent.m_pColliderCom->Get_CollMatrix());
}

void CUnit::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{

}