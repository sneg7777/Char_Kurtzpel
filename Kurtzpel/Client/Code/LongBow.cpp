#include "stdafx.h"
#include "LongBow.h"
#include "Export_Function.h"
#include "Player.h"

CLongBow* CLongBow::m_pInstance = nullptr;

CLongBow::CLongBow(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit(pGraphicDev)
{
	
}

CLongBow::~CLongBow(void)
{

}

HRESULT Client::CLongBow::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	
	CUnit::Add_Component();

	// Mesh
	pComponent = m_pStaticMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_LongBow"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	m_pTransformCom->m_vInfo[Engine::INFO_POS] = { 5.f, -15.f, 10.f };
	m_pTransformCom->m_vScale = { 1.f, 1.f, 1.f };

	CPlayer::GetInstance()->m_LongBow = this;
	CPlayer::GetInstance()->m_WeaponEquip = CPlayer::Weapon_LongBow;
	return S_OK;
}


CLongBow* CLongBow::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLongBow*	pInstance = new CLongBow(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CLongBow::Free(void)
{
	CUnit::Free();
}


HRESULT Client::CLongBow::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	//m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(82.f));
	m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(204.f));
	m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(211.f));
	m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(128.f));
	m_RocationX = 204.f;
	m_RocationY = 211.f;
	m_RocationZ = 128.f;
	return S_OK;
}
Client::_int Client::CLongBow::Update_Object(const _float& fTimeDelta)
{
	if (m_IsDead) {
		return 1;
	}

	if (nullptr == m_pParentBoneMatrix)
	{
		BoneAttach("Weapon_R");
	}

	CUnit::Update_Object(fTimeDelta);
		
	m_pTransformCom->Set_ParentMatrix(&(*m_pParentBoneMatrix * *m_pParentWorldMatrix));
	

	//m_bColl = Collision_ToObject(L"GameLogic", L"Player");

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	
	//if (Engine::Get_DIKeyState(DIK_X) & 0x80) {
	//	/*m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(-m_RocationX));
	//	m_RocationX++;
	//	m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(m_RocationX));
	//	if (m_RocationX > 360.f)
	//		m_RocationX -= 360.f;*/
	//	m_pTransformCom->m_vInfo[Engine::INFO_POS].x++;
	//}
	//if (Engine::Get_DIKeyState(DIK_C) & 0x80) {
	//	/*m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(-m_RocationY));
	//	m_RocationY++;
	//	m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(m_RocationY));
	//	if (m_RocationY > 360.f)
	//		m_RocationY -= 360.f;*/
	//	m_pTransformCom->m_vInfo[Engine::INFO_POS].y++;
	//}
	//if (Engine::Get_DIKeyState(DIK_V) & 0x80) {
	//	/*m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(-m_RocationZ));
	//	m_RocationZ++;
	//	m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(m_RocationZ));
	//	if (m_RocationZ > 360.f)
	//		m_RocationZ -= 360.f;*/
	//	m_pTransformCom->m_vInfo[Engine::INFO_POS].z++;
	//}
	return 0;
}
void Client::CLongBow::Render_Object(void)
{
	LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// ���� ���̴� ������ ���� �ִ� �ִ� �н��� ������ ����, ����ϴ� ���
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

void CLongBow::Set_Pos() {
	CPlayer* pPlayer = CPlayer::GetInstance();
	if (pPlayer->m_State == CPlayer::State::State_Attack || pPlayer->m_State == CPlayer::State::State_Skill) {
		m_pTransformCom->m_vInfo[Engine::INFO_POS] = { 0.f, 0.f, 0.f };
		BoneAttach("Weapon_Hand_R");
		if (m_RocationX != 0.f) {
			m_pTransformCom->m_vInfo[Engine::INFO_POS] = { 0.f, 0.f, 0.f };
			m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(-m_RocationX + 0.f));
			m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(-m_RocationY + 0.f));
			m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(-m_RocationZ + 0.f));
			m_RocationX = 0.f;
			m_RocationY = 0.f;
			m_RocationZ = 0.f;
		}
	}
	else {
		BoneAttach("Weapon_R");
		if (m_RocationX != 204.f) {
			m_pTransformCom->m_vInfo[Engine::INFO_POS] = { 5.f, -15.f, 10.f };
			m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(-m_RocationX + 204.f));
			m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(-m_RocationY + 211.f));
			m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(-m_RocationZ + 128.f));
			m_RocationX = 204.f;
			m_RocationY = 211.f;
			m_RocationZ = 128.f;
		}
	}
}

void CLongBow::BoneAttach(string _frame)
{
	Engine::CDynamicMesh* pPlayerMeshCom = CPlayer::GetInstance()->m_pMeshCom;
	//dynamic_cast<Engine::CDynamicMesh*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Mesh", Engine::ID_STATIC));
	NULL_CHECK_RETURN(pPlayerMeshCom, );

	//const Engine::D3DXFRAME_DERIVED* pFrame = pPlayerMeshCom->Get_FrameByName("Weapon_Spine_R");
	const Engine::D3DXFRAME_DERIVED* pFrame = pPlayerMeshCom->Get_FrameByName(_frame.c_str());

	m_pParentBoneMatrix = &pFrame->CombinedTransformationMatrix;

	Engine::CTransform* pPlayerTransCom = CPlayer::GetInstance()->m_pTransformCom;
	//dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"GameLogic", L"Player", L"Com_Transform", Engine::ID_DYNAMIC));
	NULL_CHECK_RETURN(pPlayerTransCom, );
	m_pParentWorldMatrix = pPlayerTransCom->Get_WorldMatrix();
}

_bool CLongBow::Collision_ToObject(const _tchar * pLayerTag, const _tchar * pObjTag)
{
	Engine::CCollider*	pPlayerColliderCom = dynamic_cast<Engine::CCollider*>(Engine::Get_Component(pLayerTag, pObjTag, L"Com_Collider", Engine::ID_STATIC));
	NULL_CHECK_RETURN(pPlayerColliderCom, false);
	
	/*return m_pCalculatorCom->Collision_AABB(pPlayerColliderCom->Get_Min(),
											pPlayerColliderCom->Get_Max(),
											pPlayerColliderCom->Get_CollMatrix(),
											m_pColliderCom->Get_Min(), 
											m_pColliderCom->Get_Max(),
											m_pColliderCom->Get_CollMatrix());*/

	return m_pCalculatorCom->Collision_OBB(pPlayerColliderCom->Get_Min(),
		pPlayerColliderCom->Get_Max(),
		pPlayerColliderCom->Get_CollMatrix(),
		m_pColliderCom->Get_Min(),
		m_pColliderCom->Get_Max(),
		m_pColliderCom->Get_CollMatrix());
}

HRESULT Client::CLongBow::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
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