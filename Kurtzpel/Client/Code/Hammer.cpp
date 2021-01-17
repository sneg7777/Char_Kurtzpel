#include "stdafx.h"
#include "Hammer.h"
#include "Export_Function.h"
#include "Player.h"

CHammer* CHammer::m_pInstance = nullptr;

CHammer::CHammer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit(pGraphicDev)
{
	
}

CHammer::~CHammer(void)
{

}

HRESULT Client::CHammer::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	
	CUnit::Add_Component();

	// Mesh
	pComponent = m_sComponent.m_pStaticMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Hammer"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS] = { -25.f, 50.f, 0.f };
	m_sComponent.m_pTransformCom->m_vScale = { 0.7f, 0.7f, 0.7f };

	CPlayer::GetInstance()->m_Hammer = this;
	CPlayer::GetInstance()->m_WeaponEquip = CPlayer::Weapon_Hammer;
	return S_OK;
}


CHammer* CHammer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CHammer*	pInstance = new CHammer(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CHammer::Free(void)
{
	CUnit::Free();
}


HRESULT Client::CHammer::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_sComponent.m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(172.5f));
	m_sComponent.m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(22.5f));
	m_RocationX = 172.5f;
	m_RocationZ = 22.5f;
	return S_OK;
}
Client::_int Client::CHammer::Update_Object(const _float& fTimeDelta)
{
	if (m_sStat.m_IsDead) {
		return 1;
	}

	if (nullptr == m_pParentBoneMatrix)
	{
		BoneAttach("Weapon_R");
	}

	CUnit::Update_Object(fTimeDelta);
		
	m_sComponent.m_pTransformCom->Set_ParentMatrix(&(*m_pParentBoneMatrix * *m_pParentWorldMatrix));
	

	//m_bColl = Collision_ToObject(L"GameLogic", L"Player");

	m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}
void Client::CHammer::Render_Object(void)
{
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

void CHammer::Set_Pos() {
	CPlayer* pPlayer = CPlayer::GetInstance();
	if (pPlayer->m_State == CPlayer::State::State_Attack || pPlayer->m_State == CPlayer::State::State_Skill) {
		m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS] = { 0.f, 0.f, 0.f };
		BoneAttach("Weapon_Hand_R");
		if (m_RocationX != 0.f) {
			m_sComponent.m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(-m_RocationX + 0.f));
			m_sComponent.m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(-m_RocationZ));
			m_RocationX = 0.f;
			m_RocationZ = 0.f;
		}
	}
	else {
		m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS] = { -25.f, 50.f, 0.f };
		BoneAttach("Weapon_R");
		if (m_RocationX != 172.5f) {
			m_sComponent.m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(-m_RocationX + 172.5f));
			m_sComponent.m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(-m_RocationZ + 22.5f));
			m_RocationX = 172.5f;
			m_RocationZ = 22.5f;
		}
	}
}

void CHammer::BoneAttach(string _frame)
{
	Engine::CDynamicMesh* pPlayerMeshCom = CPlayer::GetInstance()->Get_sComponent()->m_pMeshCom;
	//dynamic_cast<Engine::CDynamicMesh*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Mesh", Engine::ID_STATIC));
	NULL_CHECK_RETURN(pPlayerMeshCom, );

	//const Engine::D3DXFRAME_DERIVED* pFrame = pPlayerMeshCom->Get_FrameByName("Weapon_Spine_R");
	const Engine::D3DXFRAME_DERIVED* pFrame = pPlayerMeshCom->Get_FrameByName(_frame.c_str());

	m_pParentBoneMatrix = &pFrame->CombinedTransformationMatrix;

	Engine::CTransform* pPlayerTransCom = CPlayer::GetInstance()->Get_sComponent()->m_pTransformCom;
	//dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", Engine::ID_DYNAMIC));
	NULL_CHECK_RETURN(pPlayerTransCom, );
	m_pParentWorldMatrix = pPlayerTransCom->Get_WorldMatrix();
}

_bool CHammer::Collision_ToObject(const _tchar * pLayerTag, const _tchar * pObjTag)
{
	Engine::CCollider*	pPlayerColliderCom = dynamic_cast<Engine::CCollider*>(Engine::Get_Component(pLayerTag, pObjTag, L"Com_Collider", Engine::ID_STATIC));
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

HRESULT Client::CHammer::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
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