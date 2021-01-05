#include "stdafx.h"
#include "ApostleOfGreed.h"
#include "Export_Function.h"
#include "DynamicCamera.h"
#include "SphereCollider.h"
#include "Player.h"
#include "Stage.h"
#include "NaviTerrain.h"
#include "Random_Manager.h"

#define PLAYER_SEARCH_DISTANCE 20.f
#define PLAYER_ATTACK_DISTANCE 10.f
#define ApSkill_CoolTotal 3.f
#define ApSkill_Cool1 5.f
#define ApSkill_Cool3 5.f
#define ApSkill_Cool3_DisTance 15.f
#define ApSkill_Cool7 5.f
#define ApSkill_Cool7_DisTance 30.f
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
	m_fInitSpeed = 9.f;
	m_fSpeed = m_fInitSpeed;
	m_fMaxHp = 12000.f;
	m_fHp = m_fMaxHp;
	m_fMaxKnockBackHp = 1000.f;
	m_fKnockBackHp = m_fMaxKnockBackHp;
	m_fAttack = 100.f;
	m_dwNaviIndex = 0;
}

CApostleOfGreed::~CApostleOfGreed(void)
{

}

Client::_vec3 Client::CApostleOfGreed::PickUp_OnTerrain(void)
{
	Engine::CTerrainTex*		pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK_RETURN(pTerrainBufferCom, _vec3(0.f, 0.f, 0.f));

	Engine::CTransform*		pTerrainTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Transform", Engine::ID_DYNAMIC));
	NULL_CHECK_RETURN(pTerrainTransformCom, _vec3(0.f, 0.f, 0.f));

	return m_pCalculatorCom->Picking_OnTerrain(g_hWnd, pTerrainBufferCom, pTerrainTransformCom);
}

HRESULT Client::CApostleOfGreed::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	CMonster::Add_Component();
	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_M_ApostleOfGreed"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);
	m_pMeshCom->Set_AniAngle(95.f);
	//m_pMeshCom->Set_AniAngle(275.f);
	//
	m_pTransformCom->Set_Pos(&_vec3(72.f, 0.f, 72.f));
	//m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	Engine::CGameObject::Update_Object(1.f);
	
	//m_pMeshCom->Play_Animation(1.f);

	Load_ColliderFile(L"../Bin/Resource/Mesh/DynamicMesh/Save/ApostleOfGreed.dat");
	for (auto& sphere : m_VecSphereCollider)
	{
		sphere->m_pDynamicMesh = this;
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

	m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

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
	CMonster::Free();
}


HRESULT Client::CApostleOfGreed::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(0.018f, 0.018f, 0.018f);
	m_pMeshCom->Set_AnimationSet(25);

	return S_OK;
}

Client::_int Client::CApostleOfGreed::Update_Object(const _float& fTimeDelta)
{
	if (m_IsDead || m_fHp < 0.f)
		return 1;

	Calc_Time(fTimeDelta);

	SetUp_OnTerrain();
	Pattern(fTimeDelta);

	CMonster::Update_Object(fTimeDelta);
	m_pMeshCom->Play_Animation(fTimeDelta * m_AniSpeed);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}

Client::_int Client::CApostleOfGreed::LateUpdate_Object(const _float& fTimeDelta)
{

	

	return 0;
}

void Client::CApostleOfGreed::Render_Object(void)
{
	LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
	pEffect->BeginPass(0);

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	m_pMeshCom->Render_Meshes(pEffect);

	pEffect->EndPass();
	pEffect->End();

	//m_pNaviMeshCom->Render_NaviMeshes();


	Engine::Safe_Release(pEffect);
	/*_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pColliderCom->Render_Collider(Engine::COL_TRUE, &matWorld);*/
}

void Client::CApostleOfGreed::SetUp_OnTerrain(void)
{
	_vec3	vPosition;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	Engine::CTerrainTex*		pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPosition, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_pTransformCom->Move_Pos(vPosition.x, fHeight, vPosition.z);
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
		m_pMeshCom->Set_AnimationSet(25);
		m_AniSpeed = 1.f;
		Init_BoneAttack();
		break;
	case Client::CApostleOfGreed::State_Move:
		m_pMeshCom->Set_AnimationSet(28);
		m_fSpeed = m_fInitSpeed;
		m_AniSpeed = 1.f;
		break;
	case Client::CApostleOfGreed::State_Rocate:
		m_pMeshCom->Set_AnimationSet(24);
		m_AniSpeed = 1.f;
		break;
	case Client::CApostleOfGreed::State_Skill: {
		m_SkillState = _skill;
		if (m_SkillState == Skill_Ap_1) {
			m_SkillCool[SKill_Cool_Ap::SCool_Ap_1] = ApSkill_Cool1;
			m_pMeshCom->Set_AnimationSet(17);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.3f;
			Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, false);
		}
		else if (m_SkillState == Skill_Ap_3) {
			m_SkillCool[SKill_Cool_Ap::SCool_Ap_3] = ApSkill_Cool3;
			m_pMeshCom->Set_AnimationSet(14);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.3f;
			Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, false, 4.f);
			Set_BonePartColliderAttack(CSphereCollider::BonePart_RHand, false, 4.f);
		}
		else if (m_SkillState == Skill_Ap_7) {
			m_SkillCool[SKill_Cool_Ap::SCool_Ap_7] = ApSkill_Cool7;
			m_pMeshCom->Set_AnimationSet(7);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 0.8f;
		}
	}
		break;
	case Client::CApostleOfGreed::State_JumpEnd:
		break;
	case Client::CApostleOfGreed::State_KnockBack:
		m_pMeshCom->Set_AnimationSet(18);
		m_AniSpeed = 1.f;
		break;
	case Client::CApostleOfGreed::State_Groggy:
		m_pMeshCom->Set_AnimationSet(19);
		m_AniSpeed = 1.f;
		m_TimeGroggy = GROGGY;
		break;
	case Client::CApostleOfGreed::State_GroggyUp:
		m_pMeshCom->Set_AnimationSet(20);
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
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
	D3DXVec3Normalize(&vDir, &vDir);
	//뭐지?
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
	if (m_fKnockBackHp < 0.f) {
		m_fKnockBackHp = m_fMaxKnockBackHp;
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
		if (m_pMeshCom->Is_AnimationSetEnd(0.2f)) {
			Set_StateToAnimation(State::State_Groggy);
		}
	}
	else if (m_State == State::State_Groggy) {
		if (m_TimeGroggy < 0.f) {
			Set_StateToAnimation(State::State_GroggyUp);
		}
	}
	else if (m_State == State::State_GroggyUp) {
		if (m_pMeshCom->Is_AnimationSetEnd(0.2f)) {
			m_State = State::State_Wait;
			m_pMeshCom->Set_AnimationSet(25);
		}
	}
	else if (m_State == State::State_Skill) {
		Event_Skill(fTimeDelta, pNaviMeshCom, vPos, vDir, playerTodisTance);
	}
	else if (m_State == State::State_Move) {

		m_vDir = m_pPlayerTrans->m_vInfo[Engine::INFO_POS] - m_pTransformCom->m_vInfo[Engine::INFO_POS];
		Rocate_PlayerToWardAngle(fTimeDelta, 180.f);
		//_vec3 afterPos = m_pTransformCom->m_vInfo[Engine::INFO_POS] + (m_vDir * fTimeDelta * m_fSpeed);
		//m_pTransformCom->Set_Pos(&afterPos);
		m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

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

void CApostleOfGreed::Collision(CSphereCollider* _mySphere, Engine::CGameObject* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{
	if (_mySphere->m_BoneTeam == _colSphere->m_BoneTeam)
		return;

	if (_mySphere->m_BonePart == CSphereCollider::BonePart_CollBody) {
		if (_colSphere->m_BonePart == CSphereCollider::BonePart_PlayerHammer
			&& _colSphere->m_WeaponAttack) {
			if (!_colSphere->Check_DamagedObject(this)) {
				_colSphere->m_VecDamagedObject.emplace_back(this);
				m_fHp -= _col->m_fAttack;
				m_fKnockBackHp -= _col->m_fAttack;

			}
		}
	}
	
}

void CApostleOfGreed::Event_Skill(float fTimeDelta, Engine::CNaviMesh* pNaviMeshCom, _vec3 vPos, _vec3 vDir, float playerTodisTance) {
	if (m_SkillState == Skill_Ap::Skill_Ap_1) {
		if (m_pMeshCom->Is_AnimationSetEnd(0.15f)) {
			Set_StateToAnimation(State::State_Wait);
		}
		else {
			float trackPos = m_pMeshCom->Get_AnimationTrackPos();
			//m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

			if (trackPos > 1.6f) {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, true);
			}
			else {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, false);
			}
			return;
		}
	}
	else if (m_SkillState == Skill_Ap::Skill_Ap_3) {
		if (m_pMeshCom->Is_AnimationSetEnd(0.1f)) {
			Set_StateToAnimation(State::State_Wait);
		}
		else {
			float trackPos = m_pMeshCom->Get_AnimationTrackPos();
			//m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

			if (2.66f < trackPos && trackPos < 3.33f) {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, true, 4.f);
				Set_BonePartColliderAttack(CSphereCollider::BonePart_RHand, true, 4.f);
			}
			else {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, false, 4.f);
				Set_BonePartColliderAttack(CSphereCollider::BonePart_RHand, false, 4.f);
			}

			if (2.9f < trackPos && trackPos < 3.1f) {
				m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * 30.f), &m_dwNaviIndex));
			}
			return;
		}
	}
	else if (m_SkillState == Skill_Ap::Skill_Ap_7) {
		if (m_pMeshCom->Is_AnimationSetEnd(0.1f)) {
			Set_StateToAnimation(State::State_Wait);
		}
		else {
			float trackPos = m_pMeshCom->Get_AnimationTrackPos();
			if (0.16f < trackPos && trackPos < 0.76f) {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_BodyWeapon, true, 3.f);
				m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_PlayerDistanceSave * fTimeDelta * 1.4f), &m_dwNaviIndex));
			}
			else {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_BodyWeapon, false);
			}
		}
	}
}

bool CApostleOfGreed::Random_Skill(float playerTodisTance) {
	if (m_SkillCool[SKill_Cool_Ap::SCool_Ap_Total] > 0.f)
		return false;
	int iPattern = CRandom_Manager::Random() % 3;
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
			m_PlayerDistanceSave = m_pPlayerTrans->m_vInfo[Engine::INFO_POS] - m_pTransformCom->m_vInfo[Engine::INFO_POS];
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