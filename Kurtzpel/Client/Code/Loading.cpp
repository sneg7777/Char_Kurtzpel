#include "stdafx.h"
#include "Loading.h"

#include "Export_Function.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bFinish(false)
{
	ZeroMemory(m_szLoading, sizeof(_tchar) * 256);
	m_pGraphicDev->AddRef();
}

CLoading::~CLoading(void)
{

}

_uint	CALLBACK CLoading::Thread_Main(void* pArg)
{
	CLoading*		pLoading = (CLoading*)pArg;

	_uint iFlag = 0;

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_STAGE:
		iFlag = pLoading->Loading_ForStage();		
		break;

	case LOADING_BOSS:
		break;
	}
	
	LeaveCriticalSection(pLoading->Get_Crt());
	_endthreadex(0);

	return iFlag;
}

HRESULT CLoading::Ready_Loading(LOADINGID eLoading)
{
	InitializeCriticalSection(&m_Crt);		// 크리티컬 섹션 생성

	// 쓰레드 생성 함수
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, Thread_Main, this, 0, NULL);

	m_eLoading = eLoading;

	return S_OK;
}

_uint CLoading::Loading_ForStage(void)
{
	lstrcpy(m_szLoading, L"Buffer Loading.............................");
	
	int i = 0;

	// component
	Engine::CComponent*		pComponent = nullptr;

	pComponent = Engine::CCalculator::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::Ready_Proto(L"Proto_Calculator", pComponent);
	
	pComponent = Engine::COptimization::Create(m_pGraphicDev, true, VTXCNTX, VTXCNTZ);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Engine::Ready_Proto(L"Proto_Optimization", pComponent);


	// buffer

	//FAILED_CHECK_RETURN(Engine::Ready_Buffer(m_pGraphicDev, Engine::RESOURCE_STATIC, L"Buffer_RcTex", Engine::BUFFER_RCTEX), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Buffer(m_pGraphicDev,
												Engine::RESOURCE_STATIC,
												L"Buffer_TerrainTex",
												Engine::BUFFER_TERRAINTEX,
												VTXCNTX,
												VTXCNTZ,
												VTXITV),
												E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Buffer(m_pGraphicDev,
												Engine::RESOURCE_STATIC,
												L"Buffer_CubeTex",
												Engine::BUFFER_CUBETEX),
												E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Buffer(m_pGraphicDev,
												Engine::RESOURCE_STATIC,
												L"Buffer_CollSphere",
												Engine::BUFFER_COLLSPHERE),
												E_FAIL);


	lstrcpy(m_szLoading, L"Texture Loading.............................");
	// 텍스쳐

	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Logo", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Logo/Logo.jpg"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Player", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Player/Ma.jpg"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Monster", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Monster%d.png"), E_FAIL);

	//FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev,
	//											Engine::RESOURCE_STAGE,
	//											L"Texture_Terrain",
	//											Engine::TEX_NORMAL,
	//											L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2),
	//											E_FAIL);
		FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Texture_Terrain",
												Engine::TEX_NORMAL,
												L"../Bin/Resource/Texture/Terrain/Ground_%d.tga", 2),
												E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev,
		Engine::RESOURCE_STAGE,
		L"Texture_Filter",
		Engine::TEX_NORMAL,
		L"../Bin/Resource/Texture/Terrain/Filter.bmp", 1),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev,
		Engine::RESOURCE_STAGE,
		L"Texture_Aura",
		Engine::TEX_NORMAL,
		L"../Bin/Resource/Texture/Terrain/Aura0.tga", 1),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev,
		Engine::RESOURCE_STAGE,
		L"Texture_SkyBox",
		Engine::TEX_CUBE,
		L"../Bin/Resource/Texture/SkyBox/burger%d.dds", 4),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Effect", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Explosion/Explosion%d.png", 90), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_SkillFrame", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ul/SkillFrame.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_RageSkillFrame", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ul/RageSkillFrame.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillZ", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ul/GH_RageSkill.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillE", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ul/GH_SkillE.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillF", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ul/GH_SkillF.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillQ", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ul/GH_SkillQ.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_LBSkillZ", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ul/LB_RageSkill.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_SkillAvoid", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ul/Skill_Avoid.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_SkillWeaponChange", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ul/Skill_WeaponChange.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_KeyQ", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ul/Key_Q.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_KeyE", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ul/Key_E.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_KeyF", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ul/Key_F.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_KeyZ", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ul/Key_Z.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_KeyTab", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ul/Key_Tab.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_KeyLShift", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Ul/Key_LShift.tga"), E_FAIL);
	
	lstrcpy(m_szLoading, L"Mesh Loading.............................");
	///////////////////////////////////////////////// Static Mesh
	/*FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_Stone", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/TombStone/",
												L"TombStone.X"),
												E_FAIL);*/
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_ZonePlace", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/ZonePlace/",
												L"Zone_Place.X"),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_PillarTo", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/PillarTo/",
												L"PillarTo.X"),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_GlassTo", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/GlassTo/",
												L"GlassTo.X"),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_GlassTo2", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/GlassTo2/",
												L"GlassTo2.X"),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_Door", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/Door/",
												L"Door.X"),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_Fence", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/Fence/",
												L"Fence.X"),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_Platform", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/Platform/",
												L"Platform.X"),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_ShapeCell", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/ShapeCell/",
												L"ShapeCell.X"),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_Arrow", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/Arrow/",
												L"Arrow.X"),
												E_FAIL);

	/////////////////////////////////////////////////

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_Navi",
												Engine::TYPE_NAVI,
												NULL,
												NULL),
												E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_Player",
												Engine::TYPE_DYNAMIC,
												L"../Bin/Resource/Mesh/DynamicMesh/Char/",
												L"Player_1.X"),
												E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_M_ApostleOfGreed",
												Engine::TYPE_DYNAMIC,
												L"../Bin/Resource/Mesh/DynamicMesh/Monster/ApostleOfGreed/",
												L"ApostleOfGreed.X"),
												E_FAIL);

	/*FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_Equip_Top_01",
												Engine::TYPE_STATIC,
												L"../Bin/Resource/Mesh/DynamicMesh/Char/",
												L"Equip_Top_01.X"),
												E_FAIL);*/

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
													Engine::RESOURCE_STAGE,
													L"Mesh_Hammer",
													Engine::TYPE_STATIC,
													L"../Bin/Resource/Mesh/StaticMesh/Hammer/",
													L"Hammer.X"),
													E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
													Engine::RESOURCE_STAGE,
													L"Mesh_LongBow",
													Engine::TYPE_STATIC,
													L"../Bin/Resource/Mesh/StaticMesh/LongBow/",
													L"LongBow.X"),
													E_FAIL);
	/*FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		Engine::RESOURCE_STAGE,
		L"Mesh_Sword",
		Engine::TYPE_STATIC,
		L"../Bin/Resource/Mesh/StaticMesh/Sword/",
		L"Sword.X"),
		E_FAIL);*/

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
											Engine::RESOURCE_STAGE,
											L"Mesh_Tree",
											Engine::TYPE_STATIC,
											L"../Bin/Resource/Mesh/StaticMesh/Tree/",
											L"Tree01.X"),
											E_FAIL);
	
	
	lstrcpy(m_szLoading, L"Loading Complete!!!");

	m_bFinish = true;


	return 0;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eLoading)
{
	CLoading*	pInstance = new CLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eLoading)))
		Safe_Release(pInstance);

	return pInstance;

}

void CLoading::Free(void)
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);

	Engine::Safe_Release(m_pGraphicDev);
}

