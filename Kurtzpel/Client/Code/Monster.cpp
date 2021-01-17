#include "stdafx.h"
#include "Monster.h"
#include "Export_Function.h"
#include "SphereCollider.h"
#include "Player.h"
#include "Unit_D.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit_D(pGraphicDev)
{
	m_UnitName = UnitName::Monster;
}

CMonster::~CMonster(void)
{

}

HRESULT Client::CMonster::Add_Component(void)
{
	CUnit_D::Add_Component();
	return S_OK;
}

void CMonster::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CMonster::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

Client::_int Client::CMonster::Update_Object(const _float& fTimeDelta)
{
	CUnit_D::Update_Object(fTimeDelta);

	return 0;
}

void Client::CMonster::Render_Object(void)
{
	LPD3DXEFFECT	 pEffect = m_sComponent.m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	m_sComponent.m_pMeshCom->Render_Meshes(pEffect);

	pEffect->End();

	Engine::Safe_Release(pEffect);
}

_bool CMonster::Collision_ToObject(const _tchar* pLayerTag, const _tchar* pObjTag)
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

float CMonster::PlayerSearchDistance()
{
	CPlayer* player = CPlayer::GetInstance();
	if (nullptr == player) return 0.f;

	Engine::CTransform* playerTrans = player->Get_sComponent()->m_pTransformCom;// dynamic_cast<Engine::CTransform*>(player->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC));

	if (m_pPlayerTrans != playerTrans)
		m_pPlayerTrans = playerTrans;

	float distX = m_pPlayerTrans->m_vInfo[Engine::INFO_POS].x - m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS].x;
	float distZ = m_pPlayerTrans->m_vInfo[Engine::INFO_POS].z - m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS].z;

	float disPlayer;
	disPlayer = sqrt(distX * distX + distZ * distZ);

	return disPlayer; 

}

void CMonster::Set_PlayerTowardAngle()
{
	_vec3 monsterPos = m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS];
	_vec3 playerPos = m_pPlayerTrans->m_vInfo[Engine::INFO_POS];
	_vec3 monsterDir = playerPos - monsterPos;
	D3DXVec3Normalize(&monsterDir, &monsterDir);

	_vec3 monsterLook = m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_LOOK];
	D3DXVec3Normalize(&monsterLook, &monsterLook);
	float radian = D3DXVec3Dot(&monsterDir, &monsterLook);
	m_TowardAngle1 = D3DXToDegree(radian);
	_vec3 monsterRight = m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_RIGHT];
	D3DXVec3Normalize(&monsterLook, &monsterRight);
	float radian2 = D3DXVec3Dot(&monsterDir, &monsterRight);
	m_TowardAngle2 = D3DXToDegree(radian2);
	// 뒤쪽
	if (m_TowardAngle1 < 0.f) {
		// 오른쪽
		if (m_TowardAngle2 >= 0.1f) {
			m_AngleOfSame = false;
		}
		// 왼쪽
		else if (m_TowardAngle2 < -0.1f) {
			m_AngleOfSame = false;
		}
		else {
			m_AngleOfSame = true;
		}
	}
	// 앞쪽
	else {
		// 오른쪽
		if (m_TowardAngle2 >= 0.1f) {
			m_AngleOfSame = false;
		}
		// 왼쪽
		else if (m_TowardAngle2 < -0.1f) {
			m_AngleOfSame = false;
		}
		else {
			m_AngleOfSame = true;
		}
	}
}

void CMonster::Rocate_PlayerToWardAngle(float fTimeDelta, float _speed)
{
	// 뒤쪽
	if (m_TowardAngle1 < 0.f) {
		// 오른쪽
		if (m_TowardAngle2 >= 0.1f) {
			m_sComponent.m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(-_speed) * fTimeDelta);
		}
		// 왼쪽
		else if (m_TowardAngle2 < -0.1f) {
			m_sComponent.m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(_speed) * fTimeDelta);
		}
	}
	// 앞쪽
	else {
		// 오른쪽
		if (m_TowardAngle2 >= 0.1f) {
			m_sComponent.m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(-_speed) * fTimeDelta);
		}
		// 왼쪽
		else if (m_TowardAngle2 < -0.1f) {
			m_sComponent.m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(_speed) * fTimeDelta);
		}
	}
}
