#include "stdafx.h"
#include "TestPlayer.h"
#include "Export_Function.h"

CTestPlayer::CTestPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_vDir(0.f, 0.f, 0.f)
{

}

CTestPlayer::~CTestPlayer(void)
{

}

HRESULT Client::CTestPlayer::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(Engine::RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_LOGO, L"Texture_Player"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Clone(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);
	return S_OK;
}

void Client::CTestPlayer::Key_Input(const _float& fTimeDelta)
{
	m_pTransformCom->Get_Info(Engine::INFO_UP, &m_vDir);

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		D3DXVec3Normalize(&m_vDir, &m_vDir);
		m_pTransformCom->Move_Pos(&(m_vDir * m_fSpeed * fTimeDelta));
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		D3DXVec3Normalize(&m_vDir, &m_vDir);
		m_pTransformCom->Move_Pos(&(m_vDir * -m_fSpeed * fTimeDelta));
	}

	if (GetAsyncKeyState('Q') & 0x8000)
		m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(90.f * fTimeDelta));

	if (GetAsyncKeyState('A') & 0x8000)
		m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(-90.f * fTimeDelta));

	if (GetAsyncKeyState('W') & 0x8000)
		m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(90.f * fTimeDelta));

	if (GetAsyncKeyState('S') & 0x8000)
		m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(-90.f * fTimeDelta));

	if (GetAsyncKeyState('E') & 0x8000)
		m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(90.f * fTimeDelta));

	if (GetAsyncKeyState('D') & 0x8000)
		m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(-90.f * fTimeDelta));
	
	
}

CTestPlayer* CTestPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTestPlayer*	pInstance = new CTestPlayer(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CTestPlayer::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CTestPlayer::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	//m_pTransformCom->m_vScale = _vec3(4.f, 2.f, 1.f);

	return S_OK;
}
Client::_int Client::CTestPlayer::Update_Object(const _float& fTimeDelta)
{

	Key_Input(fTimeDelta);
	Engine::CGameObject::Update_Object(fTimeDelta);


	return 0;
}
void Client::CTestPlayer::Render_Object(void)
{
	m_pTransformCom->Set_Transform(m_pGraphicDev);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Render_Texture(0);
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}
