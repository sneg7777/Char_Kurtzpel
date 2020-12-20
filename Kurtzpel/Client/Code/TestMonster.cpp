#include "stdafx.h"
#include "TestMonster.h"
#include "Export_Function.h"

CTestMonster::CTestMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CTestMonster::~CTestMonster(void)
{

}

HRESULT Client::CTestMonster::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CTriCol*>(Engine::Clone(Engine::RESOURCE_STATIC, L"Buffer_TriCol"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	
	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Clone(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Transform", pComponent);
	return S_OK;
}


CTestMonster* CTestMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTestMonster*	pInstance = new CTestMonster(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CTestMonster::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CTestMonster::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	//m_pTransformCom->m_vScale = _vec3(4.f, 2.f, 1.f);

	return S_OK;
}
Client::_int Client::CTestMonster::Update_Object(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);

	Engine::CTransform*	pPlayerTransform = dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"Environment", L"TestPlayer", L"Com_Transform", Engine::ID_DYNAMIC));
	NULL_CHECK_RETURN(pPlayerTransform, -1);

	_vec3	vPlayerPos;
	pPlayerTransform->Get_Info(Engine::INFO_POS, &vPlayerPos);

	m_pTransformCom->Chase_Target(&vPlayerPos, m_fSpeed, fTimeDelta);

	return 0;
}
void Client::CTestMonster::Render_Object(void)
{
	m_pTransformCom->Set_Transform(m_pGraphicDev);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}
