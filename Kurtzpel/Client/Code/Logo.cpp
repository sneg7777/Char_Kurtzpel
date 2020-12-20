#include "stdafx.h"
#include "Logo.h"
#include "Export_Function.h"

#include "Stage.h"


CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{

}

CLogo::~CLogo(void)
{

}

HRESULT CLogo::Ready_Scene(void)
{
	FAILED_CHECK_RETURN(Ready_Resource(Engine::RESOURCE_END), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Environment_Layer(L"Environment"), E_FAIL);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE);
	FAILED_CHECK_RETURN(m_pLoading, E_FAIL);


	return S_OK;
}

Engine::_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	if (true == m_pLoading->Get_Finish())
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			CScene*		pScene = nullptr;
			pScene = CStage::Create(m_pGraphicDev);

			FAILED_CHECK_RETURN(Engine::SetUp_Scene(pScene), E_FAIL);

			return iExit;
		}	
	}

	return iExit;
}

void CLogo::Render_Scene(void)
{
	// DEBUG ¿ë
	Engine::Render_Font(L"Font_Jinji", m_pLoading->Get_LoadString(), &_vec2(10.f, 10.f), D3DXCOLOR(1.f, 0.f, 0.f, 1.f));

}

HRESULT CLogo::Ready_Environment_Layer(const _tchar * pLayerTag)
{
	Engine::CLayer*			pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	
	Engine::CGameObject*		pGameObject = nullptr;

	pGameObject = CBackGround::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", pGameObject), E_FAIL);

	/*pGameObject = CTestPlayer::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"TestPlayer", pGameObject), E_FAIL);

	pGameObject = CTestMonster::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"TestMonster", pGameObject), E_FAIL);*/

	
	m_mapLayer.emplace(pLayerTag, pLayer);
	
	return S_OK;

	/*pGameObject = CBackGround::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		goto except;
	
	if (FAILED(pLayer->Add_GameObject(L"BackGround", pGameObject)))
		goto except;
	
	m_mapLayer.emplace(pLayerTag, pLayer);

	return S_OK;

except:
	Client::Safe_Release(pLayer);

	return E_FAIL;*/
}

HRESULT CLogo::Ready_GameLogic_Layer(const _tchar * pLayerTag)
{
	return S_OK;
}

HRESULT CLogo::Ready_UI_Layer(const _tchar * pLayerTag)
{
	return S_OK;
}

HRESULT Client::CLogo::Ready_Resource(Engine::RESOURCETYPE eType)
{
	FAILED_CHECK_RETURN(Engine::Reserve_ContainerSize(eType), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Buffer(m_pGraphicDev, Engine::RESOURCE_STATIC, L"Buffer_TriCol", Engine::BUFFER_TRICOL), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Buffer(m_pGraphicDev, Engine::RESOURCE_STATIC, L"Buffer_RcTex", Engine::BUFFER_RCTEX), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_LOGO, L"Texture_Logo", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Logo/Logo.jpg"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_LOGO, L"Texture_Player", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Player/Ma.jpg"), E_FAIL);

	Engine::CComponent*		pComponent = nullptr;

	pComponent = Engine::CTransform::Create();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::Ready_Proto(L"Proto_Transform", pComponent);


	return S_OK;
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLogo*	pInstance = new CLogo(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CLogo::Free(void)
{
	Safe_Release(m_pLoading);

	Engine::CScene::Free();
}

