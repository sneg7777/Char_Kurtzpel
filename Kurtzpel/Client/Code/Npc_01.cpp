#include "stdafx.h"
#include "Npc_01.h"
#include "Export_Function.h"
#include "DynamicCamera.h"
#include "SphereCollider.h"
#include "Player.h"
#include "Stage.h"
#include "Unit_D.h"
#include "Random_Manager.h"
#include "UI_Manager.h"
#include "NpcQuest_Manager.h"
#include "Monster1_TwoHand.h"
#include "CameraScene_Manager.h"

CNpc_01::CNpc_01(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit_D(pGraphicDev)
{
	//m_sStat.m_dwNaviIndex = 0;
}

CNpc_01::~CNpc_01(void)
{

}

HRESULT Client::CNpc_01::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	CUnit_D::Add_Component();
	// Mesh
	pComponent = m_sComponent.m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Lire"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);
	m_sComponent.m_pMeshCom->Set_AniAngle(230.f);
	//m_pMeshCom->Set_AniAngle(275.f);
	//
	m_sComponent.m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(-80.f), true);
	m_sComponent.m_pTransformCom->Set_Pos(&_vec3(92.2f, 0.f, 59.f));
	//m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	Engine::CGameObject::Update_Object(0.f);
	
	//m_pMeshCom->Play_Animation(1.f);

	Load_ColliderFile(L"../Bin/Resource/Mesh/DynamicMesh/Save/Lire.dat");
	for (auto& sphere : m_VecSphereCollider)
	{
		sphere->m_pDynamicThis = this;
		if (!sphere->m_FrameName.compare("Core")) {
			sphere->m_BonePart = CSphereCollider::BonePart_CollBody;
		}
		else if (!sphere->m_FrameName.compare("Pelvis")) {
			sphere->m_BonePart = CSphereCollider::BonePart_NpcTalk;
		}
		sphere->m_BoneTeam = CSphereCollider::BoneTeam_Npc;
	}
	//m_pTransformCom->Set_Pos(&_vec3(52.f, 0.f, 52.f));
	//TODO 그냥 레디에서 Y회전하기
	return S_OK;
}

HRESULT CNpc_01::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_sComponent.m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	if (CNpcQuest_Manager::Get_Instance()->Get_NpcQuestInfo()->m_PlayerTalk) {
		_vec4 vColor = { 0.f, 0.f, 0.f, 1.f };
		pEffect->SetVector("g_vColor", &vColor);
		pEffect->SetFloat("g_fBoldSize", 0.005f);
	}
	else if (CNpcQuest_Manager::Get_Instance()->Get_NpcQuestInfo()->m_NpcColl) {
		_vec4 vColor = { 171.f / 255.f, 242.f / 255.f, 0.f / 255.f, 1.f };
		pEffect->SetVector("g_vColor", &vColor);
		pEffect->SetFloat("g_fBoldSize", 0.01f);
	}
	else {
		_vec4 vColor = { 0.f, 0.f, 0.f, 1.f };
		pEffect->SetVector("g_vColor", &vColor);
		pEffect->SetFloat("g_fBoldSize", 0.01f);
	}
	pEffect->SetBool("g_bIsDissolve", false);

	return S_OK;
}


CNpc_01* CNpc_01::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CNpc_01*	pInstance = new CNpc_01(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);
	return pInstance;
}

void CNpc_01::Free(void)
{
	for (auto& hpdec : m_VecDelayHpDec) 
		Engine::Safe_Delete(hpdec);
	m_VecDelayHpDec.clear();

	CUnit_D::Free();
}


HRESULT Client::CNpc_01::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_sComponent.m_pTransformCom->Set_Scale(0.013f, 0.013f, 0.013f);
	m_sComponent.m_pMeshCom->Set_AnimationSet(0);
	m_AniSpeed = 1.f;
	m_UnitName = UnitName::Npc;

	return S_OK;
}

Client::_int Client::CNpc_01::Update_Object(const _float& fTimeDelta)
{
	if (m_sStat.m_IsDead)
		return 1;

	NpcNextTalkKey();

	Calc_Time(fTimeDelta);
	SetUp_OnTerrain();

	CUnit_D::Update_Object(fTimeDelta);
	m_sComponent.m_pMeshCom->Play_Animation(fTimeDelta * m_AniSpeed);

	m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}

Client::_int Client::CNpc_01::LateUpdate_Object(const _float& fTimeDelta)
{

	

	return 0;
}

void Client::CNpc_01::Render_Object(void)
{
	LPD3DXEFFECT	 pEffect = m_sComponent.m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;
	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
	for (_int i = 0; i < iMaxPass; i++)
	{
		pEffect->BeginPass(i);

		FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

		m_sComponent.m_pMeshCom->Render_Meshes(pEffect);

		pEffect->EndPass();
	}
	pEffect->End();

	Engine::Safe_Release(pEffect);
}

void Client::CNpc_01::SetUp_OnTerrain(void)
{
	_vec3	vPosition;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_Terrain->Get_sComponent_Terrain()->m_pBufferCom;
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_sComponent.m_pCalculatorCom->Compute_HeightOnTerrain(&vPosition, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_sComponent.m_pTransformCom->Move_Pos(vPosition.x, fHeight, vPosition.z);
}
 

void Client::CNpc_01::Calc_Time(_float fTimeDelta)
{
	
}

void Client::CNpc_01::Talk_Rocate()
{
	_vec3 vDir;
	CPlayer::GetInstance()->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_POS, &vDir);
	m_sComponent.m_pTransformCom->Chase_Target(&vDir, 1.f, 0.f);
}

void CNpc_01::NpcNextTalkKey() {
	if (CNpcQuest_Manager::Get_NpcQuestInfo()->m_PlayerTalk && !m_KeyEnter) {
		if ((Engine::Get_DIKeyState(DIK_RETURN) & 0x80)) {
			m_KeyEnter = true;
			NpcNextTalk();
		}
	}
	else if (!(Engine::Get_DIKeyState(DIK_RETURN) & 0x80)) {
		m_KeyEnter = false;
	}
}

void CNpc_01::NpcNextTalk() {
	CNpcQuest_Manager* questMgr = CNpcQuest_Manager::Get_Instance();
	CUI_Manager* uiMgr = CUI_Manager::Get_Instance();
	uiMgr->Delete_TalkText();
	int talkNumber = questMgr->Get_NpcQuestInfo()->m_TalkNumber += 1;
	switch (questMgr->Get_NpcQuestInfo()->m_QuestNumber)
	{
		case 0: {
			if (talkNumber == 1)
				uiMgr->Create_Text(CUI::UIKind::UIK_TalkText, L"Texture_Text_1dntjs");
			else if (talkNumber == 2)
				uiMgr->Create_Text(CUI::UIKind::UIK_TalkText, L"Texture_Text_2tmvpdltm");
			else if (talkNumber == 3) {
				questMgr->Get_NpcQuestInfo()->m_PlayerTalk = false;
				questMgr->Get_NpcQuestInfo()->m_AttackCount = 0;
				questMgr->Get_NpcQuestInfo()->m_TalkEnd = true;
				questMgr->Get_NpcQuestInfo()->m_QuestNumber++;

			}
			break;
		}
		case 2: {
			CNpcQuest_Manager::Get_NpcQuestInfo()->m_PlayerTalk = false;
			questMgr->Get_NpcQuestInfo()->m_QuestNumber++;
			questMgr->Get_NpcQuestInfo()->m_SkillQCount = 0;
			questMgr->Get_NpcQuestInfo()->m_SkillECount = 0;
			questMgr->Get_NpcQuestInfo()->m_SkillFCount = 0;
			questMgr->Get_NpcQuestInfo()->m_TalkEnd = true;
			break;
		}
		case 4: {
			if (talkNumber == 1)
				uiMgr->Create_Text(CUI::UIKind::UIK_TalkText, L"Texture_Text_5dksekrh");
			else {
				questMgr->Get_NpcQuestInfo()->m_PlayerTalk = false;
				questMgr->Get_NpcQuestInfo()->m_QuestNumber++;
				questMgr->Get_NpcQuestInfo()->m_RollingCount = 0;
				questMgr->Get_NpcQuestInfo()->m_TalkEnd = true;
			}
			break;
		}
		case 6: {
			if (talkNumber == 1)
				uiMgr->Create_Text(CUI::UIKind::UIK_TalkText, L"Texture_Text_7dlwpenl");
			else {
				questMgr->Get_NpcQuestInfo()->m_PlayerTalk = false;
				questMgr->Get_NpcQuestInfo()->m_TalkEnd = true;
				questMgr->Get_NpcQuestInfo()->m_QuestNumber++;
				Engine::CGameObject* pGameObject = CMonster1_TwoHand::Create(m_pGraphicDev);
				NULL_CHECK_RETURN(pGameObject, );
				Engine::CManagement::GetInstance()->m_pScene->Get_Layer(Engine::CLayer::Layer_Dynamic)->Add_GameObject(L"M_Monster1_TwoHand", pGameObject);
				dynamic_cast<CMonster1_TwoHand*>(pGameObject)->Get_sComponent()->m_pTransformCom->Set_Pos(41.7f, 0.f, 47.8f);
				pGameObject->Engine::CGameObject::Update_Object(0.f);
				dynamic_cast<CUnit*>(pGameObject)->Get_sComponent()->m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(240.f));
				CCameraScene_Manager::Get_Instance()->Set_CameraScene(1);
			}
			break;
		}
		case 8: {
			if (talkNumber == 1) {
				uiMgr->Create_Text(CUI::UIKind::UIK_TalkText, L"Texture_Text_9ghkfdmf");
				CPlayer::GetInstance()->m_bCheck[CPlayer::bCheck::bCheck_WeaponChange] = true;
				questMgr->Get_NpcQuestInfo()->m_WeaponChange = true;
			}
			else if (talkNumber == 2)
				uiMgr->Create_Text(CUI::UIKind::UIK_TalkText, L"Texture_Text_10sorkrkw");
			else if (talkNumber == 3)
				uiMgr->Create_Text(CUI::UIKind::UIK_TalkText, L"Texture_Text_12ektlans");
			else {
				questMgr->Get_NpcQuestInfo()->m_PlayerTalk = false;
				questMgr->Get_NpcQuestInfo()->m_TalkEnd = true;
				questMgr->Get_NpcQuestInfo()->m_QuestNumber++;
			}
			break;
		}
		case 10: {
			questMgr->Get_NpcQuestInfo()->m_PlayerTalk = false;
			questMgr->Get_NpcQuestInfo()->m_TalkEnd = true;
			questMgr->Get_NpcQuestInfo()->m_QuestNumber++;
			break;
		}
		default: {
			questMgr->Get_NpcQuestInfo()->m_TalkEnd = true;
			questMgr->Get_NpcQuestInfo()->m_PlayerTalk = false;
			break;
		}
	}
	return;
}

void CNpc_01::NpcQuest()
{
	CNpcQuest_Manager* questMgr = CNpcQuest_Manager::Get_Instance();
	CUI_Manager* uiMgr = CUI_Manager::Get_Instance();
	uiMgr->Delete_QuestClearText();
	questMgr->Get_NpcQuestInfo()->m_TalkNumber = 0;
	switch (questMgr->Get_NpcQuestInfo()->m_QuestNumber)
	{
		case 0: {
			uiMgr->Create_Text(CUI::UIKind::UIK_TalkText, L"Texture_Text_0dkssud");
			break;
		}
		case 2: {
			uiMgr->Create_Text(CUI::UIKind::UIK_TalkText, L"Texture_Text_3ekdmadms");
			break;
		}
		case 4: {
			uiMgr->Create_Text(CUI::UIKind::UIK_TalkText, L"Texture_Text_4dlehddmf");
			break;
		}
		case 6: {
			uiMgr->Create_Text(CUI::UIKind::UIK_TalkText, L"Texture_Text_6rnfmrl");
			break;
		}
		case 8: {
			uiMgr->Create_Text(CUI::UIKind::UIK_TalkText, L"Texture_Text_8duftlagl");
			break;
		}
		case 10: {
			uiMgr->Create_Text(CUI::UIKind::UIK_TalkText, L"Texture_Text_13dlwpak");
			break;
		}
		default:
		{
			uiMgr->Create_Text(CUI::UIKind::UIK_TalkText, L"Texture_Text_znptmxm");
			break;
		}
	}
	
	return;
}

void CNpc_01::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{
	if (CSphereCollider::BoneTeam_Player != _colSphere->m_BoneTeam)
		return;

	//if (_mySphere->m_BonePart == CSphereCollider::BonePart_CollBody) {

	//}

	if (_mySphere->m_BonePart == CSphereCollider::BonePart_NpcTalk) {
		if (_colSphere->m_BonePart == CSphereCollider::BonePart_CollBody) {
			CNpcQuest_Manager::Get_NpcQuestInfo()->m_NpcColl = true;
		}
	}
	
}
