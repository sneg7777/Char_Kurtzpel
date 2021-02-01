#include "Export_Utility.h"

USING(Engine)
IMPLEMENT_SINGLETON(CManagement)

Engine::CManagement::CManagement(void)
	: m_pScene(nullptr)
{

}

Engine::CManagement::~CManagement(void)
{
	Free();
}

HRESULT CManagement::Ready_Shader(LPDIRECT3DDEVICE9& pGraphicDev)
{
	D3DVIEWPORT9		ViewPort;
	pGraphicDev->GetViewport(&ViewPort);

	FAILED_CHECK_RETURN(Ready_RenderTarget(pGraphicDev,
		L"Target_Albedo",
		ViewPort.Width,
		ViewPort.Height,
		D3DFMT_A16B16G16R16F,
		D3DXCOLOR(0.f, 0.f, 0.f, 0.f)),
		E_FAIL);

	FAILED_CHECK_RETURN(Ready_DebugBuffer(L"Target_Albedo", 0.f, 0.f, 100.f, 100.f), E_FAIL);

	FAILED_CHECK_RETURN(Ready_RenderTarget(pGraphicDev,
		L"Target_Normal",
		ViewPort.Width,
		ViewPort.Height,
		D3DFMT_A16B16G16R16F,
		D3DXCOLOR(0.f, 0.f, 0.f, 1.f)),
		E_FAIL);
	FAILED_CHECK_RETURN(Ready_DebugBuffer(L"Target_Normal", 0.f, 100.f, 100.f, 100.f), E_FAIL);

	FAILED_CHECK_RETURN(Ready_RenderTarget(pGraphicDev,
		L"Target_Shade",
		ViewPort.Width,
		ViewPort.Height,
		D3DFMT_A16B16G16R16F,
		D3DXCOLOR(0.f, 0.f, 0.f, 1.f)),
		E_FAIL);
	FAILED_CHECK_RETURN(Ready_DebugBuffer(L"Target_Shade", 0.f, 200.f, 100.f, 100.f), E_FAIL);

	FAILED_CHECK_RETURN(Ready_RenderTarget(pGraphicDev,
		L"Target_Specular",
		ViewPort.Width,
		ViewPort.Height,
		D3DFMT_A16B16G16R16F,
		D3DXCOLOR(0.f, 0.f, 0.f, 0.f)),
		E_FAIL);
	FAILED_CHECK_RETURN(Ready_DebugBuffer(L"Target_Specular", 0.f, 300.f, 100.f, 100.f), E_FAIL);

	FAILED_CHECK_RETURN(Ready_RenderTarget(pGraphicDev,
		L"Target_Depth",
		ViewPort.Width,
		ViewPort.Height,
		D3DFMT_A32B32G32R32F,
		D3DXCOLOR(1.f, 1.f, 1.f, 1.f)),
		E_FAIL);
	FAILED_CHECK_RETURN(Ready_DebugBuffer(L"Target_Depth", 0.f, 400.f, 100.f, 100.f), E_FAIL);


	FAILED_CHECK_RETURN(Ready_MRT(L"MRT_Deferred", L"Target_Albedo"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_MRT(L"MRT_Deferred", L"Target_Normal"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_MRT(L"MRT_Deferred", L"Target_Depth"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_MRT(L"MRT_LightAcc", L"Target_Shade"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_MRT(L"MRT_LightAcc", L"Target_Specular"), E_FAIL);


	CShader* pShader = nullptr;

	// Sample
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Sample.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Proto(L"Proto_Shader_Sample", pShader), E_FAIL);

	// Terrain
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Terrain.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Proto(L"Proto_Shader_Terrain", pShader), E_FAIL);

	// Terrain
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Mesh.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Proto(L"Proto_Shader_Mesh", pShader), E_FAIL);

	// Shade
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Shade.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Proto(L"Proto_Shader_Shade", pShader), E_FAIL);

	// Blend
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Blend.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Proto(L"Proto_Shader_Blend", pShader), E_FAIL);

	// SkyBox
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_SkyBox.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Proto(L"Proto_Shader_SkyBox", pShader), E_FAIL);

	// Effect
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Effect.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Proto(L"Proto_Shader_Effect", pShader), E_FAIL);

	// Unit
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Unit.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Proto(L"Proto_Shader_Unit", pShader), E_FAIL);

	// Texture
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Texture.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Proto(L"Proto_Shader_Texture", pShader), E_FAIL);

	// EffectMesh
	pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_EffectMesh.hpp");
	NULL_CHECK_RETURN(pShader, E_FAIL);
	FAILED_CHECK_RETURN(Ready_Proto(L"Proto_Shader_EffectMesh", pShader), E_FAIL);

	// Test
	//pShader = CShader::Create(pGraphicDev, L"../../Reference/Header/Shader_Test.hpp");
	//NULL_CHECK_RETURN(pShader, E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Proto(L"Proto_Shader_Test", pShader), E_FAIL);

	return S_OK;
}

HRESULT Engine::CManagement::SetUp_Scene(CScene* pScene)
{
	Safe_Release(m_pScene); // 

	m_pScene = pScene;

	return S_OK;
}

Engine::_int Engine::CManagement::Update_Scene(const _float& fTimeDelta)
{
	if (nullptr == m_pScene)
		return -1;

	m_pScene->Update_Scene(fTimeDelta);

	return 0;
}

Engine::_int Engine::CManagement::LateUpdate_Scene(const _float& fTimeDelta)
{
	if (nullptr == m_pScene)
		return -1;

	m_pScene->LateUpdate_Scene(fTimeDelta);

	return 0;
}

void Engine::CManagement::Render_Scene(LPDIRECT3DDEVICE9& pGraphicDev)
{
	Get_Renderer()->Render_GameObject(pGraphicDev);

	if (nullptr == m_pScene)
		return;

	m_pScene->Render_Scene();
}

void Engine::CManagement::Free(void)
{
	Safe_Release(m_pScene);
}

Engine::CComponent* Engine::CManagement::Get_Component(const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag, COMPONENTID eID)
{
	if (nullptr == m_pScene)
		return nullptr;

	return m_pScene->Get_Component(pLayerTag, pObjTag, pComponentTag, eID);
}

Engine::CGameObject* Engine::CManagement::Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag)
{
	if (nullptr == m_pScene)
		return nullptr;

	return m_pScene->Get_GameObject(pLayerTag, pObjTag);
}


