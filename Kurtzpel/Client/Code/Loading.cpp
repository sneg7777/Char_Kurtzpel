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

	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Logo", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Logo/Logo.tga"), E_FAIL);

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
		L"Texture_SkillE_Terrain",
		Engine::TEX_NORMAL,
		L"../Bin/Resource/Texture/UI/LB_SkillE_Terrain.tga", 1),
		//L"../Bin/Resource/Texture/Terrain/Aura0.tga", 1),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev,
		Engine::RESOURCE_STAGE,
		L"Texture_SkyBox",
		Engine::TEX_CUBE,
		L"../Bin/Resource/Texture/SkyBox/burger%d.dds", 4),
		E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Effect", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Explosion/Explosion%d.png", 90), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_SkillFrame", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/SkillFrame.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_SkillFrameBlue", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/SkillFrameBlue.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_SkillFrameOrange", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/SkillFrameOrange.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_SkillFrameGreen", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/SkillFrameGreen.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_RageSkillFrame", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/RageSkillFrame.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillZ", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/GH_RageSkill.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillE", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/GH_SkillE.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillF", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/GH_SkillF.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_GHSkillQ", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/GH_SkillQ.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_LBSkillZ", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/LB_RageSkill.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_LBSkillE", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/LB_SkillE.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_LBSkillF", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/LB_SkillF.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_LBSkillQ", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/LB_SkillQ.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_SkillAvoid", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Skill_Avoid.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_SkillWeaponChange", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Skill_WeaponChange.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_KeyQ", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Key_Q.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_KeyE", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Key_E.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_KeyF", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Key_F.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_KeyZ", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Key_Z.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_KeyTab", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Key_Tab.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_KeyLShift", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Key_LShift.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_KeyEnter", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Key_Enter.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_Player_Hp0", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player_Hp_0.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_Player_Hp1", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player_Hp_1.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_Player_Hp2", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player_Hp_2.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_Player_Hp3", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player_Hp_3.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_Monster_Hp0", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Monster_Hp_0.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_Monster_Hp1", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Monster_Hp_1.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_Monster_Hp2", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Monster_Hp_2.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_Monster_Hp3", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/Monster_Hp_3.tga"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_UI_LireArt", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/UI/LireArt.png"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_dncmrdp", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/dncmrdp.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_znptmxm", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/znptmxm.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_npcdhk", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/npcdhk.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_vhxkf", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/vhxkf.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_0dkssud", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/0dkssud.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_1dntjs", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/1dntjs.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_2tmvpdltm", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/2tmvpdltm.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_3ekdmadms", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/3ekdmadms.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_4dlehddmf", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/4dlehddmf.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_5dksekrh", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/5dksekrh.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_6rnfmrl", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/6rnfmrl.png"), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_7dlwpdkv", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/7dlwpdkv.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_7dlwpenl", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/7dlwpenl.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_8duftlagl", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/8duftlagl.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_9ghkfdmf", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/9ghkfdmf.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_10sorkrkw", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/10sorkrkw.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_11shffkdns", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/11shffkdns.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_12ektlans", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/12ektlans.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_13dlwpak", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/13dlwpak.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Text_99wkfgotdj", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Text/99wkfgotdj.png"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_DissolveTex", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/DissolveTex.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Effect_Test", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Effect/Test.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Effect_Test2", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Effect/Test2.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Effect_Portal", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Effect/Portal2.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Effect_BasicAttack01", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Effect/BasicAttack01.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Effect_GHHit01", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Effect/GH_Hit01.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Effect_LBHit01", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Effect/LB_Hit01.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Effect_PhoenixHit01", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Effect/PhoenixHit01.tga"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(m_pGraphicDev, Engine::RESOURCE_STAGE, L"Texture_Effect_GH_RockHit", Engine::TEX_NORMAL, L"../Bin/Resource/Texture/Effect/GH_RockHit.tga"), E_FAIL);
	
	
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
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_Stage2_Wall01", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/Stage2_Wall01/",
												L"Stage2_Wall01.X"),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_Stage2_Wall02", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/Stage2_Wall02/",
												L"Stage2_Wall02.X"),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_Stage2_Wall03", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/Stage2_Wall03/",
												L"Stage2_Wall03.X"),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_Stage2_WoodFence", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/Stage2_WoodFence/",
												L"Stage2_WoodFence.X"),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_Stage2_WoodFence2", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/Stage2_WoodFence2/",
												L"Stage2_WoodFence2.X"),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_Monster2_CrossBowArrow", 
												Engine::TYPE_STATIC, 
												L"../Bin/Resource/Mesh/StaticMesh/Monster2_CrossBowArrow/",
												L"Monster2_CrossBowArrow.X"),
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
												L"Mesh_Navi2",
												Engine::TYPE_NAVI,
												NULL,
												NULL, 2),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_ArrowNavi",
												Engine::TYPE_ARROWNAVI,
												NULL,
												NULL),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_ArrowNavi2",
												Engine::TYPE_ARROWNAVI,
												NULL,
												NULL, 2),
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
												L"Mesh_Lire",
												Engine::TYPE_DYNAMIC,
												L"../Bin/Resource/Mesh/DynamicMesh/Lire/",
												L"Lire.X"),
												E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_Phoenix",
												Engine::TYPE_DYNAMIC,
												L"../Bin/Resource/Mesh/DynamicMesh/Phoenix/",
												L"Phoenix.X"),
												E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_M_ApostleOfGreed",
												Engine::TYPE_DYNAMIC,
												L"../Bin/Resource/Mesh/DynamicMesh/Monster/ApostleOfGreed/",
												L"ApostleOfGreed.X"),
												E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_M_Monster1_TwoHand",
												Engine::TYPE_DYNAMIC,
												L"../Bin/Resource/Mesh/DynamicMesh/Monster/Monster1_TwoHand/",
												L"Monster1_TwoHand.X"),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_M_Monster2_CrossBow",
												Engine::TYPE_DYNAMIC,
												L"../Bin/Resource/Mesh/DynamicMesh/Monster/Monster2_CrossBow/",
												L"Monster2_CrossBow.X"),
												E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_GH_SkillF_StoneIn",
												Engine::TYPE_DYNAMIC,
												L"../Bin/Resource/Mesh/DynamicMesh/GH_SkillF_StoneIn/",
												L"GH_SkillF_StoneIn.X"),
												E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
												Engine::RESOURCE_STAGE,
												L"Mesh_GH_SkillF_StoneOut",
												Engine::TYPE_DYNAMIC,
												L"../Bin/Resource/Mesh/DynamicMesh/GH_SkillF_StoneOut/",
												L"GH_SkillF_StoneOut.X"),
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
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
													Engine::RESOURCE_STAGE,
													L"Mesh_GH_Trace01",
													Engine::TYPE_STATIC,
													L"../Bin/Resource/Mesh/StaticMesh/GH_Trace01/",
													L"GH_Trace01.X"),
													E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
													Engine::RESOURCE_STAGE,
													L"Mesh_GH_Trace02",
													Engine::TYPE_STATIC,
													L"../Bin/Resource/Mesh/StaticMesh/GH_Trace01/",
													L"GH_Trace02.X"),
													E_FAIL);
		FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
													Engine::RESOURCE_STAGE,
													L"Mesh_GH_Trace03",
													Engine::TYPE_STATIC,
													L"../Bin/Resource/Mesh/StaticMesh/GH_Trace01/",
													L"GH_Trace03.X"),
													E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
													Engine::RESOURCE_STAGE,
													L"Mesh_Effect01",
													Engine::TYPE_STATIC,
													L"../Bin/Resource/Mesh/StaticMesh/Effect/",
													L"Effect01.X"),
													E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
													Engine::RESOURCE_STAGE,
													L"Mesh_Effect02",
													Engine::TYPE_STATIC,
													L"../Bin/Resource/Mesh/StaticMesh/Effect/",
													L"Effect02.X"),
													E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
													Engine::RESOURCE_STAGE,
													L"Mesh_Effect03",
													Engine::TYPE_STATIC,
													L"../Bin/Resource/Mesh/StaticMesh/Effect/",
													L"Effect03.X"),
													E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
													Engine::RESOURCE_STAGE,
													L"Mesh_Effect04",
													Engine::TYPE_STATIC,
													L"../Bin/Resource/Mesh/StaticMesh/Effect/",
													L"Effect04.X"),
													E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
													Engine::RESOURCE_STAGE,
													L"Mesh_Effect05",
													Engine::TYPE_STATIC,
													L"../Bin/Resource/Mesh/StaticMesh/Effect/",
													L"Effect05.X"),
													E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
													Engine::RESOURCE_STAGE,
													L"Mesh_Effect06",
													Engine::TYPE_STATIC,
													L"../Bin/Resource/Mesh/StaticMesh/Effect/",
													L"Effect06.X"),
													E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
													Engine::RESOURCE_STAGE,
													L"Mesh_Effect07",
													Engine::TYPE_STATIC,
													L"../Bin/Resource/Mesh/StaticMesh/Effect/",
													L"Effect07.X"),
													E_FAIL);
	/*FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
		Engine::RESOURCE_STAGE,
		L"Mesh_Sword",
		Engine::TYPE_STATIC,
		L"../Bin/Resource/Mesh/StaticMesh/Sword/",
		L"Sword.X"),
		E_FAIL);*/

	/*FAILED_CHECK_RETURN(Engine::Ready_Meshes(m_pGraphicDev,
											Engine::RESOURCE_STAGE,
											L"Mesh_Tree",
											Engine::TYPE_STATIC,
											L"../Bin/Resource/Mesh/StaticMesh/Tree/",
											L"Tree01.X"),
											E_FAIL);*/
	
	
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

