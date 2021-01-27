#include "stdafx.h"
#include "Monster1_TwoHand.h"
#include "Export_Function.h"
#include "DynamicCamera.h"
#include "SphereCollider.h"
#include "Player.h"
#include "Stage.h"
#include "NaviTerrain.h"
#include "Random_Manager.h"
#include "UI_Manager.h"
#include "NpcQuest_Manager.h"

#define PLAYER_SEARCH_DISTANCE 14.f
#define PLAYER_ATTACK_DISTANCE 6.f
#define THSkill_CoolTotal 3.f
#define THSkill_Cool1 1.f
#define THSkill_Cool2 1.f
#define THSkill_Cool2_DisTance 15.f
#define THSkill_Cool3 1.f
#define THSkill_Cool3_DisTance 30.f
CMonster1_TwoHand::CMonster1_TwoHand(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
	for (int i = 0; i < SKill_Cool_TH::SCool_TH_End; i++)
	{
		m_SkillCool[i] = 0.f;
	}
	m_sStat.m_fInitSpeed = 9.f;
	m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
	m_sStat.m_fDelayHp = m_sStat.m_fMaxDelayHp = m_sStat.m_fHp = m_sStat.m_fMaxHp = 5000.f;
	m_sStat.m_fKnockBackHp =  m_sStat.m_fMaxKnockBackHp = 3000.f;
	m_sStat.m_fAttack = 50.f;
	m_sStat.m_dwNaviIndex = 0;
}

CMonster1_TwoHand::~CMonster1_TwoHand(void)
{

}

HRESULT Client::CMonster1_TwoHand::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	CMonster::Add_Component();
	// Mesh
	pComponent = m_sComponent.m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_M_Monster1_TwoHand"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);
	m_sComponent.m_pMeshCom->Set_AniAngle(0.f);
	//m_pMeshCom->Set_AniAngle(275.f);
	//
	m_sComponent.m_pTransformCom->Set_Pos(&_vec3(72.f, 0.f, 72.f));
	//m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	Engine::CGameObject::Update_Object(0.f);
	
	//m_pMeshCom->Play_Animation(1.f);

	Load_ColliderFile(L"../Bin/Resource/Mesh/DynamicMesh/Save/Monster1_TwoHand.dat");
	for (auto& sphere : m_VecSphereCollider)
	{
		sphere->m_pDynamicThis = this;
		if (!sphere->m_FrameName.compare("Bip001")) {
			sphere->m_BonePart = CSphereCollider::BonePart_CollBody;
		}
		else if (!sphere->m_FrameName.compare("Point_Weapon_R_Hand")) {
			sphere->m_BonePart = CSphereCollider::BonePart_Weapon;
		}
		sphere->m_BoneTeam = CSphereCollider::BoneTeam_Enemy;
	}
	//m_pTransformCom->Set_Pos(&_vec3(52.f, 0.f, 52.f));
	return S_OK;
}

HRESULT CMonster1_TwoHand::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_sComponent.m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	return S_OK;
}


CMonster1_TwoHand* CMonster1_TwoHand::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMonster1_TwoHand*	pInstance = new CMonster1_TwoHand(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);
	return pInstance;
}

void CMonster1_TwoHand::Free(void)
{
	for (auto& hpdec : m_VecDelayHpDec) 
		Engine::Safe_Delete(hpdec);
	m_VecDelayHpDec.clear();

	CMonster::Free();
}


HRESULT Client::CMonster1_TwoHand::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_sComponent.m_pTransformCom->Set_Scale(0.018f, 0.018f, 0.018f);
	Set_StateToAnimation(State::State_Respawn);

	return S_OK;
}

Client::_int Client::CMonster1_TwoHand::Update_Object(const _float& fTimeDelta)
{
	if (m_sStat.m_IsDead || m_sStat.m_IsDeadTime > 2.f) {
		CNpcQuest_Manager::Get_NpcQuestInfo()->m_DeadTwoHand++;
		return 1;
	}

	Calc_Time(fTimeDelta);

	SetUp_OnTerrain();
	Pattern(fTimeDelta);
	Update_DelayHpDec(fTimeDelta);

	m_sComponent.m_pMeshCom->Play_Animation(fTimeDelta * m_AniSpeed);
	CMonster::Update_Object(fTimeDelta);

	m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}

Client::_int Client::CMonster1_TwoHand::LateUpdate_Object(const _float& fTimeDelta)
{

	

	return 0;
}

void Client::CMonster1_TwoHand::Render_Object(void)
{
	//m_sComponent.m_pMeshCom->Play_Animation(0.f);
	m_sComponent.m_pMeshCom->Play_Animation(0.f);
	LPD3DXEFFECT	 pEffect = m_sComponent.m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
	pEffect->BeginPass(0);

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	m_sComponent.m_pMeshCom->Render_Meshes(pEffect);

	pEffect->EndPass();
	pEffect->End();

	//m_pNaviMeshCom->Render_NaviMeshes();


	Engine::Safe_Release(pEffect);
	/*_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pColliderCom->Render_Collider(Engine::COL_TRUE, &matWorld);*/
}

void Client::CMonster1_TwoHand::SetUp_OnTerrain(void)
{
	_vec3	vPosition;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_Terrain->Get_sComponent_Terrain()->m_pBufferCom;
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_sComponent.m_pCalculatorCom->Compute_HeightOnTerrain(&vPosition, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_sComponent.m_pTransformCom->Move_Pos(vPosition.x, fHeight, vPosition.z);
}


void Client::CMonster1_TwoHand::Calc_Time(_float fTimeDelta)
{
	if (m_TimeGroggy > 0.f) {
		m_TimeGroggy -= fTimeDelta;
	}
	for (int i = 0; i < SKill_Cool_TH::SCool_TH_End; i++)
	{
		if (m_SkillCool[i] > 0.f)
			m_SkillCool[i] -= fTimeDelta;
	}
}

void Client::CMonster1_TwoHand::Init_BoneAttack() {
	for (auto& bone : m_VecSphereCollider)
	{
		bone->m_WeaponAttack = false;
	}
}

void Client::CMonster1_TwoHand::Set_StateToAnimation(State _state, Skill_TH _skill) {
	switch (_state)
	{
	case Client::CMonster1_TwoHand::State_Respawn:
		m_sComponent.m_pMeshCom->Set_AnimationSet(63);
		m_AniSpeed = 1.1f;
		break;
	case Client::CMonster1_TwoHand::State_Dead:
		m_sComponent.m_pMeshCom->Set_AnimationSet(27);
		m_AniSpeed = 1.f;
		break;
	case Client::CMonster1_TwoHand::State_DeadDown:
		m_sComponent.m_pMeshCom->Set_AnimationSet(50);
		break;
	case Client::CMonster1_TwoHand::State_Wait:
		m_sComponent.m_pMeshCom->Set_AnimationSet(59);
		m_AniSpeed = 1.f;
		Init_BoneAttack();
		break;
	case Client::CMonster1_TwoHand::State_Move:
		m_sComponent.m_pMeshCom->Set_AnimationSet(62);
		m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
		m_AniSpeed = 1.f;
		break;
	case Client::CMonster1_TwoHand::State_Rocate:
		m_sComponent.m_pMeshCom->Set_AnimationSet(57);
		m_AniSpeed = 1.f;
		break;
	case Client::CMonster1_TwoHand::State_Skill: {
		m_SkillState = _skill;
		if (m_SkillState == Skill_TH_1) {
			m_SkillCool[SKill_Cool_TH::SCool_TH_1] = THSkill_Cool1;
			m_sComponent.m_pMeshCom->Set_AnimationSet(0);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.3f;
			Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, m_sStat.m_fAttack, true);
		}
		else if (m_SkillState == Skill_TH_2) {
			m_SkillCool[SKill_Cool_TH::SCool_TH_2] = THSkill_Cool2;
			m_sComponent.m_pMeshCom->Set_AnimationSet(1);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.3f;
			Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, m_sStat.m_fAttack, true);
		}
		else if (m_SkillState == Skill_TH_3) {
			m_SkillCool[SKill_Cool_TH::SCool_TH_3] = THSkill_Cool3;
			m_sComponent.m_pMeshCom->Set_AnimationSet(2);
			m_sStat.m_fSpeed = m_sStat.m_fInitSpeed;
			m_AniSpeed = 1.3f;
			Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, m_sStat.m_fAttack, true);
		}
	}
		break;
	case Client::CMonster1_TwoHand::State_JumpEnd:
		break;
	case Client::CMonster1_TwoHand::State_KnockBack:
		m_sComponent.m_pMeshCom->Set_AnimationSet(16);
		m_AniSpeed = 1.f;
		break;
	case Client::CMonster1_TwoHand::State_Damaged:
		m_sComponent.m_pMeshCom->Set_AnimationSet(16);
		m_AniSpeed = 1.f;
		break;
	case Client::CMonster1_TwoHand::State_Groggy:
		m_sComponent.m_pMeshCom->Set_AnimationSet(6);
		m_AniSpeed = 1.f;
		m_TimeGroggy = GROGGY;
		break;
	case Client::CMonster1_TwoHand::State_GroggyUp:
		m_sComponent.m_pMeshCom->Set_AnimationSet(59);
		m_AniSpeed = 1.f;
		break;
	case Client::CMonster1_TwoHand::State_End:
		break;
	default:
		break;
	}
	m_State = _state;
}

void Client::CMonster1_TwoHand::Pattern(_float fTimeDelta)
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
	//뭐지?
	vDir *= -1.f;
	//////////////////////////////////////////////////////////////////////////////////////
	if (m_State == State::State_Respawn) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.3f)) {
			Set_StateToAnimation(State::State_Wait);
		}
		else {
			return;
		}
	}
	else if (m_State == State::State_Dead) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.3f)) {
			Set_StateToAnimation(State::State_DeadDown);
		}
		else {
			return;
		}
	}
	else if (m_State == State::State_DeadDown) {
		m_sStat.m_IsDeadTime += fTimeDelta;
	}
	//////////////////////////////////////////////////////////////////////////////////////
	if (m_isSearch && m_State == State::State_Wait || m_State == State::State_Move || m_State == State::State_Rocate) {
		bool state_check = Random_Skill(playerTodisTance);
		if (state_check) {
			m_State = State::State_Skill;
			m_SkillCool[SKill_Cool_TH::SCool_TH_Total] = THSkill_CoolTotal;
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
			m_sComponent.m_pMeshCom->Set_AnimationSet(59);
		}
	}
	else if (m_State == State::State_Damaged) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.1f)) {
			Set_StateToAnimation(State::State_Wait);
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

void CMonster1_TwoHand::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{

	//if (_colSphere->m_BonePart == CSphereCollider::BonePart_CollBody && !_colSphere->m_WeaponAttack) {
	//	CUnit_D* _colUnit = dynamic_cast<CUnit_D*>(_col);
	//	if (nullptr == _colUnit)
	//		return;

	//	//
	//	_vec3 _colUnitPos = _colUnit->Get_sComponent()->m_pTransformCom->m_vInfo[Engine::INFO_POS];
	//	_vec3 _myUnitPos = m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS];
	//	float _Radius = (_colUnit->Get_sComponent()->m_pTransformCom->m_vScale.x * _colSphere->m_pTransformCom->m_vScale.x);
	//	_vec3 CollDir = _myUnitPos - _colUnitPos;
	//	CollDir.y = 0.f;
	//	D3DXVec3Normalize(&CollDir, &CollDir);
	//	Engine::CNaviMesh* pNaviMeshCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_NaviTerrain->m_pNaviMeshCom;
	//	_vec3	vPos;
	//	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	//	m_sComponent.m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(CollDir * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));
	//	Engine::CGameObject::Update_Object(fTimeDelta);
	//	//
	//}

	if (_mySphere->m_BoneTeam == _colSphere->m_BoneTeam || m_sStat.m_fHp <= 0.f)
		return;

	if (_mySphere->m_BonePart == CSphereCollider::BonePart_CollBody) {
		if (/*_colSphere->m_BonePart == CSphereCollider::BonePart_PlayerHammer
			&& */_colSphere->m_WeaponAttack) {
			if (!_colSphere->Check_DamagedObject(this)) {
				if(m_sStat.m_fKnockBackHp > 0.f)
					Set_StateToAnimation(State::State_Damaged);
				_colSphere->m_VecDamagedObject.emplace_back(this);
				m_sStat.m_fHp -= _col->Get_sStat()->m_fAttack;
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
				else {
					CUI_Manager::Get_Instance()->Set_DamagedEnemy(nullptr);
					Set_StateToAnimation(State::State_Dead);
				}
			}
		}
	}
	
}

void CMonster1_TwoHand::Event_Skill(float fTimeDelta, Engine::CNaviMesh* pNaviMeshCom, _vec3 vPos, _vec3 vDir, float playerTodisTance) {
	if (m_SkillState == Skill_TH::Skill_TH_1) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.15f)) {
			Set_StateToAnimation(State::State_Wait);
		}
		else {
			float trackPos = m_sComponent.m_pMeshCom->Get_AnimationTrackPos();
			//m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_sStat.m_fSpeed), &m_sStat.m_dwNaviIndex));

			/*if (trackPos > 1.6f) {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, m_sStat.m_fAttack, true);
			}*/
			if (true) {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, m_sStat.m_fAttack, true);
			}
			else {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, m_sStat.m_fAttack, false);
			}
			return;
		}
	}
	else if (m_SkillState == Skill_TH::Skill_TH_2) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.1f)) {
			Set_StateToAnimation(State::State_Wait);
		}
		else {
			if (true) {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, m_sStat.m_fAttack, true);
			}
			else {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, m_sStat.m_fAttack, false);
			}
			return;
		}
	}
	else if (m_SkillState == Skill_TH::Skill_TH_3) {
		if (m_sComponent.m_pMeshCom->Is_AnimationSetEnd(0.1f)) {
			Set_StateToAnimation(State::State_Wait);
		}
		else {
			if (true) {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, m_sStat.m_fAttack, true);
			}
			else {
				Set_BonePartColliderAttack(CSphereCollider::BonePart_Weapon, m_sStat.m_fAttack, false);
			}
		}
	}
}

bool CMonster1_TwoHand::Random_Skill(float playerTodisTance) {
	if (m_SkillCool[SKill_Cool_TH::SCool_TH_Total] > 0.f)
		return false;
	int iPattern = CRandom_Manager::Random() % 3;
	switch (iPattern)
	{
	case 0: {
		if (m_SkillCool[SKill_Cool_TH::SCool_TH_1] <= 0.f && playerTodisTance < PLAYER_ATTACK_DISTANCE && m_AngleOfSame) {
			Set_StateToAnimation(State::State_Skill, Skill_TH::Skill_TH_1);
			return true;
		}
		else {
			return false;
		}
		break;
	}
	case 1: {
		if (m_SkillCool[SKill_Cool_TH::SCool_TH_2] <= 0.f && playerTodisTance < PLAYER_ATTACK_DISTANCE && m_AngleOfSame) {
			Set_StateToAnimation(State::State_Skill, Skill_TH::Skill_TH_2);
			return true;
		}
		else {
			return false;
		}
		break;
	}
	case 2: {
		if (m_SkillCool[SKill_Cool_TH::SCool_TH_3] <= 0.f && playerTodisTance < PLAYER_ATTACK_DISTANCE && m_AngleOfSame) {
			Set_StateToAnimation(State::State_Skill, Skill_TH::Skill_TH_3);
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

void CMonster1_TwoHand::Emplace_DelayHpDec(float _attack) {
	sDelayHpDec* pDelayHpDec = new sDelayHpDec;
	pDelayHpDec->m_fDelayTime = 2.f;
	pDelayHpDec->m_fHpDec = _attack;
	m_VecDelayHpDec.emplace_back(pDelayHpDec);
	return;
}

void CMonster1_TwoHand::Update_DelayHpDec(float fTimeDelta) {
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