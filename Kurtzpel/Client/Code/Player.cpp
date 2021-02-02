#include "stdafx.h"
#include "Player.h"
#include "Export_Function.h"
#include "DynamicCamera.h"
#include "SphereCollider.h"
#include "Hammer.h"
#include "LongBow.h"
#include "Stage.h"
#include "NaviTerrain.h"
#include "Arrow.h"
#include "SkillCollider.h"
#include "Arrow_SkillQ.h"
#include "Arrow_SkillF.h"
#include "NpcQuest_Manager.h"
#include "Phoenix.h"
#include "Npc_01.h"
#include "Portal.h"
#include "CameraScene_Manager.h"
#include "GH_RockIn.h"
#include "SoundManager.h"
#include "EffectMesh_GHSkillE.h"

#define MP_GH_Q 50.f
#define MP_GH_E 50.f
#define MP_GH_F 50.f
#define MP_LB_Q 50.f
#define MP_LB_E 50.f
#define MP_LB_F 50.f
#define CancelTime 1.5f
#define DashGauge 20.f
#define RollGauge 35.f
#define COLLDOWNPOWER 2.5f
#define ATTACK_JUMPACCEL 1.2f
#define ATTACK_INIT 600.f
CPlayer* CPlayer::m_pInstance = nullptr;
CPlayer::Weapon_Equip CPlayer::m_WeaponEquip = CPlayer::Weapon_Equip::Weapon_Hammer;

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit_D(pGraphicDev)
{
	//if (m_pInstance == nullptr)
		m_pInstance = this;
	for (int i = 0; i < bCheck::bCheck_End; i++)
	{
		m_bCheck[i] = false;
	}
	m_bCheck[bCheck::bCheck_WeaponChange] = CNpcQuest_Manager::Get_NpcQuestInfo()->m_WeaponChange;
	for (int i = 0; i < TimeCheck::TimeCheck_End; i++)
	{
		m_TimeCheck[i] = 0.f;
	}
	m_sStat.m_fInitSpeed = 11.5f;
	m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
	m_fJumpPower = 0.17f;
	m_sStat.m_fMaxHp = 1000.f;
	m_sStat.m_fHp = m_sStat.m_fMaxHp;
	m_sStat.m_fDelayHp = m_sStat.m_fMaxHp;
	m_sStat.m_fAttack = ATTACK_INIT;
	m_sStat.m_fMaxMp = 300.f;
	m_sStat.m_fMp = m_sStat.m_fMaxMp;
	m_CameraDist = 400.f;
	m_MaxDashGauge = 100.f;
	m_DashGauge = m_MaxDashGauge;
}

CPlayer::~CPlayer(void)
{

}

HRESULT Client::CPlayer::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	CUnit_D::Add_Component();
	// Mesh
	pComponent = m_sComponent.m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Player"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);
	m_sComponent.m_pMeshCom->Set_AniAngle(265.f);

	//
	m_sComponent.m_pTransformCom->Set_Pos(&Engine::_vec3{ 54.f, 0.f, 54.f });
	m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	Engine::CGameObject::Update_Object(1.f);

	//m_sComponent.m_pMeshCom->Play_Animation(1.f);
	//Engine::CRenderer::GetInstance()->Render_GameObject(m_pGraphicDev);
	//
	Load_ColliderFile(L"../Bin/Resource/Mesh/DynamicMesh/Save/Player.dat");
	for (auto& sphere : m_VecSphereCollider)
	{
		sphere->m_pDynamicThis = this;
		if (!sphere->m_FrameName.compare("Core")) {
			sphere->m_BonePart = CSphereCollider::BonePart_CollBody;
		}
		else if (!sphere->m_FrameName.compare("Weapon_Hand_R")) {
			sphere->m_BonePart = CSphereCollider::BonePart_PlayerHammer;
		}
		sphere->m_BoneTeam = CSphereCollider::BoneTeam_Player;
	}
	return S_OK;
}

void Client::CPlayer::Set_StateToAnimation(State _state, _vec3 _vPos, _vec3 _vDir, float fTimeDelta) {
	CNpcQuest_Manager::NpcQuestInfo* questInfo = CNpcQuest_Manager::Get_NpcQuestInfo();
	if (m_WeaponEquip == Weapon_Equip::Weapon_Hammer)
	{
		switch (_state)
		{
		case Client::CPlayer::State_Idle: {
			m_State = _state;
			m_sStat.m_fAttack = ATTACK_INIT;
			m_sComponent.m_pMeshCom->Set_AnimationSet(243);
			m_AniSpeed = 1.f;
			m_Attack_State = Attack_State::StateA_None;
			m_Hammer->Set_Pos();
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
			break;
		}
		case Client::CPlayer::State_Move: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(260);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_MoveSA: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(258);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_MoveSD: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(256);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_Dash: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(266);
			m_sStat.m_fSpeed = 2.4f * m_sStat.m_fInitSpeed;
			m_AniSpeed = 2.1f;
			m_DashGauge -= DashGauge;
			Engine::CRenderer::GetInstance()->Set_MotionBlur(1.f);
			break;
		}
		case Client::CPlayer::State_Rolling: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(246);
			m_sStat.m_fSpeed = 1.5f * m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.8f;
			m_DashGauge -= RollGauge;
			questInfo->m_RollingCount++;
			Voice_Random(RandomSound::Sound_AttackVoice);
			break;
		}
		case Client::CPlayer::State_Attack: {
			m_WeaponDissolve = 0.f;
			m_bCheck[bCheck::bCheck_WeaponChangeIdle] = true;
			questInfo->m_AttackCount++;
			m_Attack_State = (Attack_State)((int)m_Attack_State + 1);
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
			m_sComponent.m_pMeshCom->Set_AniAngle(265.f);
			//if (m_Attack_State == Attack_State::StateA_End)
			//	m_Attack_State = Attack_State::StateA_Basic1;
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.4f;
			Voice_Random(RandomSound::Sound_AttackVoice);
			switch (m_Attack_State)
			{
			case Client::CPlayer::StateA_Basic1:
				m_sComponent.m_pMeshCom->Set_AnimationSet(146);
				//SoundManager::PlayOverlapSound(L"AttackVoice01.ogg", SoundChannel::PLAYER, VOLUME);
				break;
			case Client::CPlayer::StateA_Basic2:
				m_sComponent.m_pMeshCom->Set_AnimationSet(144);
				break;
			case Client::CPlayer::StateA_Basic3:
				m_sComponent.m_pMeshCom->Set_AnimationSet(142);
				break;
			default:
				break;
			}
			if (m_State != State::State_Idle && m_Attack_State == CPlayer::StateA_Basic1)
				m_AccelSpeed = m_AccelSpeedInit = m_sStat.m_fSpeed;
			else
				m_AccelSpeed = m_AccelSpeedInit = 0.f;
			m_State = _state;
			m_Hammer->Set_Pos();
			
			break;
		}
		case Client::CPlayer::State_Skill: {
			m_WeaponDissolve = 0.f;
			m_bCheck[bCheck::bCheck_WeaponChangeIdle] = true;
			//Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
			m_sComponent.m_pMeshCom->Set_AniAngle(265.f);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			switch (m_Attack_State)
			{
			case Client::CPlayer::StateA_SkillQ:

				SoundManager::PlayOverlapSound(L"SkillVoice01.ogg", SoundChannel::PLAYER , 0.1f);

				//m_sStat.m_fAttack = ATTACK_INIT * 5.f;
				m_sComponent.m_pMeshCom->Set_AnimationSet(123);
				m_AniSpeed = 1.2f;
				m_AniClip = AnimationClip::Ani_1;
				//Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
				m_sStat.m_fMp -= MP_GH_Q;
				questInfo->m_SkillQCount++;
				Voice_Random(RandomSound::Sound_SkillVoice);
				break;
			case Client::CPlayer::StateA_SkillE: {
				//m_sStat.m_fAttack = ATTACK_INIT * 3.5f;
				m_sComponent.m_pMeshCom->Set_AnimationSet(134);
				m_AniSpeed = 1.2f;
				m_bCheck[bCheck::bCheck_Skill_F1] = false;
				m_sStat.m_fMp -= MP_GH_E;
				questInfo->m_SkillECount++;
				Voice_Random(RandomSound::Sound_SkillVoice);
				break;
			}
			case Client::CPlayer::StateA_SkillF: {
				m_sStat.m_fAttack = ATTACK_INIT * 1.5f;
				m_sComponent.m_pMeshCom->Set_AnimationSet(130);
				m_AniSpeed = 1.3f;
				m_bCheck[bCheck::bCheck_Skill_F1] = false;
				m_bCheck[bCheck::bCheck_Skill_F2] = false;
				m_bCheck[bCheck::bCheck_Skill_F3] = false;
				//Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
				m_sStat.m_fMp -= MP_GH_F;
				questInfo->m_SkillFCount++;
				//Voice_Random(Voice::Voice_Attack);
				break;
			}
			case Client::CPlayer::StateA_SkillZ: {
				m_sComponent.m_pMeshCom->Set_AnimationSet(13);
				m_AniSpeed = 1.1f;
				Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
				Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
				m_TimeCheck[TimeCheck::TimeCheck_Invin] = 10.f;
				CCameraScene_Manager::Get_Instance()->Set_CameraScene(2);
				Voice_Random(RandomSound::Sound_SkillVoice);
				break;
			}
			default:
				break;
			}
			if (m_State != State::State_Idle)
				m_AccelSpeed = m_AccelSpeedInit = m_sStat.m_fSpeed;
			else
				m_AccelSpeed = m_AccelSpeedInit = 0.f;
			m_State = _state;
			m_Hammer->Set_Pos();
			
			break;
		}
		case Client::CPlayer::State_JumpEnd: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(277);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_JumpComboEnd: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(155);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.8f;
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
			
			m_State = _state;
			//m_Hammer->Set_Pos();
			
			break;
		}
		case Client::CPlayer::State_Damaged: {
			if (m_sStat.m_fKnockBackPower < COLLDOWNPOWER) {
				m_sComponent.m_pMeshCom->Set_AnimationSet(160);
				m_AniSpeed = 1.5f;
			}
			else {  //강한어택 맞음
				m_sComponent.m_pMeshCom->Set_AnimationSet(183);
				m_bCheck[bCheck::bCheck_DamagedUp] = false;
				m_AniSpeed = 1.4f;
			}
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			Voice_Random(RandomSound::Sound_DamagedVoice);
			break;
		}
		case Client::CPlayer::State_End: {
			break;
		}
		default:
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////// LongBow Status
	else if (m_WeaponEquip == Weapon_Equip::Weapon_LongBow)
	{
		switch (_state)
		{
		case Client::CPlayer::State_Idle: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(243);
			m_AniSpeed = 1.f;
			m_LongBow->Set_Pos();
			m_Attack_State = Attack_State::StateA_None;
			//Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
			break;
		}
		case Client::CPlayer::State_Move: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(260);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_MoveSA: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(258);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_MoveSD: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(256);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_Dash: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(266);
			m_sStat.m_fSpeed = 2.4f * m_sStat.m_fInitSpeed;
			m_AniSpeed = 2.1f;
			m_DashGauge -= DashGauge;
			Engine::CRenderer::GetInstance()->Set_MotionBlur(1.f);
			break;
		}
		case Client::CPlayer::State_Rolling: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(246);
			m_sStat.m_fSpeed = 1.5f * m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.8f;
			m_DashGauge -= RollGauge;
			questInfo->m_RollingCount++;
			Voice_Random(RandomSound::Sound_AttackVoice);
			break;
		}
		case Client::CPlayer::State_Attack: {
			m_WeaponDissolve = 0.f;
			questInfo->m_AttackCount++;
			Create_ArrowShot(_vPos, _vDir, fTimeDelta);
			m_Attack_State = (Attack_State)((int)m_Attack_State + 1);
			//Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
			m_sComponent.m_pMeshCom->Set_AniAngle(265.f);
			//if (m_Attack_State == Attack_State::StateA_End)
			//	m_Attack_State = Attack_State::StateA_Basic1;
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 2.7f;
			Voice_Random(RandomSound::Sound_AttackVoice);
			switch (m_Attack_State)
			{
			case Client::CPlayer::StateA_Basic1:
				m_sComponent.m_pMeshCom->Set_AnimationSet(77);
				break;
			case Client::CPlayer::StateA_Basic2:
				m_sComponent.m_pMeshCom->Set_AnimationSet(74);
				break;
			case Client::CPlayer::StateA_Basic3:
				m_sComponent.m_pMeshCom->Set_AnimationSet(71);
				break;
			default:
				break;
			}
			if (m_State != State::State_Idle && m_Attack_State == CPlayer::StateA_Basic1)
				m_AccelSpeed = m_AccelSpeedInit = m_sStat.m_fSpeed;
			else
				m_AccelSpeed = m_AccelSpeedInit = 0.f;
			m_State = _state;
			m_LongBow->Set_Pos();
			
			break;
		}
		case Client::CPlayer::State_Skill: {
			m_WeaponDissolve = 0.f;
			//Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
			m_sComponent.m_pMeshCom->Set_AniAngle(265.f);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			switch (m_Attack_State)
			{
			case Client::CPlayer::StateA_SkillQ:
				m_sComponent.m_pMeshCom->Set_AnimationSet(56);
				m_AniSpeed = 3.f;
				m_AniClip = AnimationClip::Ani_1;
				m_sStat.m_fMp -= MP_LB_Q;
				questInfo->m_SkillQCount++;
				Voice_Random(RandomSound::Sound_SkillVoice);
				break;
			case Client::CPlayer::StateA_SkillE: {
				m_AniClip = AnimationClip::Ani_1;
				m_sComponent.m_pMeshCom->Set_AnimationSet(31);
				m_AniSpeed = 3.f;
				m_sStat.m_fMp -= MP_LB_E;
				questInfo->m_SkillECount++;
				break;
			}
			case Client::CPlayer::StateA_SkillF: {
				m_sComponent.m_pMeshCom->Set_AnimationSet(20);
				m_AniSpeed = 2.5f;
				m_AniClip = AnimationClip::Ani_1;
				m_sStat.m_fMp -= MP_LB_F;
				questInfo->m_SkillFCount++;
				Voice_Random(RandomSound::Sound_SkillVoice);
				break;
			}
			case Client::CPlayer::StateA_SkillZ: {
				m_sComponent.m_pMeshCom->Set_AnimationSet(6);
				m_AniSpeed = 2.f;
				m_bCheck[bCheck::bCheck_LB_Phoenix_SkillZ] = false;
				m_TimeCheck[TimeCheck::TimeCheck_Invin] = 10.f;
				m_AniClip = AnimationClip::Ani_1;
				CCameraScene_Manager::Get_Instance()->Set_CameraScene(3);
				break;
			}
			default:
				break;
			}
			if (m_State != State::State_Idle)
				m_AccelSpeed = m_AccelSpeedInit = m_sStat.m_fSpeed;
			else
				m_AccelSpeed = m_AccelSpeedInit = 0.f;
			m_State = _state;
			m_LongBow->Set_Pos();
			
			break;
		}
		case Client::CPlayer::State_JumpEnd: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(277);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_JumpComboEnd: {
			//m_sComponent.m_pMeshCom->Set_AnimationSet(155);
			//m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			//m_AniSpeed = 1.8f;
			//Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
			//m_State = _state;
			////m_LongBow->Set_Pos();
			//
			break;
		}
		case Client::CPlayer::State_Damaged: {
			if (m_sStat.m_fKnockBackPower < COLLDOWNPOWER) {
				m_sComponent.m_pMeshCom->Set_AnimationSet(160);
				m_AniSpeed = 1.5f;
			}
			else {  //강한어택 맞음
				m_sComponent.m_pMeshCom->Set_AnimationSet(183);
				m_bCheck[bCheck::bCheck_DamagedUp] = false;
				m_AniSpeed = 1.4f;
			}
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			Voice_Random(RandomSound::Sound_DamagedVoice);
			break;
		}
		case Client::CPlayer::State_End: {
			break;
		}
		default:
			break;
		}
	}
	m_State = _state;
}

void Client::CPlayer::Hammer_Key_Input(const _float& fTimeDelta)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	Engine::CNaviMesh* pNaviMeshCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_NaviTerrain->m_pNaviMeshCom;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_LOOK, &m_sStat.m_vDir);
	D3DXVec3Normalize(&m_sStat.m_vDir, &m_sStat.m_vDir);
	_vec3	vPos, vDir, vDir2;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	vDir = m_sStat.m_vDir;
	Key_DoubleInput(fTimeDelta);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// 피격 당할때
	if (m_State == State::State_Damaged) { ////////////////////////////////////////////////////////////////////////////// 피격
		if (m_bCheck[bCheck_JumpToDamagedUp] || m_sStat.m_fKnockBackPower > COLLDOWNPOWER) {		//강한 공격을 맞고 다운
			if (!m_bCheck[bCheck::bCheck_DamagedUp]) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.4f)) {
					m_bCheck[bCheck::bCheck_DamagedUp] = true;
					m_sComponent.m_pMeshCom->Set_AnimationSet(245);
				}
				else {
					if (m_sComponent.m_pMeshCom->Get_AnimationTrackPos() < 0.68f) {
						m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_sStat.m_fKnockBackDir * fTimeDelta * m_sStat.m_fSpeed * 1.5f * m_sStat.m_fKnockBackPower), &m_sStat.m_dwNaviIndex));
					}
					m_sStat.m_fSpeed -= m_sStat.m_fInitSpeed * fTimeDelta * 2.5f;
					return;
				}
			}
			else {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.9f)) {
					m_AniSpeed;
					Set_StateToAnimation(State::State_Idle);
					m_bCheck[bCheck_JumpToDamagedUp] = false;
				}
				return;
			}
		}
		else {											//약한 공격을 맞고 약간 넉백
			if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd()) {
				Set_StateToAnimation(State::State_Idle);
			}
			else {
				m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_sStat.m_fKnockBackDir * fTimeDelta * m_sStat.m_fSpeed * 1.5f * m_sStat.m_fKnockBackPower), &m_sStat.m_dwNaviIndex));
				m_sStat.m_fSpeed -= m_sStat.m_fInitSpeed * fTimeDelta * 2.5f;
				return;
			}
		}
	}
	else if (m_State == State::State_Skill) { /////////////////////////////////////////////////////////////////////////// Skill

		Event_Skill(fTimeDelta, pNaviMeshCom, vPos, vDir);

	}
	else if (m_State == State::State_Attack) { ////////////////////////////////////////////////////////////////////////// Attack
		if (m_bCheck[bCheck::bCheck_MouseL_Already]) {
			if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.85f)) {
				Set_StateToAnimation(State::State_Attack);
				m_bCheck[bCheck::bCheck_MouseL_Already] = false;
				Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
			}
		}
		else if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.35f)) {
			Set_StateToAnimation(State::State_Idle);
			m_Attack_State = Attack_State::StateA_None;
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
		}
		else if((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) && (m_Attack_State == Attack_State::StateA_Basic1 || m_Attack_State == Attack_State::StateA_Basic2)){
			float aniPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();

			if (aniPos > 0.35f) {
				m_bCheck[bCheck::bCheck_MouseL_Already] = true;
			}
		}
		//공격모션 동안 움직임
		float trackPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();
		if (trackPos < 0.3f && trackPos > 0.1f) {
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
		}
		if (trackPos > 0.1f && trackPos < 0.7f) {
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
		}
		else {
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
		}
		Move_AccelSpeed(vPos, vDir, fTimeDelta, pNaviMeshCom);
	}
	else if (m_State == State::State_JumpComboEnd) { ////////////////////////////////////////////////////////////////////// JumpComboEnd
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.05f)) {
			Set_StateToAnimation(State::State_Idle);
		}
		Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
		return;
	}
	else if (m_State == State::State_Dash || m_State == State::State_Rolling) { ///////////////////////////////////////////////////////////////////////////// Dash
		if ((m_State == State::State_Dash && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.7f))
			|| (m_State == State::State_Rolling && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.1f)))//m_TimeCheck[TimeCheck::TimeCheck_Dash] < 0.f)
		{
			Set_StateToAnimation(State::State_Idle);
			m_bCheck[bCheck::bCheck_KeyW] = false;
			m_bCheck[bCheck::bCheck_KeyA] = false;
			m_bCheck[bCheck::bCheck_KeyS] = false;
			m_bCheck[bCheck::bCheck_KeyD] = false;
			m_bCheck[bCheck::bCheck_DBKeyW] = false;
			m_bCheck[bCheck::bCheck_DBKeyA] = false;
			m_bCheck[bCheck::bCheck_DBKeyS] = false;
			m_bCheck[bCheck::bCheck_DBKeyD] = false;
			return;
		}
		else if(m_State == State::State_Rolling && m_sComponent.m_pMeshCom->Get_AnimationTrackPos() > 1.2f){

		}
		else if (m_sStat.m_EnumDir == EnumDir::Up) {
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(265.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::UpLeft) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(220.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::UpRight) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(310.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::Down) {
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(85.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::DownLeft) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(130.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::DownRight) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(40.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::Left) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(175.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::Right) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(355.f);
		}

	}
	//////////////////////////////////////////////////////////////////// 실제 키 입력부분
	else if ((Engine::Get_DIKeyState(DIK_F) & 0x80) && m_TimeCheck[TimeCheck_Cool_F] <= 0.f && m_sStat.m_fMp > MP_GH_F) {
		m_TimeCheck[TimeCheck_Cool_F] = COOLTIME_GH_F;
		m_Attack_State = Attack_State::StateA_SkillF;
		Set_StateToAnimation(State::State_Skill);
	}
	else if ((Engine::Get_DIKeyState(DIK_E) & 0x80) && m_TimeCheck[TimeCheck_Cool_E] <= 0.f && m_sStat.m_fMp > MP_GH_E) {
		m_TimeCheck[TimeCheck_Cool_E] = COOLTIME_GH_E;
		m_Attack_State = Attack_State::StateA_SkillE;
		Set_StateToAnimation(State::State_Skill);
	}
	else if ((Engine::Get_DIKeyState(DIK_Q) & 0x80) && m_TimeCheck[TimeCheck_Cool_Q] <= 0.f && m_sStat.m_fMp > MP_GH_Q) {
		m_TimeCheck[TimeCheck_Cool_Q] = COOLTIME_GH_Q;
		m_Attack_State = Attack_State::StateA_SkillQ;
		Set_StateToAnimation(State::State_Skill);
	}
	else if ((Engine::Get_DIKeyState(DIK_Z) & 0x80)) {
		m_Attack_State = Attack_State::StateA_SkillZ;
		Set_StateToAnimation(State::State_Skill);
	}
	else if ((Engine::Get_DIKeyState(DIK_LSHIFT) & 0x80) && m_DashGauge > RollGauge && m_TimeCheck[TimeCheck_Cool_LSHIFT] <= 0.f) {
		Set_StateToAnimation(State::State_Rolling);
		m_TimeCheck[TimeCheck_Cool_LSHIFT] = COOLTIME_LSHIFT;
	}
	//////////////////////////////////////////////////////////////////// 좌클릭공격
	else if ((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) && !m_bCheck[bCheck::bCheck_MouseL])
	{
		m_bCheck[bCheck::bCheck_MouseL] = true;
		Set_StateToAnimation(State::State_Attack, vPos, vDir, fTimeDelta);
		m_bCheck[bCheck::bCheck_MouseL_Already] = false;

	}
	//////////////////////////////////////////////////////////////////// 무기 체인지
	else if (m_bCheck[bCheck::bCheck_WeaponChange] && (Engine::Get_DIKeyState(DIK_TAB) & 0x80) && m_TimeCheck[TimeCheck_Cool_Tab] <= 0.f) {
		m_bCheck[CPlayer::bCheck_State_WeaponChange] = true;
		m_WeaponDissolve = 0.f;
		m_TimeCheck[TimeCheck_Cool_Tab] = COOLTIME_TAB;
	}
	//////////////////////////////////////////////////////////////////// 이동
	else
	{
		Move_WASD(fTimeDelta, vPos, vDir, vDir2, pNaviMeshCom);
	}

	if ((m_State == State::State_Move || m_State == State::State_MoveSA || m_State == State::State_MoveSD)
		&& !(Engine::Get_DIKeyState(DIK_W) & 0x80) && !(Engine::Get_DIKeyState(DIK_S) & 0x80) && !(Engine::Get_DIKeyState(DIK_A) & 0x80) && !(Engine::Get_DIKeyState(DIK_D) & 0x80)) {
		m_State = State::State_Idle;
	}

	if ((m_State == State::State_Idle || m_State == State::State_Move || m_State == State::State_MoveSA || m_State == State::State_MoveSD)
		&& Engine::Get_DIKeyState(DIK_SPACE) & 0x80) {
		m_JumpIdleState = JumpIdleAni::JumpIdle_JumpStart;
	}
	// Jump 가 끝났을때 모션
	if (m_State == State::State_JumpEnd) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.3f)) {
			Set_StateToAnimation(State::State_Idle);
		}
		return;
	}
	// Idle 모션
	else if (m_State == State::State_Idle) {
		Set_StateToAnimation(State::State_Idle);
	}


}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// LongBow Pattern
void Client::CPlayer::LongBow_Key_Input(const _float& fTimeDelta)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	Engine::CNaviMesh* pNaviMeshCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_NaviTerrain->m_pNaviMeshCom;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_LOOK, &m_sStat.m_vDir);
	D3DXVec3Normalize(&m_sStat.m_vDir, &m_sStat.m_vDir);
	_vec3	vPos, vDir, vDir2;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	vDir = m_sStat.m_vDir;
	Key_DoubleInput(fTimeDelta);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 피격 당할때
	if (m_State == State::State_Damaged) { ////////////////////////////////////////////////////////////////////////////// 피격
		if (m_bCheck[bCheck_JumpToDamagedUp] || m_sStat.m_fKnockBackPower > COLLDOWNPOWER) {		//강한 공격을 맞고 다운
			if (!m_bCheck[bCheck::bCheck_DamagedUp]) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.4f)) {
					m_bCheck[bCheck::bCheck_DamagedUp] = true;
					m_sComponent.m_pMeshCom->Set_AnimationSet(245);
				}
				else {
					if (m_sComponent.m_pMeshCom->Get_AnimationTrackPos() < 0.68f) {
						m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_sStat.m_fKnockBackDir * fTimeDelta * m_sStat.m_fSpeed * 1.5f * m_sStat.m_fKnockBackPower), &m_sStat.m_dwNaviIndex));
					}
					m_sStat.m_fSpeed -= m_sStat.m_fInitSpeed * fTimeDelta * 2.5f;
					return;
				}
			}
			else {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.9f)) {
					m_AniSpeed;
					Set_StateToAnimation(State::State_Idle);
					m_bCheck[bCheck_JumpToDamagedUp] = false;
				}
				return;
			}
		}
		else {											//약한 공격을 맞고 약간 넉백
			if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd()) {
				Set_StateToAnimation(State::State_Idle);
			}
			else {
				m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_sStat.m_fKnockBackDir * fTimeDelta * m_sStat.m_fSpeed * 1.5f * m_sStat.m_fKnockBackPower), &m_sStat.m_dwNaviIndex));
				m_sStat.m_fSpeed -= m_sStat.m_fInitSpeed * fTimeDelta * 2.5f;
				return;
			}
		}
	}
	else if (m_State == State::State_Skill) { /////////////////////////////////////////////////////////////////////////// Skill

		Event_Skill(fTimeDelta, pNaviMeshCom, vPos, vDir);

	}
	else if (m_State == State::State_Attack) { ////////////////////////////////////////////////////////////////////////// Attack
		if (m_bCheck[bCheck::bCheck_MouseL_Already]) {
			if (m_Attack_State == Attack_State::StateA_Basic1) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.06f)) {
					Set_StateToAnimation(State::State_Attack, vPos, vDir, fTimeDelta);
						m_bCheck[bCheck::bCheck_MouseL_Already] = false;
				}
			}
			else if (m_Attack_State == Attack_State::StateA_Basic2) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.6f)) {
					Set_StateToAnimation(State::State_Attack, vPos, vDir, fTimeDelta);
					m_bCheck[bCheck::bCheck_MouseL_Already] = false;
				}
			}
		}
		else if (m_Attack_State == Attack_State::StateA_Basic1 && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.8f)) {
			Set_StateToAnimation(State::State_Idle);
			m_Attack_State = Attack_State::StateA_None;
		}
		else if (m_Attack_State == Attack_State::StateA_Basic2 && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.3f)) {
			Set_StateToAnimation(State::State_Idle);
			m_Attack_State = Attack_State::StateA_None;
		}
		else if (m_Attack_State == Attack_State::StateA_Basic3 && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.8f)) {
			Set_StateToAnimation(State::State_Idle);
			m_Attack_State = Attack_State::StateA_None;
		}
		else if ((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) && (m_Attack_State == Attack_State::StateA_Basic1 || m_Attack_State == Attack_State::StateA_Basic2)) {
			float aniPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();

			if (aniPos > 0.5f) {
				m_bCheck[bCheck::bCheck_MouseL_Already] = true;
			}
		}

		Move_AccelSpeed(vPos, vDir, fTimeDelta, pNaviMeshCom);
	}
	else if (m_State == State::State_JumpComboEnd) { ////////////////////////////////////////////////////////////////////// JumpComboEnd
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.05f)) {
			Set_StateToAnimation(State::State_Idle);
		}
		Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
		return;
	}
	else if (m_State == State::State_Dash || m_State == State::State_Rolling) { ///////////////////////////////////////////////////////////////////////////// Dash
		if ((m_State == State::State_Dash && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.7f))
			|| (m_State == State::State_Rolling && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.1f)))//m_TimeCheck[TimeCheck::TimeCheck_Dash] < 0.f)
		{
			Set_StateToAnimation(State::State_Idle);
			m_bCheck[bCheck::bCheck_KeyW] = false;
			m_bCheck[bCheck::bCheck_KeyA] = false;
			m_bCheck[bCheck::bCheck_KeyS] = false;
			m_bCheck[bCheck::bCheck_KeyD] = false;
			m_bCheck[bCheck::bCheck_DBKeyW] = false;
			m_bCheck[bCheck::bCheck_DBKeyA] = false;
			m_bCheck[bCheck::bCheck_DBKeyS] = false;
			m_bCheck[bCheck::bCheck_DBKeyD] = false;
			return;
		}
		else if (m_State == State::State_Rolling && m_sComponent.m_pMeshCom->Get_AnimationTrackPos() > 1.2f) {

		}
		else if (m_sStat.m_EnumDir == EnumDir::Up) {
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(265.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::UpLeft) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(220.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::UpRight) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(310.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::Down) {
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(85.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::DownLeft) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(130.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::DownRight) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(40.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::Left) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(175.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::Right) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(355.f);
		}

	}
	//////////////////////////////////////////////////////////////////// 실제 키 입력부분
	else if (!m_bCheck[bCheck_LB_SkillE] && (Engine::Get_DIKeyState(DIK_F) & 0x80) && m_TimeCheck[TimeCheck_Cool_F] <= 0.f && m_sStat.m_fMp > MP_LB_F) {
		m_TimeCheck[TimeCheck_Cool_F] = COOLTIME_LB_F;
		m_Attack_State = Attack_State::StateA_SkillF;
		Set_StateToAnimation(State::State_Skill);
	}
	else if (!m_bCheck[bCheck_LB_SkillE] && (Engine::Get_DIKeyState(DIK_E) & 0x80) && m_TimeCheck[TimeCheck_Cool_E] <= 0.f && m_sStat.m_fMp > MP_LB_E) {
		m_TimeCheck[TimeCheck_Cool_E] = COOLTIME_LB_E;
		m_bCheck[bCheck_LB_SkillE] = true;
		//m_Attack_State = Attack_State::StateA_SkillE;
		//Set_StateToAnimation(State::State_Skill);
	}
	else if (m_bCheck[bCheck_LB_SkillE] && (Engine::Get_DIKeyState(DIK_E) & 0x80) && m_TimeCheck[TimeCheck_Cool_E] <= COOLTIME_LB_E - CancelTime) {
		m_bCheck[bCheck_LB_SkillE] = false;
		//m_sStat.m_fMp += MP_LB_E * 0.5f;
		if (m_sStat.m_fMp > m_sStat.m_fMaxMp)
			m_sStat.m_fMp = m_sStat.m_fMaxMp;
	}
	else if (!m_bCheck[bCheck_LB_SkillE] && (Engine::Get_DIKeyState(DIK_Q) & 0x80) && m_TimeCheck[TimeCheck_Cool_Q] <= 0.f && m_sStat.m_fMp > MP_LB_Q) {
		m_TimeCheck[TimeCheck_Cool_Q] = COOLTIME_LB_Q;
		m_Attack_State = Attack_State::StateA_SkillQ;
		Set_StateToAnimation(State::State_Skill);
	}
	else if (!m_bCheck[bCheck_LB_SkillE] && (Engine::Get_DIKeyState(DIK_Z) & 0x80)) {
		m_Attack_State = Attack_State::StateA_SkillZ;
		Set_StateToAnimation(State::State_Skill);
	}
	else if ((Engine::Get_DIKeyState(DIK_LSHIFT) & 0x80) && m_DashGauge > RollGauge) {
		Set_StateToAnimation(State::State_Rolling);
	}
	//////////////////////////////////////////////////////////////////// 좌클릭공격
	else if ((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) && !m_bCheck[bCheck::bCheck_MouseL])
	{
		if (m_bCheck[bCheck_LB_SkillE]) {
			m_bCheck[bCheck_LB_SkillE] = false;
			m_TimeCheck[TimeCheck::TimeCheck_LB_SkillE_Ready] = 4.f;
			m_Attack_State = Attack_State::StateA_SkillE;
			Set_StateToAnimation(State::State_Skill);
		}
		else {
			Set_StateToAnimation(State::State_Attack, vPos, vDir, fTimeDelta);
		}
		m_bCheck[bCheck::bCheck_MouseL] = true;
		m_bCheck[bCheck::bCheck_MouseL_Already] = false;

	}
	//////////////////////////////////////////////////////////////////// 무기 체인지
	else if ((Engine::Get_DIKeyState(DIK_TAB) & 0x80) && m_TimeCheck[TimeCheck_Cool_Tab] <= 0.f) {
		m_bCheck[CPlayer::bCheck_State_WeaponChange] = true;
		m_WeaponDissolve = 0.f;
		m_TimeCheck[TimeCheck_Cool_Tab] = COOLTIME_TAB;

	}
	//////////////////////////////////////////////////////////////////// 이동
	else 
	{
		Move_WASD(fTimeDelta, vPos, vDir, vDir2, pNaviMeshCom);
	}

	if (m_bCheck[bCheck_LB_SkillE]) {
		m_LB_SkillE_Pos = vPos + (vDir * (13.5f + m_LookAtY * 4.5f));
	}
	if (m_TimeCheck[TimeCheck::TimeCheck_LB_SkillE_Attack_Arrow_Start_Total] > 0.f && m_TimeCheck[TimeCheck::TimeCheck_LB_SkillE_Attack_Arrow_Start_Total] < 2.f) {
		if (m_TimeCheck[TimeCheck::TimeCheck_LB_SkillE_Attack_Arrow_Start] <= 0.f) {
			m_TimeCheck[TimeCheck::TimeCheck_LB_SkillE_Attack_Arrow_Start] = 0.05f;
			Create_ArrowShot_SkillE_Start();
		}
	}

	if ((m_State == State::State_Move || m_State == State::State_MoveSA || m_State == State::State_MoveSD)
		&& !(Engine::Get_DIKeyState(DIK_W) & 0x80) && !(Engine::Get_DIKeyState(DIK_S) & 0x80) && !(Engine::Get_DIKeyState(DIK_A) & 0x80) && !(Engine::Get_DIKeyState(DIK_D) & 0x80)) {
		m_State = State::State_Idle;
	}

	if ((m_State == State::State_Idle || m_State == State::State_Move || m_State == State::State_MoveSA || m_State == State::State_MoveSD)
		&& Engine::Get_DIKeyState(DIK_SPACE) & 0x80) {
		m_JumpIdleState = JumpIdleAni::JumpIdle_JumpStart;
	}
	// Jump 가 끝났을때 모션
	if (m_State == State::State_JumpEnd) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.3f)) {
			Set_StateToAnimation(State::State_Idle);
		}
		return;
	}
	// Idle 모션
	else if (m_State == State::State_Idle) {
		Set_StateToAnimation(State::State_Idle);
	}


}

void Client::CPlayer::WeaponChangeState_Key_Input(const _float& fTimeDelta)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	Engine::CNaviMesh* pNaviMeshCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_NaviTerrain->m_pNaviMeshCom;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_LOOK, &m_sStat.m_vDir);
	D3DXVec3Normalize(&m_sStat.m_vDir, &m_sStat.m_vDir);
	_vec3	vPos, vDir, vDir2;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	vDir = m_sStat.m_vDir;
	Key_DoubleInput(fTimeDelta);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 피격 당할때
	if (m_State == State::State_Damaged) { ////////////////////////////////////////////////////////////////////////////// 피격
		if (m_bCheck[bCheck_JumpToDamagedUp] || m_sStat.m_fKnockBackPower > COLLDOWNPOWER) {		//강한 공격을 맞고 다운
			if (!m_bCheck[bCheck::bCheck_DamagedUp]) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.4f)) {
					m_bCheck[bCheck::bCheck_DamagedUp] = true;
					m_sComponent.m_pMeshCom->Set_AnimationSet(245);
				}
				else {
					if (m_sComponent.m_pMeshCom->Get_AnimationTrackPos() < 0.68f) {
						m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_sStat.m_fKnockBackDir * fTimeDelta * m_sStat.m_fSpeed * 1.5f * m_sStat.m_fKnockBackPower), &m_sStat.m_dwNaviIndex));
					}
					m_sStat.m_fSpeed -= m_sStat.m_fInitSpeed * fTimeDelta * 2.5f;
					return;
				}
			}
			else {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.9f)) {
					m_AniSpeed;
					Set_StateToAnimation(State::State_Idle);
					m_bCheck[bCheck_JumpToDamagedUp] = false;
				}
				return;
			}
		}
		else {											//약한 공격을 맞고 약간 넉백
			if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd()) {
				Set_StateToAnimation(State::State_Idle);
			}
			else {
				m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_sStat.m_fKnockBackDir * fTimeDelta * m_sStat.m_fSpeed * 1.5f * m_sStat.m_fKnockBackPower), &m_sStat.m_dwNaviIndex));
				m_sStat.m_fSpeed -= m_sStat.m_fInitSpeed * fTimeDelta * 2.5f;
				return;
			}
		}
	}
	else if (m_State == State::State_Dash || m_State == State::State_Rolling) { ///////////////////////////////////////////////////////////////////////////// Dash
		if ((m_State == State::State_Dash && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.7f))
			|| (m_State == State::State_Rolling && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.1f)))//m_TimeCheck[TimeCheck::TimeCheck_Dash] < 0.f)
		{
			Set_StateToAnimation(State::State_Idle);
			m_bCheck[bCheck::bCheck_KeyW] = false;
			m_bCheck[bCheck::bCheck_KeyA] = false;
			m_bCheck[bCheck::bCheck_KeyS] = false;
			m_bCheck[bCheck::bCheck_KeyD] = false;
			m_bCheck[bCheck::bCheck_DBKeyW] = false;
			m_bCheck[bCheck::bCheck_DBKeyA] = false;
			m_bCheck[bCheck::bCheck_DBKeyS] = false;
			m_bCheck[bCheck::bCheck_DBKeyD] = false;
			return;
		}
		else if (m_State == State::State_Rolling && m_sComponent.m_pMeshCom->Get_AnimationTrackPos() > 1.2f) {

		}
		else if (m_sStat.m_EnumDir == EnumDir::Up) {
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(265.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::UpLeft) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(220.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::UpRight) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(310.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::Down) {
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(85.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::DownLeft) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(130.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::DownRight) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(40.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::Left) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(175.f);
		}
		else if (m_sStat.m_EnumDir == EnumDir::Right) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			m_sComponent.m_pMeshCom->Set_AniAngle(355.f);
		}

	}
	//////////////////////////////////////////////////////////////////// 실제 키 입력부분
	else if ((Engine::Get_DIKeyState(DIK_LSHIFT) & 0x80) && m_DashGauge > RollGauge && m_TimeCheck[TimeCheck_Cool_LSHIFT] <= 0.f) {
		Set_StateToAnimation(State::State_Rolling);
		m_TimeCheck[TimeCheck_Cool_LSHIFT] = COOLTIME_LSHIFT;
	}
	//////////////////////////////////////////////////////////////////// 좌클릭공격
	else if ((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) && !m_bCheck[bCheck::bCheck_MouseL])
	{
		m_bCheck[bCheck::bCheck_MouseL] = true;
		m_bCheck[bCheck::bCheck_MouseL_Already] = false;

	}
	//////////////////////////////////////////////////////////////////// 이동
	else
	{
		Move_WASD(fTimeDelta, vPos, vDir, vDir2, pNaviMeshCom);
	}

	if ((m_State == State::State_Move || m_State == State::State_MoveSA || m_State == State::State_MoveSD)
		&& !(Engine::Get_DIKeyState(DIK_W) & 0x80) && !(Engine::Get_DIKeyState(DIK_S) & 0x80) && !(Engine::Get_DIKeyState(DIK_A) & 0x80) && !(Engine::Get_DIKeyState(DIK_D) & 0x80)) {
		m_State = State::State_Idle;
	}

	/*if ((m_State == State::State_Idle || m_State == State::State_Move || m_State == State::State_MoveSA || m_State == State::State_MoveSD)
		&& Engine::Get_DIKeyState(DIK_SPACE) & 0x80) {
		m_JumpIdleState = JumpIdleAni::JumpIdle_JumpStart;
	}*/
	// Jump 가 끝났을때 모션
	if (m_State == State::State_JumpEnd) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.3f)) {
			Set_StateToAnimation(State::State_Idle);
		}
		return;
	}
	// Idle 모션
	else if (m_State == State::State_Idle) {
		Set_StateToAnimation(State::State_Idle);
	}
	if (!m_bCheck[bCheck::bCheck_State_WeaponChangeHalf]) {
		m_WeaponDissolve += fTimeDelta * 1.5f;
		if (m_WeaponDissolve >= 1.f) {
			m_bCheck[bCheck::bCheck_State_WeaponChangeHalf] = true;
			Change_Weapon();
		}
	}
	else {
		m_WeaponDissolve -= fTimeDelta * 1.5f;
		if (m_WeaponDissolve <= 0.f) {
			m_bCheck[bCheck::bCheck_State_WeaponChangeHalf] = false;
			m_bCheck[bCheck::bCheck_State_WeaponChange] = false;
		}
	}
	
}

void Client::CPlayer::Move_WASD(_float fTimeDelta, _vec3& vPos, _vec3& vDir, _vec3& vDir2, Engine::CNaviMesh* pNaviMeshCom) {
	if (Engine::Get_DIKeyState(DIK_W) & 0x80)
	{
		if (m_bCheck[bCheck::bCheck_DBKeyW]) {
			if (m_DashGauge < DashGauge) {
				m_bCheck[bCheck::bCheck_DBKeyW] = false;
				m_bCheck[bCheck::bCheck_DBKeyA] = false;
				m_bCheck[bCheck::bCheck_DBKeyD] = false;
			}
			else {
				if (m_bCheck[bCheck::bCheck_DBKeyA]) {
					m_sStat.m_EnumDir = EnumDir::UpLeft;
				}
				else if (m_bCheck[bCheck::bCheck_DBKeyD]) {
					m_sStat.m_EnumDir = EnumDir::UpRight;
				}
				else {
					m_sStat.m_EnumDir = EnumDir::Up;
				}
				Set_StateToAnimation(State::State_Dash);
				m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
			}
		}
		else {
			if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
				vDir2 = { -vDir.z, 0.f, vDir.x };
				vDir2 += vDir;
				D3DXVec3Normalize(&vDir2, &vDir2);
				m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

				m_sComponent.m_pMeshCom->Set_AniAngle(220.f);
				m_sStat.m_EnumDir = EnumDir::UpLeft;
				Set_StateToAnimation(State::State_Move);
			}
			else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
				vDir2 = { vDir.z, 0.f, -vDir.x };
				vDir2 += vDir;
				D3DXVec3Normalize(&vDir2, &vDir2);
				m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

				m_sComponent.m_pMeshCom->Set_AniAngle(310.f);
				m_sStat.m_EnumDir = EnumDir::UpRight;
				Set_StateToAnimation(State::State_Move);
			}
			else {
				m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

				m_sComponent.m_pMeshCom->Set_AniAngle(265.f);
				m_sStat.m_EnumDir = EnumDir::Up;
				Set_StateToAnimation(State::State_Move);
			}
		}
	}
	else if (Engine::Get_DIKeyState(DIK_S) & 0x80)
	{
		if (m_bCheck[bCheck::bCheck_DBKeyS]) {
			if (m_DashGauge < DashGauge) {
				m_bCheck[bCheck::bCheck_DBKeyS] = false;
				m_bCheck[bCheck::bCheck_DBKeyA] = false;
				m_bCheck[bCheck::bCheck_DBKeyD] = false;
			}
			else {
				if (m_bCheck[bCheck::bCheck_DBKeyA]) {
					m_sStat.m_EnumDir = EnumDir::DownLeft;
				}
				else if (m_bCheck[bCheck::bCheck_DBKeyD]) {
					m_sStat.m_EnumDir = EnumDir::DownRight;
				}
				else {
					m_sStat.m_EnumDir = EnumDir::Down;
				}
				Set_StateToAnimation(State::State_Dash);
				m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
			}
		}
		else if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(175.f);
			m_sStat.m_EnumDir = EnumDir::DownLeft;
			Set_StateToAnimation(State::State_MoveSA);
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(355.f);
			m_sStat.m_EnumDir = EnumDir::DownRight;
			Set_StateToAnimation(State::State_MoveSD);
		}
		else {
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(85.f);
			m_sStat.m_EnumDir = EnumDir::Down;
			Set_StateToAnimation(State::State_Move);
		}
	}
	else if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
		m_State = State::State_Move;
		if (m_bCheck[bCheck::bCheck_DBKeyA]) {
			if (m_DashGauge < DashGauge) {
				m_bCheck[bCheck::bCheck_DBKeyA] = false;
				m_bCheck[bCheck::bCheck_DBKeyW] = false;
				m_bCheck[bCheck::bCheck_DBKeyS] = false;
			}
			else {
				if (m_bCheck[bCheck::bCheck_DBKeyW]) {
					m_sStat.m_EnumDir = EnumDir::UpLeft;
				}
				else if (m_bCheck[bCheck::bCheck_DBKeyS]) {
					m_sStat.m_EnumDir = EnumDir::DownLeft;
				}
				else {
					m_sStat.m_EnumDir = EnumDir::Left;
				}
				Set_StateToAnimation(State::State_Dash);
				m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
			}
		}
		else {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(175.f);
			m_sStat.m_EnumDir = EnumDir::Left;
			Set_StateToAnimation(State::State_Move);
		}
	}
	else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
		m_State = State::State_Move;
		if (m_bCheck[bCheck::bCheck_DBKeyD]) {
			if (m_DashGauge < DashGauge) {
				m_bCheck[bCheck::bCheck_DBKeyD] = false;
				m_bCheck[bCheck::bCheck_DBKeyW] = false;
				m_bCheck[bCheck::bCheck_DBKeyS] = false;
			}
			else {
				if (m_bCheck[bCheck::bCheck_DBKeyW]) {
					m_sStat.m_EnumDir = EnumDir::UpRight;
				}
				else if (m_bCheck[bCheck::bCheck_DBKeyS]) {
					m_sStat.m_EnumDir = EnumDir::DownRight;
				}
				else {
					m_sStat.m_EnumDir = EnumDir::Right;
				}
				Set_StateToAnimation(State::State_Dash);
				m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
			}
		}
		else {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(355.f);
			m_sStat.m_EnumDir = EnumDir::Right;
			Set_StateToAnimation(State::State_Move);
		}
	}
}

void Client::CPlayer::Set_StateToAnimation_Jump(State _state, _vec3 _vPos, _vec3 _vDir, float fTimeDelta) {
	if (m_WeaponEquip == Weapon_Equip::Weapon_Hammer) {
		switch (_state)
		{
		case Client::CPlayer::State_Idle: {
			m_AniSpeed = 1.f;
			if (m_WeaponEquip == Weapon_Equip::Weapon_Hammer)
				m_Hammer->Set_Pos();
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
			m_Attack_State = Attack_State::StateA_None;
			switch (m_JumpIdleState)
			{
			case JumpIdleAni::JumpIdle_JumpStart: {
				//m_sComponent.m_pMeshCom->Set_AnimationSet(273);
				m_JumpIdleState = JumpIdleAni::JumpIdle_JumpUp;
				m_AniSpeed = 1.f;
				m_sComponent.m_pMeshCom->Set_AnimationSet(275);
				break;
			}
			case JumpIdleAni::JumpIdle_JumpUp: {
				m_AniSpeed = 1.f;
				m_sComponent.m_pMeshCom->Set_AnimationSet(275);
				break;
			}
			case JumpIdleAni::JumpIdle_JumpDown: {
				m_AniSpeed = 1.f;
				m_sComponent.m_pMeshCom->Set_AnimationSet(279);
				break;
			}
			}
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			break;
		}
		case Client::CPlayer::State_Dash: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(266);
			m_sStat.m_fSpeed = 3.5f * m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.8f;
			break;
		}
		case Client::CPlayer::State_Attack: {
			m_Attack_State = (Attack_State)((int)m_Attack_State + 1);
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
			m_sComponent.m_pMeshCom->Set_AniAngle(265.f);
			//if (m_Attack_State == Attack_State::StateA_End)
			//	m_Attack_State = Attack_State::StateA_Basic1;
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.1f;
			m_fJumpAccel = ATTACK_JUMPACCEL;
			Voice_Random(RandomSound::Sound_AttackVoice);
			switch (m_Attack_State)
			{
			case Client::CPlayer::StateA_Basic1:
				m_sComponent.m_pMeshCom->Set_AnimationSet(154);
				break;
			case Client::CPlayer::StateA_Basic2:
				m_sComponent.m_pMeshCom->Set_AnimationSet(153);
				break;
			case Client::CPlayer::StateA_Basic3:
				m_sComponent.m_pMeshCom->Set_AnimationSet(152);
				break;
			case Client::CPlayer::StateA_Basic4:
				m_sComponent.m_pMeshCom->Set_AnimationSet(150);
				Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
				break;
			default:
				break;
			}
			if (m_WeaponEquip == Weapon_Equip::Weapon_Hammer) {
				m_State = _state;
				m_Hammer->Set_Pos();
			}
			break;
		}
		case Client::CPlayer::State_JumpEnd: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(277);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_Damaged: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(183);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.4f;
			if (m_fJumpAccel < 1.42f) {
				m_fJumpAccel = 1.42f;
			}
			m_bCheck[bCheck::bCheck_DamagedUp] = false;
			m_bCheck[bCheck_JumpToDamagedUp] = true;
			Voice_Random(RandomSound::Sound_DamagedVoice);
			break;
		}
		case Client::CPlayer::State_End: {
			break;
		}
		default:
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////// LongBow
	else if (m_WeaponEquip == Weapon_Equip::Weapon_LongBow) {
		switch (_state)
		{
		case Client::CPlayer::State_Idle: {
			m_AniSpeed = 1.f;
			m_LongBow->Set_Pos();
			m_Attack_State = Attack_State::StateA_None;
			switch (m_JumpIdleState)
			{
			case JumpIdleAni::JumpIdle_JumpStart: {
				//m_sComponent.m_pMeshCom->Set_AnimationSet(273);
				m_JumpIdleState = JumpIdleAni::JumpIdle_JumpUp;
				m_AniSpeed = 1.f;
				m_sComponent.m_pMeshCom->Set_AnimationSet(275);
				break;
			}
			case JumpIdleAni::JumpIdle_JumpUp: {
				m_AniSpeed = 1.f;
				m_sComponent.m_pMeshCom->Set_AnimationSet(275);
				break;
			}
			case JumpIdleAni::JumpIdle_JumpDown: {
				m_AniSpeed = 1.f;
				m_sComponent.m_pMeshCom->Set_AnimationSet(279);
				break;
			}
			}
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			break;
		}
		case Client::CPlayer::State_Dash: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(266);
			m_sStat.m_fSpeed = 3.5f * m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.8f;
			break;
		}
		case Client::CPlayer::State_Attack: {
			Create_ArrowShot(_vPos, _vDir, fTimeDelta);
			m_Attack_State = (Attack_State)((int)m_Attack_State + 1);
			m_sComponent.m_pMeshCom->Set_AniAngle(265.f);
			//if (m_Attack_State == Attack_State::StateA_End)
			//	m_Attack_State = Attack_State::StateA_Basic1;
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 3.f;
			m_fJumpAccel = ATTACK_JUMPACCEL;
			Voice_Random(RandomSound::Sound_AttackVoice);
			switch (m_Attack_State)
			{
			case Client::CPlayer::StateA_Basic1:
				m_sComponent.m_pMeshCom->Set_AnimationSet(98);
				break;
			case Client::CPlayer::StateA_Basic2:
				m_sComponent.m_pMeshCom->Set_AnimationSet(95);
				break;
			case Client::CPlayer::StateA_Basic3:
				m_sComponent.m_pMeshCom->Set_AnimationSet(92);
				break;
			default:
				break;
			}
			m_State = _state;
			m_LongBow->Set_Pos();

			break;
		}
		case Client::CPlayer::State_JumpEnd: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(277);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_Damaged: {
			m_sComponent.m_pMeshCom->Set_AnimationSet(183);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.4f;
			if (m_fJumpAccel < 1.42f) {
				m_fJumpAccel = 1.42f;
			}
			m_bCheck[bCheck::bCheck_DamagedUp] = false;
			m_bCheck[bCheck_JumpToDamagedUp] = true;
			Voice_Random(RandomSound::Sound_DamagedVoice);
			break;
		}
		case Client::CPlayer::State_End: {
			break;
		}
		default:
			break;
		}
	}
	m_State = _state;
}

void Client::CPlayer::Hammer_Key_InputOfJump(const _float& fTimeDelta)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	Engine::CNaviMesh* pNaviMeshCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_NaviTerrain->m_pNaviMeshCom;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_LOOK, &m_sStat.m_vDir);
	D3DXVec3Normalize(&m_sStat.m_vDir, &m_sStat.m_vDir);
	_vec3	vPos, vDir, vDir2;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	vDir = m_sStat.m_vDir;
	D3DXVec3Normalize(&vDir, &vDir);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 피격 당할때
	if (m_State == State::State_Damaged) { ////////////////////////////////////////////////////////////////////////////// 피격
		if (true) {		//강한 공격을 맞고 다운 //점프피격이라 반드시
			if (!m_bCheck[bCheck::bCheck_DamagedUp]) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.4f)) {
					m_bCheck[bCheck::bCheck_DamagedUp] = true;
					m_sComponent.m_pMeshCom->Set_AnimationSet(245);
				}
				else {
					if (m_sStat.m_fKnockBackPower > COLLDOWNPOWER) {
						if (m_sComponent.m_pMeshCom->Get_AnimationTrackPos() < 0.68f) {
							m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_sStat.m_fKnockBackDir * fTimeDelta * m_sStat.m_fSpeed * 1.5f * m_sStat.m_fKnockBackPower), &m_sStat.m_dwNaviIndex));
						}
					}
					else {
						float trackPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();
						if (0.2f > trackPos) {
							m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_sStat.m_fKnockBackDir * fTimeDelta * m_sStat.m_fSpeed * 1.5f * m_sStat.m_fKnockBackPower), &m_sStat.m_dwNaviIndex));
						}
					}
					m_sStat.m_fSpeed -= m_sStat.m_fInitSpeed * fTimeDelta * 2.5f;
					return;
				}
			}
			else {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.9f)) {
					Set_StateToAnimation(State::State_Idle);
				}
				return;
			}
		}
	}
	else if (m_State == State::State_Attack) {
		if (m_Attack_State == Attack_State::StateA_Basic4){
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_sStat.m_fSpeed * 1.4f), &m_sStat.m_dwNaviIndex));
		}
		else {
			if (m_bCheck[bCheck::bCheck_MouseL_Already]) {
				if (m_Attack_State == Attack_State::StateA_Basic1) {
					if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.5f)) {
						Set_StateToAnimation_Jump(State::State_Attack);
						m_bCheck[bCheck::bCheck_MouseL_Already] = false;
						Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
					}
				}
				else if (m_Attack_State == Attack_State::StateA_Basic2) {
					if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.2f)) {
						Set_StateToAnimation_Jump(State::State_Attack);
						m_bCheck[bCheck::bCheck_MouseL_Already] = false;
						Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
					}
				}
				else if (m_Attack_State == Attack_State::StateA_Basic3) {
					if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.2f)) {
						Set_StateToAnimation_Jump(State::State_Attack);
						m_bCheck[bCheck::bCheck_MouseL_Already] = false;
						Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
					}
				}
			}
			else if (m_Attack_State == Attack_State::StateA_Basic1 && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.2f)) {
				Set_StateToAnimation_Jump(State::State_Idle);
			}
			else if (m_Attack_State == Attack_State::StateA_Basic2 && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.02f)) {
				Set_StateToAnimation_Jump(State::State_Idle);
			}
			else if (m_Attack_State == Attack_State::StateA_Basic3 && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.05f)) {
				Set_StateToAnimation_Jump(State::State_Idle);
			}
			else if ((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) &&
				(m_Attack_State == Attack_State::StateA_Basic1 || m_Attack_State == Attack_State::StateA_Basic2 || m_Attack_State == Attack_State::StateA_Basic3)) {
				float aniPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();

				if (aniPos > 0.35f) {
					m_bCheck[bCheck::bCheck_MouseL_Already] = true;
				}
			}
			//공격모션 동안 움직임
			float trackPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();
			if (trackPos < 0.3f && trackPos > 0.15f) {
				m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			}
			if (trackPos > 0.2f && trackPos < 0.9f) {
				Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
			}
			else {
				Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
			}
		}
	}
	else if ((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) && !m_bCheck[bCheck::bCheck_MouseL])
	{
		m_bCheck[bCheck::bCheck_MouseL] = true;
		Set_StateToAnimation_Jump(State::State_Attack);
		m_bCheck[bCheck::bCheck_MouseL_Already] = false;
	}
	else if (Engine::Get_DIKeyState(DIK_W) & 0x80)
	{
		if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
			
			m_sComponent.m_pMeshCom->Set_AniAngle(220.f);
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(310.f);
		}
		else {
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(265.f);
		}
		Set_StateToAnimation_Jump(State::State_Idle);
	}
	else if (Engine::Get_DIKeyState(DIK_S) & 0x80)
	{
		if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(175.f);
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(355.f);
		}
		else {
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(85.f);
		}
		Set_StateToAnimation_Jump(State::State_Idle);
	}
	else if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
		vDir2 = { -vDir.z, 0.f, vDir.x };
		m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

		m_sComponent.m_pMeshCom->Set_AniAngle(175.f);
		Set_StateToAnimation_Jump(State::State_Idle);
	}
	else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
		vDir2 = { vDir.z, 0.f, -vDir.x };
		m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

		m_sComponent.m_pMeshCom->Set_AniAngle(355.f);
		Set_StateToAnimation_Jump(State::State_Idle);
	}
	else {
		Set_StateToAnimation_Jump(State::State_Idle);
	}

}


void Client::CPlayer::LongBow_Key_InputOfJump(const _float& fTimeDelta)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	Engine::CNaviMesh* pNaviMeshCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_NaviTerrain->m_pNaviMeshCom;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_LOOK, &m_sStat.m_vDir);
	D3DXVec3Normalize(&m_sStat.m_vDir, &m_sStat.m_vDir);
	_vec3	vPos, vDir, vDir2;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	vDir = m_sStat.m_vDir;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 피격 당할때
	if (m_State == State::State_Damaged) { ////////////////////////////////////////////////////////////////////////////// 피격
		if (true) {		//강한 공격을 맞고 다운 //점프피격이라 반드시
			if (!m_bCheck[bCheck::bCheck_DamagedUp]) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.4f)) {
					m_bCheck[bCheck::bCheck_DamagedUp] = true;
					m_sComponent.m_pMeshCom->Set_AnimationSet(245);
				}
				else {
					if (m_sStat.m_fKnockBackPower > COLLDOWNPOWER) {
						if (m_sComponent.m_pMeshCom->Get_AnimationTrackPos() < 0.68f) {
							m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_sStat.m_fKnockBackDir * fTimeDelta * m_sStat.m_fSpeed * 1.5f * m_sStat.m_fKnockBackPower), &m_sStat.m_dwNaviIndex));
						}
					}
					else {
						float trackPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();
						if (0.2f > trackPos) {
							m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_sStat.m_fKnockBackDir * fTimeDelta * m_sStat.m_fSpeed * 1.5f * m_sStat.m_fKnockBackPower), &m_sStat.m_dwNaviIndex));
						}
					}
					m_sStat.m_fSpeed -= m_sStat.m_fInitSpeed * fTimeDelta * 2.5f;
					return;
				}
			}
			else {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.9f)) {
					Set_StateToAnimation(State::State_Idle);
				}
				return;
			}
		}
	}
	else if (m_State == State::State_Attack) {
		
		if (m_bCheck[bCheck::bCheck_MouseL_Already]) {
			if (m_Attack_State == Attack_State::StateA_Basic1) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.6f)) {
					Set_StateToAnimation_Jump(State::State_Attack, vPos, vDir, fTimeDelta);
					m_bCheck[bCheck::bCheck_MouseL_Already] = false;
				}
			}
			else if (m_Attack_State == Attack_State::StateA_Basic2) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.6f)) {
					Set_StateToAnimation_Jump(State::State_Attack, vPos, vDir, fTimeDelta);
					m_bCheck[bCheck::bCheck_MouseL_Already] = false;
				}
			}
		}
		else if (m_Attack_State == Attack_State::StateA_Basic1 && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.5f)) {
			Set_StateToAnimation_Jump(State::State_Idle);
		}
		else if (m_Attack_State == Attack_State::StateA_Basic2 && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.5f)) {
			Set_StateToAnimation_Jump(State::State_Idle);
		}
		else if (m_Attack_State == Attack_State::StateA_Basic3 && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.5f)) {
			Set_StateToAnimation_Jump(State::State_Idle);
		}
		else if ((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) &&
			(m_Attack_State == Attack_State::StateA_Basic1 || m_Attack_State == Attack_State::StateA_Basic2)) {
			float aniPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();

			if (aniPos > 0.4f) {
				m_bCheck[bCheck::bCheck_MouseL_Already] = true;
			}
		}
	}
	else if ((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) && !m_bCheck[bCheck::bCheck_MouseL])
	{
		m_bCheck[bCheck::bCheck_MouseL] = true;
		Set_StateToAnimation_Jump(State::State_Attack, vPos, vDir, fTimeDelta);
		m_bCheck[bCheck::bCheck_MouseL_Already] = false;
	}
	else if (Engine::Get_DIKeyState(DIK_W) & 0x80)
	{
		if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(220.f);
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(310.f);
		}
		else {
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(265.f);
		}
		Set_StateToAnimation_Jump(State::State_Idle);
	}
	else if (Engine::Get_DIKeyState(DIK_S) & 0x80)
	{
		if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(175.f);
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(355.f);
		}
		else {
			m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			m_sComponent.m_pMeshCom->Set_AniAngle(85.f);
		}
		Set_StateToAnimation_Jump(State::State_Idle);
	}
	else if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
		vDir2 = { -vDir.z, 0.f, vDir.x };
		m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

		m_sComponent.m_pMeshCom->Set_AniAngle(175.f);
		Set_StateToAnimation_Jump(State::State_Idle);
	}
	else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
		vDir2 = { vDir.z, 0.f, -vDir.x };
		m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

		m_sComponent.m_pMeshCom->Set_AniAngle(355.f);
		Set_StateToAnimation_Jump(State::State_Idle);
	}
	else {
		Set_StateToAnimation_Jump(State::State_Idle);
	}

}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer*	pInstance = new CPlayer(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);
	return pInstance;
}

void CPlayer::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CPlayer::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_sComponent.m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);

	m_sComponent.m_pMeshCom->Set_AnimationSet(243);
	m_AniSpeed = 1.f;
	m_Attack_State = Attack_State::StateA_None;
	Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
	m_sStat.m_dwNaviIndex = 0;
	//m_pNaviMeshCom->Set_NaviIndex(0);

	return S_OK;
}
Client::_int Client::CPlayer::Update_Object(const _float& fTimeDelta)
{
	Talk_NpcToPortal();
	if (CNpcQuest_Manager::Get_NpcQuestInfo()->m_PlayerTalk)
		return 0;

	Calc_Time(fTimeDelta);
	if (!CNpcQuest_Manager::Get_NpcQuestInfo()->m_TalkEnd) {
		if (m_JumpIdleState != JumpIdleAni::JumpIdle_None) {
			if (m_WeaponEquip == Weapon_Equip::Weapon_Hammer)
				Hammer_Key_InputOfJump(fTimeDelta);
			else if (m_WeaponEquip == Weapon_Equip::Weapon_LongBow)
				LongBow_Key_InputOfJump(fTimeDelta);
			Jump_Control(fTimeDelta);
		}
		else {
			SetUp_OnTerrain();
			if (m_bCheck[bCheck::bCheck_State_WeaponChange]) {
				WeaponChangeState_Key_Input(fTimeDelta);
			}
			else if (m_WeaponEquip == Weapon_Equip::Weapon_Hammer)
				Hammer_Key_Input(fTimeDelta);
			else if (m_WeaponEquip == Weapon_Equip::Weapon_LongBow)
				LongBow_Key_Input(fTimeDelta);
		}
	}
	//
	//CameraControl(fTimeDelta);
	//
	Set_RenderCollSphere();
	if (Engine::Get_DIKeyState(DIK_F11) & 0x80) {
		m_sComponent.m_pTransformCom->Set_Pos(&Engine::_vec3{ 54.f, 0.f, 54.f });
	}

	m_sComponent.m_pMeshCom->Play_Animation(fTimeDelta * m_AniSpeed);
	CUnit_D::Update_Object(fTimeDelta);

	m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}

void Client::CPlayer::Render_Object(void)
{
	CUnit_D::Render_Object();

}
void CPlayer::Add_LookAtY(float lookat)
{
	if (m_LookAtY + lookat > 4.5f) {
		m_LookAtY = 4.5f;
	}
	else if (m_LookAtY + lookat < -1.5f) {
		m_LookAtY = -1.5f;
	}
	else {
		m_LookAtY += lookat;
	}
}

void Client::CPlayer::Calc_Time(_float fTimeDelta)
{
	/*if (m_TimeCheck[TimeCheck::TimeCheck_KeyW] > 0.f)
		m_TimeCheck[TimeCheck::TimeCheck_KeyW] -= fTimeDelta;
	if (m_TimeCheck[TimeCheck::TimeCheck_KeyA] > 0.f)
		m_TimeCheck[TimeCheck::TimeCheck_KeyA] -= fTimeDelta;
	if (m_TimeCheck[TimeCheck::TimeCheck_KeyS] > 0.f)
		m_TimeCheck[TimeCheck::TimeCheck_KeyS] -= fTimeDelta;
	if (m_TimeCheck[TimeCheck::TimeCheck_KeyD] > 0.f)
		m_TimeCheck[TimeCheck::TimeCheck_KeyD] -= fTimeDelta;*/
	for (int i = 0; i < TimeCheck::TimeCheck_End; i++)
	{
		if (m_TimeCheck[i] > 0.f)
			m_TimeCheck[i] -= fTimeDelta;
	}
	if (m_sStat.m_fHp < m_sStat.m_fMaxHp) {
		m_sStat.m_fHp += fTimeDelta * 10.f;
		if (m_sStat.m_fHp > m_sStat.m_fMaxHp)
			m_sStat.m_fHp = m_sStat.m_fMaxHp;
	}
	if (m_sStat.m_fMp < m_sStat.m_fMaxMp) {
		m_sStat.m_fMp += fTimeDelta * 5.f;
		if (m_sStat.m_fMp > m_sStat.m_fMaxMp)
			m_sStat.m_fMp = m_sStat.m_fMaxMp;
	}
	if (m_DashGauge < m_MaxDashGauge) {
		m_DashGauge += fTimeDelta * 5.f;
		if (m_DashGauge > m_MaxDashGauge)
			m_DashGauge = m_MaxDashGauge;
	}
	Dissolve_WeaponIdle(fTimeDelta);
}

void Client::CPlayer::Key_DoubleInput(const _float& fTimeDelta)
{
	if (Engine::Get_DIKeyState(DIK_W) & 0x80 && !m_bCheck[bCheck::bCheck_KeyW])
	{
		m_bCheck[bCheck::bCheck_DBKeyW] = false;
		m_bCheck[bCheck::bCheck_KeyW] = true;
		if (m_TimeCheck[TimeCheck::TimeCheck_KeyW] > 0.f)
			m_bCheck[bCheck::bCheck_DBKeyW] = true;
		else
			m_TimeCheck[TimeCheck::TimeCheck_KeyW] = 0.2f;
	}
	else if (!(Engine::Get_DIKeyState(DIK_W) & 0x80)) {
		m_bCheck[bCheck::bCheck_KeyW] = false;
	}

	if (Engine::Get_DIKeyState(DIK_A) & 0x80 && !m_bCheck[bCheck::bCheck_KeyA])
	{
		m_bCheck[bCheck::bCheck_DBKeyA] = false;
		m_bCheck[bCheck::bCheck_KeyA] = true;
		if (m_TimeCheck[TimeCheck::TimeCheck_KeyA] > 0.f)
			m_bCheck[bCheck::bCheck_DBKeyA] = true;
		else
			m_TimeCheck[TimeCheck::TimeCheck_KeyA] = 0.2f;
	}
	else if (!(Engine::Get_DIKeyState(DIK_A) & 0x80)) {
		m_bCheck[bCheck::bCheck_KeyA] = false;
	}

	if (Engine::Get_DIKeyState(DIK_S) & 0x80 && !m_bCheck[bCheck::bCheck_KeyS])
	{
		m_bCheck[bCheck::bCheck_DBKeyS] = false;
		m_bCheck[bCheck::bCheck_KeyS] = true;
		if (m_TimeCheck[TimeCheck::TimeCheck_KeyS] > 0.f)
			m_bCheck[bCheck::bCheck_DBKeyS] = true;
		else
			m_TimeCheck[TimeCheck::TimeCheck_KeyS] = 0.2f;
	}
	else if (!(Engine::Get_DIKeyState(DIK_S) & 0x80)) {
		m_bCheck[bCheck::bCheck_KeyS] = false;
	}

	if (Engine::Get_DIKeyState(DIK_D) & 0x80 && !m_bCheck[bCheck::bCheck_KeyD])
	{
		m_bCheck[bCheck::bCheck_DBKeyD] = false;
		m_bCheck[bCheck::bCheck_KeyD] = true;
		if (m_TimeCheck[TimeCheck::TimeCheck_KeyD] > 0.f)
			m_bCheck[bCheck::bCheck_DBKeyD] = true;
		else
			m_TimeCheck[TimeCheck::TimeCheck_KeyD] = 0.2f;
	}
	else if (!(Engine::Get_DIKeyState(DIK_D) & 0x80)) {
		m_bCheck[bCheck::bCheck_KeyD] = false;
	}

	// 마우스
	if (!(Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80)) {
		m_bCheck[bCheck::bCheck_MouseL] = false;
	}
	if (!(Engine::Get_DIMouseState(Engine::DIM_RB) & 0x80)) {
		m_bCheck[bCheck::bCheck_MouseR] = false;
	}
}

void Client::CPlayer::Jump_Control(const _float& fTimeDelta)
{
	_vec3	vPosition;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_Terrain->Get_sComponent_Terrain()->m_pBufferCom;
	//Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_sComponent.m_pCalculatorCom->Compute_HeightOnTerrain(&vPosition, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	if (m_State == State::State_Idle || m_State == State::State_Damaged || m_State == State::State_DamagedDown || (m_WeaponEquip == Weapon_Equip::Weapon_Hammer && m_Attack_State == Attack_State::StateA_Basic4)) {
		if (m_State == State::State_Idle || m_State == State::State_Damaged || m_State == State::State_DamagedDown) {
			if (m_fJumpAccel < 2.f) {
				m_fJumpAccel += 2.5f * fTimeDelta;
			}
		}
		else if(m_Attack_State == Attack_State::StateA_Basic4){
			if (m_fJumpAccel < 4.f) {
				m_fJumpAccel += 4.f * fTimeDelta;
			}
		}
		float gravity = m_fJumpPower * m_fJumpAccel - (GRAVITY * m_fJumpAccel * m_fJumpAccel * 0.5f);

		float* beforePosY = &m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS].y;
		float afterPosY = *beforePosY + gravity * fTimeDelta * 180.f;

		if (fHeight > afterPosY) {
			m_JumpIdleState = JumpIdleAni::JumpIdle_None;
			if (m_State != State::State_Damaged && m_State != State::State_DamagedDown) {
				if (m_State == State::State_Idle)
					Set_StateToAnimation(State::State_JumpEnd);
				else if (m_Attack_State == Attack_State::StateA_Basic4)
					Set_StateToAnimation(State::State_JumpComboEnd);
			}
			//m_sComponent.m_pMeshCom->Is_Loop_Change(FALSE);
			m_fJumpAccel = 0.f;
			*beforePosY = fHeight;
		}
		else {
			if (gravity >= 0) {
				//if(m_JumpIdleState != JumpIdleAni::JumpIdle_JumpStart)
				//m_JumpIdleState = JumpIdleAni::JumpIdle_JumpUp;
			}
			else if(m_State != State::State_Damaged && m_State != State::State_DamagedDown)
				m_JumpIdleState = JumpIdleAni::JumpIdle_JumpDown;
			*beforePosY = afterPosY;
		}
	}
}

void CPlayer::Event_Skill(float fTimeDelta, Engine::CNaviMesh* pNaviMeshCom, _vec3 vPos, _vec3 vDir) {
	///////////////////////////////////////////////////////////////////////////////////////////////////////////// Hammer Skill
	if (m_WeaponEquip == Weapon_Equip::Weapon_Hammer) {
		if (m_Attack_State == Attack_State::StateA_SkillF) {
			if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.35f)) {
				Set_StateToAnimation(State::State_Idle);
			}
			float trackPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();
			if (!m_bCheck[bCheck::bCheck_Skill_F1] && trackPos > 0.3f) {
				m_bCheck[bCheck::bCheck_Skill_F1] = true;
				CGH_RockIn::Create(m_pGraphicDev, true);
				dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera()->Set_ShakeTime(0.5f);
				CSkillCollider::Create(m_pGraphicDev)->Set_Collider(vPos, 5.f, CSphereCollider::BoneTeam_Player, m_sStat.m_fAttack * 1.5f, 0.5f);
				Voice_Random(RandomSound::Sound_AttackVoice);
			}
			else if (!m_bCheck[bCheck::bCheck_Skill_F2] && trackPos > 0.9f) {
				m_bCheck[bCheck::bCheck_Skill_F2] = true;
				CGH_RockIn::Create(m_pGraphicDev, true);
				dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera()->Set_ShakeTime(0.5f);
				CSkillCollider::Create(m_pGraphicDev)->Set_Collider(vPos, 5.f, CSphereCollider::BoneTeam_Player, m_sStat.m_fAttack * 1.5f, 0.5f);
				Voice_Random(RandomSound::Sound_AttackVoice);
			}
			else if (!m_bCheck[bCheck::bCheck_Skill_F3] && trackPos > 2.3f) {
				m_bCheck[bCheck::bCheck_Skill_F3] = true;
				CGH_RockIn::Create(m_pGraphicDev, false);
				dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera()->Set_ShakeTime(1.f);
				CSkillCollider::Create(m_pGraphicDev)->Set_Collider(vPos, 6.5f, CSphereCollider::BoneTeam_Player, m_sStat.m_fAttack * 2.f, 0.5f);
				Voice_Random(RandomSound::Sound_SkillVoice);
			}
			if (1.6f < trackPos && trackPos < 2.3f) {
				m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_sStat.m_fSpeed * 0.5f), &m_sStat.m_dwNaviIndex));
			}
		}
		else if (m_Attack_State == Attack_State::StateA_SkillE) {
			if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.3f)) {
				Set_StateToAnimation(State::State_Idle);
			}
			float trackPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();
			if (!m_bCheck[bCheck::bCheck_Skill_F1] &&  trackPos > 0.7f) {
				CSkillCollider::Create(m_pGraphicDev)->Set_Collider(vPos, 7.f, CSphereCollider::BoneTeam_Player, m_sStat.m_fAttack * 3.f, 0.5f);
				dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera()->Set_ShakeTime(0.5f);
				m_bCheck[bCheck::bCheck_Skill_F1] = true;
			}
			if (0.7f < trackPos && trackPos < 0.8f && m_TimeCheck[TimeCheck_Effect_GHSKillE] <= 0.f) {
				m_TimeCheck[TimeCheck_Effect_GHSKillE] = 0.05f;
				CEffectMesh_GHSkillE::Create(m_pGraphicDev);
			}
		}
		else if (m_Attack_State == Attack_State::StateA_SkillQ) {
			if (m_AniClip == AnimationClip::Ani_1) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.05f)) {
					m_AniClip = AnimationClip::Ani_2;
					m_sComponent.m_pMeshCom->Set_AnimationSet(124);
				}
			}
			else if (m_AniClip == AnimationClip::Ani_2) {
				bool colCheck;
				m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_sStat.m_fSpeed * 4.f), &m_sStat.m_dwNaviIndex, &colCheck));
				Engine::CRenderer::GetInstance()->Set_MotionBlur(0.8f);
				if (colCheck) {
					m_AniClip = AnimationClip::Ani_3;
					
					m_sComponent.m_pMeshCom->Set_AnimationSet(125);
					CGH_RockIn::Create(m_pGraphicDev, true);
					dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera()->Set_ShakeTime(0.8f);
					CSkillCollider::Create(m_pGraphicDev)->Set_Collider(vPos, 5.f, CSphereCollider::BoneTeam_Player, m_sStat.m_fAttack * 5.f, 0.5f);
				}
			}
			else if (m_AniClip == AnimationClip::Ani_3) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.66f)) {
					Set_StateToAnimation(State::State_Idle);
				}
			}
		}
		else if (m_Attack_State == Attack_State::StateA_SkillZ) {
			if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.01f)) {
				Set_StateToAnimation(State::State_Idle);
				Set_StateToAnimation(State::State_Move);
				m_TimeCheck[TimeCheck::TimeCheck_Invin] = 0.f;
				return;
			}
			float trackPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();
			/*if (trackPos > 0.7f) {
				Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
			}
			else */if (trackPos > 1.6f && trackPos < 1.7f) {
				dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera()->Set_ShakeTime(1.2f);
			}
		}
		Move_AccelSpeed(vPos, vDir, fTimeDelta, pNaviMeshCom);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////// LongBow Skill
	if (m_WeaponEquip == Weapon_Equip::Weapon_LongBow) {
		if (m_Attack_State == Attack_State::StateA_SkillE) {
			if (m_AniClip == AnimationClip::Ani_1) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.6f)) {
					m_AniClip = AnimationClip::Ani_2;
					m_TimeCheck[TimeCheck::TimeCheck_LB_SkillE_Attack] = 1.f;
					m_TimeCheck[TimeCheck::TimeCheck_LB_SkillE_Attack_Arrow_Start_Total] = 2.5f;
					m_sComponent.m_pMeshCom->Set_AnimationSet(32);
					m_AniSpeed = 1.f;
					CSkillCollider::Create(m_pGraphicDev)->Set_Collider(m_LB_SkillE_Pos, 7.f, CSphereCollider::BoneTeam_Player, m_sStat.m_fAttack * 0.4f, 3.f, 0.3f, 0.05f, 0.7f);
					SoundManager::PlayOverlapSound(L"LB_SkillE.ogg", SoundChannel::PLAYER, VOLUME);
				}
			}
			else if (m_AniClip == AnimationClip::Ani_2) {
				if (m_TimeCheck[TimeCheck::TimeCheck_LB_SkillE_Attack] <= 0.f) {
					m_AniClip = AnimationClip::Ani_3;
					m_sComponent.m_pMeshCom->Set_AnimationSet(33);
					m_AniSpeed = 3.f;
				}
				if (m_TimeCheck[TimeCheck_LB_SkillE_Attack_Arrow] <= 0.f) {
					m_TimeCheck[TimeCheck_LB_SkillE_Attack_Arrow] = 0.2f;
					Create_ArrowShot_SkillE_Ready(vPos, _vec3{ 0.f, 1.f, 0.f });
					dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera()->Set_ShakeTime(0.2f);
				}
			}
			else if (m_AniClip == AnimationClip::Ani_3) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.4f)) {
					Set_StateToAnimation(State::State_Idle);
				}
			}
		}
		else if (m_Attack_State == Attack_State::StateA_SkillQ) {
			if (m_AniClip == AnimationClip::Ani_1) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.f)) {
					m_AniClip = AnimationClip::Ani_2;
					Create_ArrowShot_SkillQ(vPos, vDir);
					_vec3 vSkillDir = { -vDir.z, vDir.y, vDir.x  };
					_vec3 vSkillDirNomal = _vec3{ vDir.x + vSkillDir.x , vDir.y, vDir.z + vSkillDir.z };
					D3DXVec3Normalize(&vSkillDirNomal, &vSkillDirNomal);
					Create_ArrowShot_SkillQ(vPos, vSkillDirNomal);
					vSkillDir.x *= -1.f; vSkillDir.z *= -1.f;
					vSkillDirNomal = _vec3{ vDir.x + vSkillDir.x , vDir.y, vDir.z + vSkillDir.z };
					D3DXVec3Normalize(&vSkillDirNomal, &vSkillDirNomal);
					Create_ArrowShot_SkillQ(vPos, vSkillDirNomal);
					m_sComponent.m_pMeshCom->Set_AnimationSet(54);
				}
			}
			else if (m_AniClip == AnimationClip::Ani_2) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.f)) {
					m_AniClip = AnimationClip::Ani_3;
					m_sComponent.m_pMeshCom->Set_AnimationSet(55);
				}
			}
			else if (m_AniClip == AnimationClip::Ani_3) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.f)) {
					Set_StateToAnimation(State::State_Idle);
				}
			}
		}
		else if (m_Attack_State == Attack_State::StateA_SkillF) {
			if (Engine::Get_DIKeyState(DIK_F) & 0x80 && m_TimeCheck[TimeCheck_Cool_F] <= COOLTIME_LB_F - CancelTime) {
				m_AniClip = AnimationClip::Ani_4;
				m_sComponent.m_pMeshCom->Set_AnimationSet(21);
				m_LB_Arrow_Count = 0;
			}
			else if (m_AniClip == AnimationClip::Ani_1) {
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.f)) {
					m_AniClip = AnimationClip::Ani_2;
					m_sComponent.m_pMeshCom->Set_AnimationSet(18);
					m_LB_Arrow_Count = 6;
				}
			}
			else if (m_AniClip == AnimationClip::Ani_2) { // Wait
				if (m_LB_Arrow_Count == 0) {
					m_AniClip = AnimationClip::Ani_4;
					m_sComponent.m_pMeshCom->Set_AnimationSet(21);
				}
				else if ((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) && !m_bCheck[bCheck::bCheck_MouseL]) {
					m_sComponent.m_pMeshCom->Set_AnimationSet(19);
					m_bCheck[bCheck::bCheck_MouseL] = true;
					m_bCheck[bCheck::bCheck_MouseL_Already] = false;
					m_AniClip = AnimationClip::Ani_3;
					Create_ArrowShot_SkillF(vPos, vDir, fTimeDelta);
					dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera()->Set_ShakeTime(0.1f);
					m_LB_Arrow_Count--;
					Voice_Random(RandomSound::Sound_AttackVoice);
				}
			}
			else if (m_AniClip == AnimationClip::Ani_3) { // Shot
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.2f)) {
					m_sComponent.m_pMeshCom->Set_AnimationSet(18);
					m_AniClip = AnimationClip::Ani_2;
				}
				else if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.4f) && (Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80)) {
					m_AniClip = AnimationClip::Ani_2;
					m_sComponent.m_pMeshCom->Set_AnimationSet(18);
				}
			}
			else if (m_AniClip == AnimationClip::Ani_4) { // End
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.3f)) {
					Set_StateToAnimation(State::State_Idle);
				}
			}
		}
		else if (m_Attack_State == Attack_State::StateA_SkillZ) {
			if (m_AniClip == AnimationClip::Ani_1) { //준비
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(4.f)) {
					m_AniClip = AnimationClip::Ani_2;
					m_sComponent.m_pMeshCom->Set_AnimationSet(8);
					SoundManager::PlayOverlapSound(L"LB_SkillZ01.ogg", SoundChannel::PLAYER, VOLUME);
				}
			}
			else if (m_AniClip == AnimationClip::Ani_2) { //샷
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.1f)) {
					m_AniClip = AnimationClip::Ani_3;
					m_sComponent.m_pMeshCom->Set_AnimationSet(7);
				}
				if (!m_bCheck[bCheck::bCheck_LB_Phoenix_SkillZ] && m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.1f)) {
					m_bCheck[bCheck::bCheck_LB_Phoenix_SkillZ] = true;
					Create_Phoenix_SkillZ(vPos, vDir);
					Voice_Random(RandomSound::Sound_AttackVoice);
					dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera()->Set_ShakeTime(0.8f);
				}
			}
			else if (m_AniClip == AnimationClip::Ani_3) { //End
				if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(1.f)) {
					Set_StateToAnimation(State::State_Idle);
					m_TimeCheck[TimeCheck::TimeCheck_Invin] = 0.f;
				}
			}
		}
		Move_AccelSpeed(vPos, vDir, fTimeDelta, pNaviMeshCom);
	}
}

void CPlayer::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{
	//충돌체가 같은 팀이었을경우
	if (_mySphere->m_BoneTeam == _colSphere->m_BoneTeam)
		return;

	if (_colSphere->m_BonePart == CSphereCollider::BonePart_CollBody && !_colSphere->m_WeaponAttack) {
		CUnit_D* _colUnit = dynamic_cast<CUnit_D*>(_col);
		if (nullptr == _colUnit)
			return;
		if (CNpcQuest_Manager::Get_NpcQuestInfo()->m_PlayerTalk)
			return;
		//
		_vec3 _colUnitPos = _colUnit->Get_sComponent()->m_pTransformCom->m_vInfo[Engine::INFO_POS];
		_vec3 _myUnitPos = m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS];
		float _Radius = (_colUnit->Get_sComponent()->m_pTransformCom->m_vScale.x * _colSphere->m_pTransformCom->m_vScale.x);
		_vec3 CollDir = _myUnitPos - _colUnitPos;
		CollDir.y = 0.f;
		D3DXVec3Normalize(&CollDir, &CollDir);
		Engine::CNaviMesh* pNaviMeshCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_NaviTerrain->m_pNaviMeshCom;
		_vec3	vPos;
		m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
		m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(CollDir * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
		Engine::CGameObject::Update_Object(fTimeDelta);
		//
	}

	//내 몸에 충돌했을때
	if (_mySphere->m_BonePart == CSphereCollider::BonePart_CollBody) {
		//스킬A 돌진시
		if (m_Attack_State == Attack_State::StateA_SkillQ) {
			if (m_AniClip == AnimationClip::Ani_2) {
				if (_colSphere->m_BonePart == CSphereCollider::BonePart_CollBody) {
					m_AniClip = AnimationClip::Ani_3;
					
					m_sComponent.m_pMeshCom->Set_AnimationSet(125);
					CGH_RockIn::Create(m_pGraphicDev, true);
					dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera()->Set_ShakeTime(0.8f);
					_vec3 vPos;
					m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
					CSkillCollider::Create(m_pGraphicDev)->Set_Collider(vPos, 5.f, CSphereCollider::BoneTeam_Player, m_sStat.m_fAttack * 5.f, 0.5f);
				}
			}
		}
		else if (_colSphere->m_WeaponAttack && m_State != State::State_Rolling) {
			if (m_TimeCheck[TimeCheck_Invin] <= 0.f) {
				m_sStat.m_fHp -= _colSphere->Get_sStat()->m_fAttack;
				m_sStat.m_fKnockBackPower = _colSphere->m_WeaponPower;
				if(m_JumpIdleState == JumpIdle_None)
					Set_StateToAnimation(State::State_Damaged);
				else
					Set_StateToAnimation_Jump(State::State_Damaged);
				m_LB_Arrow_Count = 0;
				m_TimeCheck[TimeCheck_Invin] = 4.f;
				m_sStat.m_fKnockBackDir = m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS] - dynamic_cast<CUnit*>(_col)->Get_sComponent()->m_pTransformCom->m_vInfo[Engine::INFO_POS];
				D3DXVec3Normalize(&m_sStat.m_fKnockBackDir, &m_sStat.m_fKnockBackDir);
			}
		}
	}
}

void CPlayer::Set_RenderCollSphere() {
	if (Engine::Get_DIKeyState(DIK_F2) & 0x80) {
		if (!m_bCheck[bCheck_KeyF2]) {
			if (m_bCheck[bCheck_RenderSphere])
				m_bCheck[bCheck_RenderSphere] = false;
			else
				m_bCheck[bCheck_RenderSphere] = true;
			m_bCheck[bCheck_KeyF2] = true;
		}
	}
	else {
		m_bCheck[bCheck_KeyF2] = false;
	}
}

void CPlayer::Change_Weapon() {
	Engine::CLayer* pLayer = Engine::CManagement::GetInstance()->m_pScene->Get_Layer(Engine::CLayer::LayerName::Layer_Static);
	
	if (m_WeaponEquip == Weapon_Equip::Weapon_Hammer) {
		m_Hammer->Get_sStat()->m_IsDead = true;
		m_Hammer = nullptr;
		Engine::CGameObject* pGameObject = CLongBow::Create(m_pGraphicDev);
		pLayer->Add_GameObject(L"CLongBow", pGameObject);

		m_WeaponEquip = Weapon_Equip::Weapon_LongBow;
	}
	else if (m_WeaponEquip == Weapon_Equip::Weapon_LongBow) {
		m_bCheck[bCheck::bCheck_LB_SkillE] = false;
		m_TimeCheck[TimeCheck_LB_SkillE_Ready] = 0.f;
		m_LongBow->Get_sStat()->m_IsDead = true;
		m_LongBow = nullptr;
		Engine::CGameObject* pGameObject = CHammer::Create(m_pGraphicDev);
		pLayer->Add_GameObject(L"CHammer", pGameObject);

		m_WeaponEquip = Weapon_Equip::Weapon_Hammer;
	}
}

void CPlayer::Create_ArrowShot(_vec3 _vPos, _vec3 _vDir, float fTimeDelta)
{
	CUnit* pUnit;
	Engine::CGameObject* pGameObject = pUnit = CArrow::Create(m_pGraphicDev);
	dynamic_cast<CArrow*>(pUnit)->Create_Coll();
	pUnit->Get_sComponent()->m_pTransformCom->m_vInfo[Engine::INFO_POS] = _vPos + _vDir * 3.2f;
	pUnit->Get_sComponent()->m_pTransformCom->m_vInfo[Engine::INFO_POS].y += 1.2f;
	if (m_JumpIdleState == JumpIdleAni::JumpIdle_None)
		_vDir.y = -0.4f + m_LookAtY * 0.25f;
	else
		_vDir.y = -0.4f + m_LookAtY * 0.25f;

	D3DXVec3Normalize(&_vDir, &_vDir);
	pUnit->Get_sStat()->m_vDir = _vDir;

	dynamic_cast<CArrow*>(pUnit)->Update_Object(0.f);
	pUnit->Get_sStat()->m_fAttack = m_sStat.m_fAttack;
	Engine::CLayer* pLayer = Engine::CManagement::GetInstance()->m_pScene->Get_Layer(Engine::CLayer::LayerName::Layer_Static);
	pLayer->Add_GameObject(L"Arrow", pGameObject);
}

void CPlayer::Create_ArrowShot_SkillF(_vec3 _vPos, _vec3 _vDir, float fTimeDelta)
{
	CUnit* pUnit;
	Engine::CGameObject* pGameObject = pUnit = CArrow_SkillF::Create(m_pGraphicDev);
	dynamic_cast<CArrow_SkillF*>(pUnit)->Create_Coll();
	pUnit->Get_sComponent()->m_pTransformCom->m_vInfo[Engine::INFO_POS] = _vPos + _vDir * 6.f;
	pUnit->Get_sComponent()->m_pTransformCom->m_vInfo[Engine::INFO_POS].y += 1.2f;
	_vDir.y = -0.4f + m_LookAtY * 0.25f;
	D3DXVec3Normalize(&_vDir, &_vDir);
	pUnit->Get_sStat()->m_vDir = _vDir;

	dynamic_cast<CArrow_SkillF*>(pUnit)->Update_Object(0.f);
	pUnit->Get_sStat()->m_fAttack = m_sStat.m_fAttack;
	Engine::CLayer* pLayer = Engine::CManagement::GetInstance()->m_pScene->Get_Layer(Engine::CLayer::LayerName::Layer_Static);
	pLayer->Add_GameObject(L"Arrow", pGameObject);
}

void CPlayer::Create_ArrowShot_SkillE_Ready(_vec3 _vPos, _vec3 _vDir)
{
	CUnit* pUnit;
	Engine::CGameObject* pGameObject = pUnit = CArrow::Create(m_pGraphicDev);
	//dynamic_cast<CArrow*>(pUnit)->Create_Coll();
	pUnit->Get_sComponent()->m_pTransformCom->m_vInfo[Engine::INFO_POS] = _vPos;
	pUnit->Get_sComponent()->m_pTransformCom->m_vInfo[Engine::INFO_POS].y += 5.f;
	pUnit->Get_sStat()->m_vDir = _vDir;

	dynamic_cast<CArrow*>(pUnit)->Update_Object(0.f);
	//pUnit->m_sStat.m_fAttack = m_sStat.m_fAttack;
	Engine::CLayer* pLayer = Engine::CManagement::GetInstance()->m_pScene->Get_Layer(Engine::CLayer::LayerName::Layer_StaticNoColl);
	pLayer->Add_GameObject(L"Arrow_SkillE_Ready", pGameObject);
}

void CPlayer::Create_ArrowShot_SkillE_Start()
{
	int dist = CRandom_Manager::Random() % 8;
	
	float degree = (float)(CRandom_Manager::Random() % 360);
	float Radian = D3DXToRadian(degree);

	_vec3 vecSpawnPosition;
	vecSpawnPosition.x = dist;
	vecSpawnPosition.z = 0;

	float movePositionX = cosf(Radian) * vecSpawnPosition.x - sinf(Radian) * vecSpawnPosition.z + m_LB_SkillE_Pos.x;
	float movePositionZ = sinf(Radian) * vecSpawnPosition.x + cosf(Radian) * vecSpawnPosition.z + m_LB_SkillE_Pos.z;

	_vec3 vecRain_Pos = { movePositionX, 0.f, movePositionZ };

	_vec3 vecRandomDir = { float(CRandom_Manager::Random() % 11), 50.f, float(CRandom_Manager::Random() % 11) };
	D3DXVec3Normalize(&vecRandomDir, &vecRandomDir);
	//
	CUnit* pUnit;
	Engine::CGameObject* pGameObject = pUnit = CArrow::Create(m_pGraphicDev);
	//dynamic_cast<CArrow*>(pUnit)->Create_Coll();
	pUnit->Get_sComponent()->m_pTransformCom->m_vInfo[Engine::INFO_POS] = vecRain_Pos + vecRandomDir * 40.f;
	pUnit->Get_sStat()->m_vDir = -vecRandomDir;
	dynamic_cast<CArrow*>(pUnit)->Set_SpeedToLife(100.f, 1.2f);

	dynamic_cast<CArrow*>(pUnit)->Update_Object(0.f);
	//pUnit->m_sStat.m_fAttack = m_sStat.m_fAttack;
	Engine::CLayer* pLayer = Engine::CManagement::GetInstance()->m_pScene->Get_Layer(Engine::CLayer::LayerName::Layer_StaticNoColl);
	pLayer->Add_GameObject(L"Arrow_SkillE_Start", pGameObject);
}

void CPlayer::Create_ArrowShot_SkillQ(_vec3 _vPos, _vec3 _vDir)
{
	CUnit* pUnit;
	Engine::CGameObject* pGameObject = pUnit = CArrow_SkillQ::Create(m_pGraphicDev);
	dynamic_cast<CArrow_SkillQ*>(pUnit)->Create_Coll();
	pUnit->Get_sComponent()->m_pTransformCom->m_vInfo[Engine::INFO_POS] = _vPos + _vDir * 3.2f;
	pUnit->Get_sComponent()->m_pTransformCom->m_vInfo[Engine::INFO_POS].y += 1.2f;
	_vDir.y = -0.4f + m_LookAtY * 0.25f;

	D3DXVec3Normalize(&_vDir, &_vDir);
	pUnit->Get_sStat()->m_vDir = _vDir;
	dynamic_cast<CArrow_SkillQ*>(pUnit)->Set_SplitCount(3);

	pUnit->Engine::CGameObject::Update_Object(0.f);
	dynamic_cast<CArrow_SkillQ*>(pUnit)->Update_Object(0.f);
	pUnit->Get_sStat()->m_fAttack = m_sStat.m_fAttack * 0.5f;
	Engine::CLayer* pLayer = Engine::CManagement::GetInstance()->m_pScene->Get_Layer(Engine::CLayer::LayerName::Layer_Static);
	pLayer->Add_GameObject(L"Arrow_SkillQ", pGameObject);
}

void CPlayer::Create_Phoenix_SkillZ(_vec3 _vPos, _vec3 _vDir)
{
	CUnit* pUnit;
	Engine::CGameObject* pGameObject = pUnit = CPhoenix::Create(m_pGraphicDev);
	pUnit->Get_sComponent()->m_pTransformCom->m_vInfo[Engine::INFO_POS] = _vPos + _vDir * 3.2f;
	pUnit->Get_sComponent()->m_pTransformCom->m_vInfo[Engine::INFO_POS].y += 3.f;
	//_vDir.y = -0.4f + m_LookAtY * 0.25f;
	//_vDir.y = -0.4f + 2.5f * 0.25f;
	_vDir.y = 0.f;

	D3DXVec3Normalize(&_vDir, &_vDir);
	pUnit->Get_sStat()->m_vDir = _vDir;

	pUnit->Engine::CGameObject::Update_Object(0.f);
	dynamic_cast<CPhoenix*>(pUnit)->Update_Object(0.f);
	pUnit->Get_sStat()->m_fAttack = m_sStat.m_fAttack * 0.4f;
	Engine::CLayer* pLayer = Engine::CManagement::GetInstance()->m_pScene->Get_Layer(Engine::CLayer::LayerName::Layer_Dynamic);
	pLayer->Add_GameObject(L"Phoenix_SkillQ", pGameObject);

	return;
}

void CPlayer::Move_AccelSpeed(_vec3 vPos, _vec3 vDir, float fTimeDelta, Engine::CNaviMesh* pNaviMeshCom)
{
	if (m_Attack_State == StateA_SkillZ) {
		m_AccelSpeed = 0.f;
		return;
	}
	if (m_AccelSpeed <= 0.f)
		return;

	m_AccelSpeed -= m_AccelSpeedInit * fTimeDelta * 3.f;
	
	_vec3	vDir2;
	if (m_sStat.m_EnumDir == EnumDir::Up) {
		m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_AccelSpeed), &m_sStat.m_dwNaviIndex));
	}
	else if (m_sStat.m_EnumDir == EnumDir::UpLeft) {
		vDir2 = { -vDir.z, 0.f, vDir.x };
		vDir2 += vDir;
		D3DXVec3Normalize(&vDir2, &vDir2);
		m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_AccelSpeed), &m_sStat.m_dwNaviIndex));
	}
	else if (m_sStat.m_EnumDir == EnumDir::UpRight) {
		vDir2 = { vDir.z, 0.f, -vDir.x };
		vDir2 += vDir;
		D3DXVec3Normalize(&vDir2, &vDir2);
		m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_AccelSpeed), &m_sStat.m_dwNaviIndex));
	}
	else if (m_sStat.m_EnumDir == EnumDir::Down) {
		m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_AccelSpeed), &m_sStat.m_dwNaviIndex));
	}
	else if (m_sStat.m_EnumDir == EnumDir::DownLeft) {
		vDir2 = { vDir.z, 0.f, -vDir.x };
		vDir2 += vDir;
		D3DXVec3Normalize(&vDir2, &vDir2);
		m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_AccelSpeed), &m_sStat.m_dwNaviIndex));
	}
	else if (m_sStat.m_EnumDir == EnumDir::DownRight) {
		vDir2 = { -vDir.z, 0.f, vDir.x };
		vDir2 += vDir;
		D3DXVec3Normalize(&vDir2, &vDir2);
		m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_AccelSpeed), &m_sStat.m_dwNaviIndex));
	}
	else if (m_sStat.m_EnumDir == EnumDir::Left) {
		vDir2 = { -vDir.z, 0.f, vDir.x };
		m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_AccelSpeed), &m_sStat.m_dwNaviIndex));
	}
	else if (m_sStat.m_EnumDir == EnumDir::Right) {
		vDir2 = { vDir.z, 0.f, -vDir.x };
		m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_AccelSpeed), &m_sStat.m_dwNaviIndex));
	}

	return;
}

void CPlayer::Talk_NpcToPortal()
{
	if (CNpcQuest_Manager::Get_NpcQuestInfo()->m_NpcColl && !CNpcQuest_Manager::Get_NpcQuestInfo()->m_PlayerTalk) {
		if ((Engine::Get_DIKeyState(DIK_T) & 0x80)) {
			CNpcQuest_Manager::Get_NpcQuestInfo()->m_PlayerTalk = true;
			CNpc_01* pNpc = dynamic_cast<CNpc_01*>(Engine::CManagement::GetInstance()->m_pScene->Get_LayerObject(Engine::CLayer::Layer_Dynamic, Engine::CGameObject::UnitName::Npc));
			if (pNpc == nullptr)
				return;
			pNpc->NpcQuest();
		}
		//dynamic_cast<CNpc_01*>(Engine::CManagement::GetInstance()->m_pScene->Get_LayerObject(Engine::CLayer::Layer_Dynamic, Engine::CGameObject::UnitName::Npc))->Talk_Rocate();
	}

	/*if (CNpcQuest_Manager::Get_NpcQuestInfo()->m_PlayerTalk) {
		if ((Engine::Get_DIKeyState(DIK_ESCAPE) & 0x80)) {
			CNpcQuest_Manager::Get_NpcQuestInfo()->m_PlayerTalk = false;
		}
	}*/

	if (CNpcQuest_Manager::Get_NpcQuestInfo()->m_PortalColl) {
		if ((Engine::Get_DIKeyState(DIK_T) & 0x80)) {
			CNpcQuest_Manager::Get_Instance()->Get_NpcQuestInfo()->m_MapMove = true;
			
		}
		//dynamic_cast<CNpc_01*>(Engine::CManagement::GetInstance()->m_pScene->Get_LayerObject(Engine::CLayer::Layer_Dynamic, Engine::CGameObject::UnitName::Npc))->Talk_Rocate();
	}
	return;
}

void CPlayer::Dissolve_WeaponIdle(float fTimeDelta)
{
	if (m_bCheck[bCheck_State_WeaponChange])
		return;
	if (m_State != State::State_Attack && m_State != State::State_Skill) {
		if (m_bCheck[bCheck::bCheck_WeaponChangeIdle]) {
			m_WeaponDissolve = 1.f;
			//m_WeaponDissolve += fTimeDelta * 10.f;
			//if (m_WeaponDissolve > 1.f) {
				//m_WeaponDissolve = 1.f;
				m_bCheck[bCheck::bCheck_WeaponChangeIdle] = false;
			//}
		}
		else {
			if (m_WeaponDissolve > 0.f)
				m_WeaponDissolve -= fTimeDelta * 0.4f;
			if (m_WeaponDissolve < 0.f) {
				m_WeaponDissolve = 0.f;
			}
		}
	}
}

void CPlayer::Voice_Random(RandomSound _voice)
{
	//Sound_Arrow_Hit, Sound_Arrow, Sound_BowTwang, Sound_Dash, Sound_GHAttack, Sound_GHHit, Sound_Jump, Sound_Phoenix, Sound_Run
	if (_voice == RandomSound::Sound_Run) {
		int voiceNumber = CRandom_Manager::Random() % 6 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"Run%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::PLAYER, VOLUME);
	}
	else if (_voice == RandomSound::Sound_AttackVoice) {
		int voiceNumber = CRandom_Manager::Random() % 14 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"AttackVoice%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::PLAYER, VOLUME);
	}
	else if (_voice == RandomSound::Sound_DamagedVoice) {
		int voiceNumber = CRandom_Manager::Random() % 4 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"Damaged%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::PLAYER, VOLUME);
	}
	else if (_voice == RandomSound::Sound_SkillVoice) {
		int voiceNumber = CRandom_Manager::Random() % 8 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"SkillVoice%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::PLAYER, VOLUME);
	}
	else if (_voice == RandomSound::Sound_Arrow_Hit) {
		int voiceNumber = CRandom_Manager::Random() % 6 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"Arrow_Hit%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::PLAYER, VOLUME);
	}
	else if (_voice == RandomSound::Sound_Arrow) {
		int voiceNumber = CRandom_Manager::Random() % 5 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"Arrow%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::PLAYER, VOLUME);
	}
	else if (_voice == RandomSound::Sound_BowTwang) {
		int voiceNumber = CRandom_Manager::Random() % 2 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"Bow_Twang%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::PLAYER, VOLUME);
	}
	else if (_voice == RandomSound::Sound_Dash) {
		int voiceNumber = CRandom_Manager::Random() % 4 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"Dash%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::PLAYER, VOLUME);
	}
	else if (_voice == RandomSound::Sound_GHAttack) {
		int voiceNumber = CRandom_Manager::Random() % 7 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"GH_Attack%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::PLAYER, VOLUME);
	}
	else if (_voice == RandomSound::Sound_GHHit) {
		int voiceNumber = CRandom_Manager::Random() % 7 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"GH_Hit%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::PLAYER, VOLUME);
	}
	else if (_voice == RandomSound::Sound_Jump) {
		int voiceNumber = CRandom_Manager::Random() % 4 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"Jump%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::PLAYER, VOLUME);
	}
	else if (_voice == RandomSound::Sound_Phoenix) {
		int voiceNumber = CRandom_Manager::Random() % 3 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"Phoenix%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::PLAYER, VOLUME);
	}

}