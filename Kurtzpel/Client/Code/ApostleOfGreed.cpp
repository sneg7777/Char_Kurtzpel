#include "stdafx.h"
#include "ApostleOfGreed.h"
#include "Export_Function.h"
#include "DynamicCamera.h"
#include "SphereCollider.h"
#include "Player.h"
#include "Stage.h"
#include "NaviTerrain.h"
#include "Random_Manager.h"
#include "UI_Manager.h"
#include "NpcQuest_Manager.h"
#include "SkillCollider.h"
#include "EffectRcTex.h"
#include "SoundManager.h"

#define PLAYER_SEARCH_DISTANCE 20.f
#define PLAYER_ATTACK_DISTANCE 10.f
#define ApSkill_CoolTotal 3.f
#define ApSkill_Cool1 12.f
#define ApSkill_Cool3 12.f
#define ApSkill_Cool3_DisTance 15.f
#define ApSkill_Cool5 12.f
#define ApSkill_Cool5_DisTance 15.f
#define ApSkill_Cool7 12.f
#define ApSkill_Cool7_DisTance 30.f
#define ApSkill_Cool10 12.f
CApostleOfGreed::CApostleOfGreed(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
	for (int i = 0; i < bCheck::bCheck_End; i++)
	{
		m_bCheck[i] = false;
	}
	for (int i = 0; i < TimeCheck::TimeCheck_End; i++)
	{
		m_TimeCheck[i] = 0.f;
	}
	for (int i = 0; i < SKill_Cool_Ap::SCool_Ap_End; i++)
	{
		m_SkillCool[i] = 0.f;
	}
	m_sStat.m_fInitSpeed = 9.f;
	m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
	m_sStat.m_fDelayHp = m_sStat.m_fMaxDelayHp = m_sStat.m_fHp = m_sStat.m_fMaxHp = 20000.f;
	m_sStat.m_fKnockBackHp =  m_sStat.m_fMaxKnockBackHp = 4000.f;
	m_sStat.m_fAttack = 100.f;
	m_sStat.m_dwNaviIndex = 0;
}

CApostleOfGreed::~CApostleOfGreed(void)
{

}

HRESULT Client::CApostleOfGreed::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	CMonster::Add_Component();
	// Mesh
	pComponent = m_sComponent.m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_M_ApostleOfGreed"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);
	m_sComponent.m_pMeshCom->Set_AniAngle(95.f);
	//m_pMeshCom->Set_AniAngle(275.f);
	//
	m_sComponent.m_pTransformCom->Set_Pos(&_vec3(72.f, 0.f, 72.f));
	//m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	Engine::CGameObject::Update_Object(0.f);
	
	//m_pMeshCom->Play_Animation(1.f);
	m_UnitName = UnitName::Monster;
	Load_ColliderFile(L"../Bin/Resource/Mesh/DynamicMesh/Save/ApostleOfGreed.dat");
	for (auto& sphere : m_VecSphereCollider)
	{
		sphere->m_pDynamicThis = this;
		if (!sphere->m_FrameName.compare("Bip001")) {
			sphere->m_BonePart = CSphereCollider::BonePart_CollBody;
		}
		else if (!sphere->m_FrameName.compare("weapon")) {
			sphere->m_BonePart = CSphereCollider::BonePart_Weapon;
		}
		else if (!sphere->m_FrameName.compare("Bip001-L-Hand")) {
			sphere->m_BonePart = CSphereCollider::BonePart_RHand;
		}
		else if (!sphere->m_FrameName.compare("Bip001-Spine")) {
			sphere->m_BonePart = CSphereCollider::BonePart_BodyWeapon;
		}
		sphere->m_BoneTeam = CSphereCollider::BoneTeam_Enemy;
	}
	//m_pTransformCom->Set_Pos(&_vec3(52.f, 0.f, 52.f));
	return S_OK;
}

HRESULT CApostleOfGreed::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_sComponent.m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	_vec4 vColor = { 0.f, 0.f, 0.f, 1.f };
	pEffect->SetVector("g_vColor", &vColor);
	pEffect->SetFloat("g_fBoldSize", 0.005f);

	return S_OK;
}


CApostleOfGreed* CApostleOfGreed::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CApostleOfGreed*	pInstance = new CApostleOfGreed(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);
	return pInstance;
}

void CApostleOfGreed::Free(void)
{
	for (auto& hpdec : m_VecDelayHpDec) 
		Engine::Safe_Delete(hpdec);
	m_VecDelayHpDec.clear();

	CMonster::Free();
}


HRESULT Client::CApostleOfGreed::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_sComponent.m_pTransformCom->Set_Scale(0.018f, 0.018f, 0.018f);
	m_sComponent.m_pMeshCom->Set_AnimationSet(25);

	return S_OK;
}

Client::_int Client::CApostleOfGreed::Update_Object(const _float& fTimeDelta)
{
	if (m_sStat.m_IsDead || m_sStat.m_fHp < 0.f) {
		CNpcQuest_Manager::Get_NpcQuestInfo()->m_DeadApostle++;
		return 1;
	}
	
	Calc_Time(fTimeDelta);

	SetUp_OnTerrain();
	Pattern(fTimeDelta);
	Update_DelayHpDec(fTimeDelta);

	CMonster::Update_Object(fTimeDelta);
	m_sComponent.m_pMeshCom->Play_Animation(fTimeDelta * m_AniSpeed);

	m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}

Client::_int Client::CApostleOfGreed::LateUpdate_Object(const _float& fTimeDelta)
{

	

	return 0;
}

void Client::CApostleOfGreed::Render_Object(void)
{
	CUnit_D::Render_Object();
}

void Client::CApostleOfGreed::SetUp_OnTerrain(void)
{
	_vec3	vPosition;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_Terrain->Get_sComponent_Terrain()->m_pBufferCom;
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_sComponent.m_pCalculatorCom->Compute_HeightOnTerrain(&vPosition, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_sComponent.m_pTransformCom->Move_Pos(vPosition.x, fHeight, vPosition.z);
}


void Client::CApostleOfGreed::Calc_Time(_float fTimeDelta)
{
	if (m_TimeGroggy > 0.f) {
		m_TimeGroggy -= fTimeDelta;
	}
	for (int i = 0; i < SKill_Cool_Ap::SCool_Ap_End; i++)
	{
		if (m_SkillCool[i] > 0.f)
			m_SkillCool[i] -= fTimeDelta;
	}
}

void Client::CApostleOfGreed::Init_BoneAttack() {
	for (auto& bone : m_VecSphereCollider)
	{
		bone->m_WeaponAttack = false;
	}
}

void Client::CApostleOfGreed::Set_StateToAnimation(State _state, Skill_Ap _skill) {
	switch (_state)
	{
	case Client::CApostleOfGreed::State_Wait:
		m_sComponent.m_pMeshCom->Set_AnimationSet(25);
		m_AniSpeed = 1.f;
		Init_BoneAttack();
		break;
	case Client::CApostleOfGreed::State_Move:
		m_sComponent.m_pMeshCom->Set_AnimationSet(28);
		m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
		m_AniSpeed = 1.f;
		break;
	case Client::CApostleOfGreed::State_Rocate:
		m_sComponent.m_pMeshCom->Set_AnimationSet(24);
		m_AniSpeed = 1.f;
		break;
	case Client::CApostleOfGreed::State_Skill: {
		m_SkillState = _skill;
		m_bCheck[bCheck_Sound1] = false;
		ApSound_RandomPlay(ApSound::ApSound_AttackVoice);
		if (m_SkillState == Skill_Ap_1) {
			m_SkillCool[SKill_Cool_Ap::SCool_Ap_1] = ApSkill_Cool1;
			m_sComponent.m_pMeshCom->Set_AnimationSet(17);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.3f;
			Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, m_sStat.m_fAttack, false);
		}
		else if (m_SkillState == Skill_Ap_3) {
			m_SkillCool[SKill_Cool_Ap::SCool_Ap_3] = ApSkill_Cool3;
			m_sComponent.m_pMeshCom->Set_AnimationSet(14);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.3f;
			Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, m_sStat.m_fAttack, false, 4.f);
			Set_BonePartColliderAttack(CSphereCollider::BonePart_RHand, m_sStat.m_fAttack, false, 4.f);
		}
		else if (m_SkillState == Skill_Ap_5) {
			m_SkillCool[SKill_Cool_Ap::SCool_Ap_5] = ApSkill_Cool5;
			m_sComponent.m_pMeshCom->Set_AnimationSet(12);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.1f;
		}
		else if (m_SkillState == Skill_Ap_7) {
			m_SkillCool[SKill_Cool_Ap::SCool_Ap_7] = ApSkill_Cool7;
			m_sComponent.m_pMeshCom->Set_AnimationSet(7);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 0.8f;
		}
		else if (m_SkillState == Skill_Ap_10) {
			m_SkillCool[SKill_Cool_Ap::SCool_Ap_10] = ApSkill_Cool10;
			m_sComponent.m_pMeshCom->Set_AnimationSet(0);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.f;
			m_SkillPosSave = { 0.f, 0.f, 0.f };
		}
	}
		break;
	case Client::CApostleOfGreed::State_JumpEnd:
		break;
	case Client::CApostleOfGreed::State_KnockBack:
		m_sComponent.m_pMeshCom->Set_AnimationSet(18);
		ApSound_RandomPlay(ApSound::ApSound_Groggy, 1.4f);
		m_AniSpeed = 1.f;
		break;
	case Client::CApostleOfGreed::State_Groggy:
		m_sComponent.m_pMeshCom->Set_AnimationSet(19);
		m_AniSpeed = 1.f;
		m_TimeGroggy = GROGGY;
		break;
	case Client::CApostleOfGreed::State_GroggyUp:
		ApSound_RandomPlay(ApSound::ApSound_GroggyUp, 1.4f);
		m_sComponent.m_pMeshCom->Set_AnimationSet(20);
		m_AniSpeed = 1.f;
		break;
	case Client::CApostleOfGreed::State_End:
		break;
	default:
		break;
	}
	m_State = _state;
}

void Client::CApostleOfGreed::Pattern(_float fTimeDelta)
{
	//////////////////////////////////////////////////////////////////////////////////////
	Engine::CNaviMesh* pNaviMeshCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_NaviTerrain->m_pNaviMeshCom;
	float playerTodisTance = PlayerSearchDistance();
	CPlayer* player = CPlayer::GetInstance();
	Set_PlayerTowardAngle();
	_vec3	vPos, vDir;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
	D3DXVec3Normalize(&vDir, &vDir);
	//����?
	vDir *= -1.f;
	//////////////////////////////////////////////////////////////////////////////////////
	if (m_isSearch && m_State == State::State_Wait || m_State == State::State_Move || m_State == State::State_Rocate) {
		bool state_check = Random_Skill(playerTodisTance);
		if (state_check) {
			m_State = State::State_Skill;
			m_SkillCool[SKill_Cool_Ap::SCool_Ap_Total] = ApSkill_CoolTotal;
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////
	if (m_sStat.m_fKnockBackHp <= 0.f && (m_State == State::State_Wait || m_State == State::State_Move || m_State == State::State_Rocate)) {
		m_sStat.m_fKnockBackHp = 0.f;
		Set_StateToAnimation(State::State_KnockBack);
	}
	if (!m_isSearch) {
		if (playerTodisTance < PLAYER_SEARCH_DISTANCE) {
			m_isSearch = true;
			Set_StateToAnimation(State::State_Wait);
		}
		return;
	}
	//////////////////////////////////////////////////////////////////////////////////////

	if (m_State == State::State_KnockBack) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.2f)) {
			Set_StateToAnimation(State::State_Groggy);
		}
	}
	else if (m_State == State::State_Groggy) {
		if (m_TimeGroggy < 0.f) {
			Set_StateToAnimation(State::State_GroggyUp);
		}
	}
	else if (m_State == State::State_GroggyUp) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.2f)) {
			m_sStat.m_fKnockBackHp = m_sStat.m_fMaxKnockBackHp;
			m_State = State::State_Wait;
			m_sComponent.m_pMeshCom->Set_AnimationSet(25);
		}
	}
	else if (m_State == State::State_Skill) {
		Event_Skill(fTimeDelta, pNaviMeshCom, vPos, vDir, playerTodisTance);
	}
	else if (m_State == State::State_Move) {

		m_sStat.m_vDir = m_pPlayerTrans->m_vInfo[Engine::INFO_POS] - m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS];
		Rocate_PlayerToWardAngle(fTimeDelta, 180.f);
		//_vec3 afterPos = m_pTransformCom->m_vInfo[Engine::INFO_POS] + (m_sStat.m_vDir * fTimeDelta * m_sStat.m_fSpeed);
		//m_pTransformCom->Set_Pos(&afterPos);
		m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

		Set_StateToAnimation(State::State_Move);
		//
	}
	else if (m_State == State::State_Rocate) {
		Set_StateToAnimation(State::State_Rocate);
		Rocate_PlayerToWardAngle(fTimeDelta, 180.f);
	}

	if (m_State == State::State_Wait || m_State == State::State_Move || m_State == State::State_Rocate){
		if (playerTodisTance < PLAYER_SEARCH_DISTANCE) {
			if (playerTodisTance < PLAYER_ATTACK_DISTANCE) {
				if (!m_AngleOfSame) {
					Set_StateToAnimation(State::State_Rocate);
				}
			}
			else {
				Set_StateToAnimation(State::State_Move);
			}
		}
		else {
			Set_StateToAnimation(State::State_Move);
		}
	}
}

void CApostleOfGreed::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{
	if (_mySphere->m_BoneTeam == _colSphere->m_BoneTeam)
		return;

	if (_mySphere->m_BonePart == CSphereCollider::BonePart_CollBody) {
		if (/*_colSphere->m_BonePart == CSphereCollider::BonePart_PlayerHammer
			&& */_colSphere->m_WeaponAttack) {
			if (!_colSphere->Check_DamagedObject(this)) {
				_colSphere->m_VecDamagedObject.emplace_back(this);
				m_sStat.m_fHp -= _col->Get_sStat()->m_fAttack;
				Sound_RandomPlay(UnitSound::Sound_MonDamaged);
				Emplace_DelayHpDec(_col->Get_sStat()->m_fAttack);
				if (m_sStat.m_fKnockBackHp >= 0.f) {
					m_sStat.m_fKnockBackHp -= _col->Get_sStat()->m_fAttack;
					if (m_sStat.m_fKnockBackHp < 0.f)
						m_sStat.m_fKnockBackHp = 0.f;
				}
				
				m_isSearch = true;
				CUI_Manager::Get_Instance()->Set_DamagedTime(TIME_ENEMYHPBAR);
				if (m_sStat.m_fHp > 0.f)
					CUI_Manager::Get_Instance()->Set_DamagedEnemy(this);
				else
					CUI_Manager::Get_Instance()->Set_DamagedEnemy(nullptr);
				Effect_Damaged(_col, _colSphere);
			}
		}
	}
	
}

void CApostleOfGreed::Event_Skill(float fTimeDelta, Engine::CNaviMesh* pNaviMeshCom, _vec3 vPos, _vec3 vDir, float playerTodisTance) {
	if (m_SkillState == Skill_Ap::Skill_Ap_1) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.15f)) {
			Set_StateToAnimation(State::State_Wait);
		}
		else {
			float trackPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();
			//m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			if (trackPos > 1.6f) {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, m_sStat.m_fAttack, true);
			}
			else {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, m_sStat.m_fAttack, false);
			}
			if (!m_bCheck[bCheck_Sound1] && trackPos > 2.f) {
				ApSound_RandomPlay(ApSound::ApSound_Skill01, 2.f);
				m_bCheck[bCheck_Sound1] = true;
				dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera()->Set_ShakeTime(0.7f);
			}
			return;
		}
	}
	else if (m_SkillState == Skill_Ap::Skill_Ap_3) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.1f)) {
			Set_StateToAnimation(State::State_Wait);
		}
		else {
			float trackPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();
			//m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			if (2.66f < trackPos && trackPos < 3.33f) {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, m_sStat.m_fAttack, true, 4.f);
				Set_BonePartColliderAttack(CSphereCollider::BonePart_RHand, m_sStat.m_fAttack, true, 4.f);
			}
			else {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, false, 4.f);
				Set_BonePartColliderAttack(CSphereCollider::BonePart_RHand, false, 4.f);
			}

			if (2.9f < trackPos && trackPos < 3.1f) {
				m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * 30.f), &m_sStat.m_dwNaviIndex));
			}
			if (!m_bCheck[bCheck_Sound1] && trackPos > 3.f) {
				ApSound_RandomPlay(ApSound::ApSound_Skill03, 2.f);
				m_bCheck[bCheck_Sound1] = true;
			}
			return;
		}
	}
	else if (m_SkillState == Skill_Ap::Skill_Ap_5) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.1f)) {
			Set_StateToAnimation(State::State_Wait);
		}
		else {
			float trackPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();

			if (!m_bCheck[bCheck_Sound1] && trackPos > 0.4f) {
				ApSound_RandomPlay(ApSound::ApSound_Circle, 2.f);
				m_bCheck[bCheck_Sound1] = true;
				dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera()->Set_ShakeTime(0.3f);
				CEffectRcTex::Create(m_pGraphicDev)->Set_Effect(false, vPos, 10.f, L"Texture_Effect_MagicCircle", 1, 1, 1.f, 90.f);// ->Get_sComponent()->m_pTransformCom->Set_Scale(5.f, 20.f, 5.f);
			}
			return;
		}
	}
	else if (m_SkillState == Skill_Ap::Skill_Ap_7) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.1f)) {
			Set_StateToAnimation(State::State_Wait);
		}
		else {
			float trackPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();
			if (0.16f < trackPos && trackPos < 0.76f) {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_BodyWeapon, m_sStat.m_fAttack, true, 3.f);
				m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_PlayerDistanceSave * fTimeDelta * 1.4f), &m_sStat.m_dwNaviIndex));
			}
			else {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_BodyWeapon, m_sStat.m_fAttack, false);
			}
			if (0.72f < trackPos && trackPos < 0.8f) {
				dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera()->Set_ShakeTime(1.2f);
			}
			if (!m_bCheck[bCheck_Sound1] && trackPos >0.8f) {
				ApSound_RandomPlay(ApSound::ApSound_Skill07);
				m_bCheck[bCheck_Sound1] = true;
			}
		}

	}
	else if (m_SkillState == Skill_Ap::Skill_Ap_10) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.1f)) {
			Set_StateToAnimation(State::State_Wait);
		}
		if (m_SkillCool[SKill_Cool_Ap::SCool_Ap_10_2] <= 0.f) {
			m_SkillCool[SKill_Cool_Ap::SCool_Ap_10_2] = 0.5f;
			
			_vec3 vPlayerPos;
			m_pPlayerTrans->Get_Info(Engine::INFO_POS, &vPlayerPos);
			vPlayerPos.y = 1.5f;
			CEffectRcTex::Create(m_pGraphicDev)->Set_Effect(false, vPlayerPos, 7.f, L"Texture_Effect_Circle01", 4, 4, 15.f, 90.f, 0.f, 0.f);
			ApSound_RandomPlay(ApSound::ApSound_Circle, 1.5f);
			if (m_SkillPosSave.x != 0.f) {
				CEffectRcTex::Create(m_pGraphicDev)->Set_Effect(true, m_SkillPosSave, 10.f, L"Texture_Effect_Thunder", 4, 1, 12.f)->Get_sComponent()->m_pTransformCom->Set_Scale(5.f, 20.f, 5.f);
				dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_DynamicCamera()->Set_ShakeTime(0.2f);
				ApSound_RandomPlay(ApSound::ApSound_Thunder);
				CSkillCollider::Create(m_pGraphicDev)->Set_Collider(m_SkillPosSave, 3.f, CSphereCollider::BoneTeam_Enemy, m_sStat.m_fAttack * 1.5f, 0.5f, 0.f, 0.f, 0.7f);
			}
			m_SkillPosSave = vPlayerPos;
		}
	}
}

bool CApostleOfGreed::Random_Skill(float playerTodisTance) {
	if (m_SkillCool[SKill_Cool_Ap::SCool_Ap_Total] > 0.f)
		return false;
	int iPattern = CRandom_Manager::Random() % 4;
	//int iPattern = 3;
	switch (iPattern)
	{
	case 0: {
		if (m_SkillCool[SKill_Cool_Ap::SCool_Ap_1] <= 0.f && playerTodisTance < PLAYER_ATTACK_DISTANCE && m_AngleOfSame) {
			Set_StateToAnimation(State::State_Skill, Skill_Ap::Skill_Ap_1);
			return true;
		}
		else {
			return false;
		}
		break;
	}
	case 1: {
		if (m_SkillCool[SKill_Cool_Ap::SCool_Ap_3] <= 0.f && playerTodisTance < ApSkill_Cool3_DisTance && m_AngleOfSame) {
			Set_StateToAnimation(State::State_Skill, Skill_Ap::Skill_Ap_3);
			return true;
		}
		else {
			return false;
		}
		break;
	}
	case 2: {
		if (m_SkillCool[SKill_Cool_Ap::SCool_Ap_7] <= 0.f && PLAYER_SEARCH_DISTANCE < playerTodisTance && playerTodisTance < ApSkill_Cool7_DisTance && m_AngleOfSame) {
			Set_StateToAnimation(State::State_Skill, Skill_Ap::Skill_Ap_7);
			m_PlayerDistanceSave = m_pPlayerTrans->m_vInfo[Engine::INFO_POS] - m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS];
			return true;
		}
		else {
			return false;
		}
		break;
	}
	case 3: {
		if (m_SkillCool[SKill_Cool_Ap::SCool_Ap_10] <= 0.f) {
			Set_StateToAnimation(State::State_Skill, Skill_Ap::Skill_Ap_10);
			//m_PlayerDistanceSave = m_pPlayerTrans->m_vInfo[Engine::INFO_POS] - m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS];
			return true;
		}
		else {
			return false;
		}
		break;
	}
	case 4: {
		if (m_SkillCool[SKill_Cool_Ap::SCool_Ap_5] <= 0.f && playerTodisTance < PLAYER_ATTACK_DISTANCE && m_AngleOfSame) {
			Set_StateToAnimation(State::State_Skill, Skill_Ap::Skill_Ap_5);
			m_PlayerDistanceSave = m_pPlayerTrans->m_vInfo[Engine::INFO_POS] - m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS];
			return true;
		}
		else {
			return false;
		}
		break;
	}
	default:
		break;
	}
	return false;
}

void CApostleOfGreed::Emplace_DelayHpDec(float _attack) {
	sDelayHpDec* pDelayHpDec = new sDelayHpDec;
	pDelayHpDec->m_fDelayTime = 2.f;
	pDelayHpDec->m_fHpDec = _attack;
	m_VecDelayHpDec.emplace_back(pDelayHpDec);
	return;
}

void CApostleOfGreed::Update_DelayHpDec(float fTimeDelta) {
	for (auto& iter = m_VecDelayHpDec.begin(); iter!= m_VecDelayHpDec.end();)
	{
		(*iter)->m_fDelayTime -= fTimeDelta;
		if ((*iter)->m_fDelayTime < 0.f)
		{
			m_sStat.m_fMaxDelayHp -= (*iter)->m_fHpDec;
			m_sStat.m_fDelayHpSpeed = m_sStat.m_fDelayHp - m_sStat.m_fMaxDelayHp;
			Safe_Delete(*iter);
			iter = m_VecDelayHpDec.erase(iter);
		}
		else
		{
			iter++;
		}
	}

	if (m_sStat.m_fMaxDelayHp < m_sStat.m_fDelayHp)
	{
		m_sStat.m_fDelayHp -= fTimeDelta * m_sStat.m_fDelayHpSpeed;
		if (m_sStat.m_fMaxDelayHp > m_sStat.m_fDelayHp)
			m_sStat.m_fDelayHp = m_sStat.m_fMaxDelayHp;
	}
	return;
}

void CApostleOfGreed::ApSound_RandomPlay(ApSound _voice, float _addVolume)
{
	if (_voice == ApSound::ApSound_AttackVoice) {
		int voiceNumber = CRandom_Manager::Random() % 7 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"Ap_AttackVoice%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::MONSTER, VOLUME_ETC * _addVolume);
	}
	else if (_voice == ApSound::ApSound_Dead) {
		SoundManager::PlayOverlapSound(L"Ap_Dead.ogg", SoundChannel::MONSTER, VOLUME_ETC * _addVolume);
	}
	else if (_voice == ApSound::ApSound_Groggy) {
		SoundManager::PlayOverlapSound(L"Ap_Groggy.ogg", SoundChannel::MONSTER, VOLUME_ETC * _addVolume);
	}
	else if (_voice == ApSound::ApSound_GroggyUp) {
		int voiceNumber = CRandom_Manager::Random() % 3 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"Ap_GroggyUp%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::MONSTER, VOLUME_ETC * _addVolume);
	}
	else if (_voice == ApSound::ApSound_Skill01 || _voice == ApSound::ApSound_Skill05) {
		int voiceNumber = CRandom_Manager::Random() % 2 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"Ap_Skill01_%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::MONSTER, VOLUME_ETC * _addVolume);
	}
	else if (_voice == ApSound::ApSound_Skill03) {
		SoundManager::PlayOverlapSound(L"Ap_Skill03.ogg", SoundChannel::MONSTER, VOLUME_ETC * _addVolume);
	}
	else if (_voice == ApSound::ApSound_Skill07) {
		int voiceNumber = CRandom_Manager::Random() % 5 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"Ap_Skill07_%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::MONSTER, VOLUME_ETC * _addVolume);
	}
	else if (_voice == ApSound::ApSound_Start) {
		SoundManager::PlayOverlapSound(L"Ap_Start.ogg", SoundChannel::MONSTER, VOLUME_ETC * _addVolume);
	}
	else if (_voice == ApSound::ApSound_Circle) {
		int voiceNumber = CRandom_Manager::Random() % 4 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"Circle%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::MONSTER, VOLUME_ETC * _addVolume);
	}
	else if (_voice == ApSound::ApSound_Thunder) {
		int voiceNumber = CRandom_Manager::Random() % 8 + 1;
		_tchar		szFileName[256] = L"";

		wsprintf(szFileName, L"Thunder%d.ogg", voiceNumber);
		SoundManager::PlayOverlapSound(szFileName, SoundChannel::MONSTER, VOLUME_ETC * _addVolume);
	}
}
