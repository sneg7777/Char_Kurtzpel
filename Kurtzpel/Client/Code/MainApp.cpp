#include "stdafx.h"
#include "MainApp.h"
#include "Logo.h"
#include "UI_Manager.h"

CMainApp::CMainApp(void)
{

}

CMainApp::~CMainApp(void)
{

}

HRESULT CMainApp::Ready_MainApp(void)
{
	FAILED_CHECK_RETURN(SetUp_DefaultSetting(&m_pGraphicDev), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Get_Renderer()->Ready_Renderer(m_pGraphicDev), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Scene(m_pGraphicDev, &m_pManagementClass), E_FAIL);

	Client::Safe_Release(m_pDeviceClass);

	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Font_Default", L"바탕", 15, 20, FW_HEAVY), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font(m_pGraphicDev, L"Font_Jinji", L"궁서", 30, 30, FW_HEAVY), E_FAIL);

	//m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);	// Z버퍼에 기록은 한다. 단, 정렬을 수행할지 말지 결정
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE); // Z버퍼에 기록을 할지 말지 결정하는 옵션
	CRandom_Manager::Get_Instance()->Ready_Random();
	CUI_Manager::Get_Instance()->Ready_Object(m_pGraphicDev);

	return S_OK;
}

_int CMainApp::Update_MainApp(const _float& fTimeDelta)
{

	if (nullptr == m_pManagementClass)
		return -1;

	m_fTime += fTimeDelta;

		
	Engine::Set_InputDev();

	_long		Temp = 0;

	/*if (Temp = Engine::Get_DIMouseMove(Engine::DIMS_Z))
	{
		int a = 0;
	}*/


	m_pManagementClass->Update_Scene(fTimeDelta);
	m_pManagementClass->LateUpdate_Scene(fTimeDelta);

	return 0;
}

void CMainApp::Render_MainApp(void)
{
	if (nullptr == m_pManagementClass)
		return;

	++m_dwRenderCnt;

	if (m_fTime >= 1.f)
	{
		wsprintf(m_szFPS, L"FPS : %d", m_dwRenderCnt);
		m_fTime = 0.f;
		m_dwRenderCnt = 0;
	}

	//m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		
	Engine::Render_Begin(D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f));
	
	m_pManagementClass->Render_Scene(m_pGraphicDev);
	Engine::Render_Font(L"Font_Jinji", m_szFPS, &_vec2(500.f, 10.f), D3DXCOLOR(0.f, 0.f,0.f, 1.f));

	Engine::Render_End();
}

HRESULT CMainApp::SetUp_DefaultSetting(LPDIRECT3DDEVICE9 * ppGraphicDev)
{
	FAILED_CHECK_RETURN(Engine::Ready_GraphicDev(g_hWnd, Engine::MODE_WIN, WINCX, WINCY, &m_pDeviceClass), E_FAIL);
	Engine::Safe_AddRef(m_pDeviceClass);

	*ppGraphicDev = m_pDeviceClass->Get_GraphicDev();
	Engine::Safe_AddRef(*ppGraphicDev);

	// InputDev 설치
	FAILED_CHECK_RETURN(Engine::Ready_InputDev(g_hInst, g_hWnd), E_FAIL);
		
	return S_OK;
}
HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement ** ppManagement)
{
	Engine::CScene*		pScene = nullptr;

	FAILED_CHECK_RETURN(Engine::Create_Management(ppManagement, pGraphicDev), E_FAIL);
	Safe_AddRef(*ppManagement);

	pScene = CLogo::Create(pGraphicDev);
	NULL_CHECK_RETURN(pScene, E_FAIL);
	
	FAILED_CHECK_RETURN((*ppManagement)->SetUp_Scene(pScene), E_FAIL);

	return S_OK;
}

CMainApp* CMainApp::Create(void)
{
	CMainApp*	pInstance = new CMainApp;

	if (FAILED(pInstance->Ready_MainApp()))
	{
		MSG_BOX("CMainApp Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free(void)
{
	Client::Safe_Release(m_pGraphicDev);
	Client::Safe_Release(m_pManagementClass);

	CRandom_Manager::Destroy_Instance();
	CUI_Manager::Destroy_Instance();

	Engine::Release_Utility();
	Engine::Release_Resoures();
	Engine::Release_System();
}

