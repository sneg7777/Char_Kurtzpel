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
	LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	m_pMeshCom->Render_Meshes(pEffect);

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

	return m_pCalculatorCom->Collision_OBB(pPlayerColliderCom->Get_Min(),
		pPlayerColliderCom->Get_Max(),
		pPlayerColliderCom->Get_CollMatrix(),
		m_pColliderCom->Get_Min(),
		m_pColliderCom->Get_Max(),
		m_pColliderCom->Get_CollMatrix());
}

float CMonster::PlayerSearchDistance()
{
	CPlayer* player = CPlayer::GetInstance();
	if (nullptr == player) return 0.f;

	Engine::CTransform* playerTrans = dynamic_cast<Engine::CTransform*>(player->Get_Component(L"Com_Transform", Engine::ID_DYNAMIC));

	if (m_pPlayerTrans != playerTrans)
		m_pPlayerTrans = playerTrans;

	float distX = m_pPlayerTrans->m_vInfo[Engine::INFO_POS].x - m_pTransformCom->m_vInfo[Engine::INFO_POS].x;
	float distZ = m_pPlayerTrans->m_vInfo[Engine::INFO_POS].z - m_pTransformCom->m_vInfo[Engine::INFO_POS].z;

	float disPlayer;
	disPlayer = sqrt(distX * distX + distZ * distZ);

	return disPlayer; 

}
