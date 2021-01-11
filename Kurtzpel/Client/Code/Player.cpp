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

#define COOLTIME_Q 5.f
#define COOLTIME_E 5.f
#define COOLTIME_F 5.f
#define COLLDOWNPOWER 2.5f
CPlayer* CPlayer::m_pInstance = nullptr;

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit_D(pGraphicDev)
{
	if (m_pInstance == nullptr)
		m_pInstance = this;
	for (int i = 0; i < bCheck::bCheck_End; i++)
	{
		m_bCheck[i] = false;
	}
	for (int i = 0; i < TimeCheck::TimeCheck_End; i++)
	{
		m_TimeCheck[i] = 0.f;
	}
	m_fInitSpeed = 11.5f;
	m_fSpeed = m_fInitSpeed;
	m_fJumpPower = 0.125f;
	m_fMaxHp = 1000.f;
	m_fHp = m_fMaxHp;
	m_fAttack = 600.f;
	m_CameraDist = 400.f;
}

CPlayer::~CPlayer(void)
{

}

Client::_vec3 Client::CPlayer::PickUp_OnTerrain(void)
{
	Engine::CTerrainTex*		pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK_RETURN(pTerrainBufferCom, _vec3(0.f, 0.f, 0.f));

	Engine::CTransform*		pTerrainTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Transform", Engine::ID_DYNAMIC));
	NULL_CHECK_RETURN(pTerrainTransformCom, _vec3(0.f, 0.f, 0.f));

	return m_pCalculatorCom->Picking_OnTerrain(g_hWnd, pTerrainBufferCom, pTerrainTransformCom);
}

HRESULT Client::CPlayer::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	CUnit_D::Add_Component();
	// Mesh
	pComponent = m_pMeshCom = dynamic_cast<Engine::CDynamicMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Player"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);
	m_pMeshCom->Set_AniAngle(265.f);

	//
	m_pTransformCom->Set_Pos(&Engine::_vec3{ 54.f, 0.f, 54.f });
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	Engine::CGameObject::Update_Object(1.f);

	//m_pMeshCom->Play_Animation(1.f);
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
	if (m_WeaponEquip == Weapon_Equip::Weapon_Hammer)
	{
		switch (_state)
		{
		case Client::CPlayer::State_Idle: {
			m_pMeshCom->Set_AnimationSet(243);
			m_AniSpeed = 1.f;
			
			m_Hammer->Set_Pos();
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
			break;
		}
		case Client::CPlayer::State_Move: {
			m_pMeshCom->Set_AnimationSet(260);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_MoveSA: {
			m_pMeshCom->Set_AnimationSet(258);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_MoveSD: {
			m_pMeshCom->Set_AnimationSet(256);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_Dash: {
			m_pMeshCom->Set_AnimationSet(266);
			m_fSpeed = 2.4f * m_fInitSpeed;
			m_AniSpeed = 2.1f;
			break;
		}
		case Client::CPlayer::State_Attack: {
			m_Attack_State = (Attack_State)((int)m_Attack_State + 1);
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
			m_pMeshCom->Set_AniAngle(265.f);
			//if (m_Attack_State == Attack_State::StateA_End)
			//	m_Attack_State = Attack_State::StateA_Basic1;
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.4f;
			switch (m_Attack_State)
			{
			case Client::CPlayer::StateA_Basic1:
				m_pMeshCom->Set_AnimationSet(146);
				break;
			case Client::CPlayer::StateA_Basic2:
				m_pMeshCom->Set_AnimationSet(144);
				break;
			case Client::CPlayer::StateA_Basic3:
				m_pMeshCom->Set_AnimationSet(142);
				break;
			default:
				break;
			}
			
			m_State = _state;
			m_Hammer->Set_Pos();
			
			break;
		}
		case Client::CPlayer::State_Skill: {
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
			m_pMeshCom->Set_AniAngle(265.f);
			m_fSpeed = m_fInitSpeed;
			switch (m_Attack_State)
			{
			case Client::CPlayer::StateA_SkillQ:
				m_pMeshCom->Set_AnimationSet(123);
				m_AniSpeed = 1.2f;
				m_AniClip = AnimationClip::Ani_1;
				Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
				break;
			case Client::CPlayer::StateA_SkillE: {
				m_pMeshCom->Set_AnimationSet(134);
				m_AniSpeed = 1.2f;
				break;
			}
			case Client::CPlayer::StateA_SkillF: {
				m_pMeshCom->Set_AnimationSet(130);
				m_AniSpeed = 1.3f;
				m_bCheck[bCheck::bCheck_Skill_F1] = false;
				m_bCheck[bCheck::bCheck_Skill_F2] = false;
				Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
				break;
			}
			case Client::CPlayer::StateA_SkillZ: {
				m_pMeshCom->Set_AnimationSet(13);
				m_AniSpeed = 1.1f;
				Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
				Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
				break;
			}
			default:
				break;
			}
			
			m_State = _state;
			m_Hammer->Set_Pos();
			
			break;
		}
		case Client::CPlayer::State_JumpEnd: {
			m_pMeshCom->Set_AnimationSet(277);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_JumpComboEnd: {
			m_pMeshCom->Set_AnimationSet(155);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.6f;
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
			
			m_State = _state;
			//m_Hammer->Set_Pos();
			
			break;
		}
		case Client::CPlayer::State_Damaged: {
			if (m_fKnockBackPower < COLLDOWNPOWER) {
				m_pMeshCom->Set_AnimationSet(160);
				m_AniSpeed = 1.5f;
			}
			else {  //강한어택 맞음
				m_pMeshCom->Set_AnimationSet(183);
				m_bCheck[bCheck::bCheck_DamagedUp] = false;
				m_AniSpeed = 1.4f;
			}
			m_fSpeed = m_fInitSpeed;
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
			m_pMeshCom->Set_AnimationSet(243);
			m_AniSpeed = 1.f;
			m_LongBow->Set_Pos();
			
			//Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
			break;
		}
		case Client::CPlayer::State_Move: {
			m_pMeshCom->Set_AnimationSet(260);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_MoveSA: {
			m_pMeshCom->Set_AnimationSet(258);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_MoveSD: {
			m_pMeshCom->Set_AnimationSet(256);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_Dash: {
			m_pMeshCom->Set_AnimationSet(266);
			m_fSpeed = 2.4f * m_fInitSpeed;
			m_AniSpeed = 2.1f;
			break;
		}
		case Client::CPlayer::State_Attack: {
			Create_ArrowShot(_vPos, _vDir, fTimeDelta);
			m_Attack_State = (Attack_State)((int)m_Attack_State + 1);
			//Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
			m_pMeshCom->Set_AniAngle(265.f);
			//if (m_Attack_State == Attack_State::StateA_End)
			//	m_Attack_State = Attack_State::StateA_Basic1;
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 3.f;
			switch (m_Attack_State)
			{
			case Client::CPlayer::StateA_Basic1:
				m_pMeshCom->Set_AnimationSet(77);
				break;
			case Client::CPlayer::StateA_Basic2:
				m_pMeshCom->Set_AnimationSet(74);
				break;
			case Client::CPlayer::StateA_Basic3:
				m_pMeshCom->Set_AnimationSet(71);
				break;
			default:
				break;
			}
			
			m_State = _state;
			m_LongBow->Set_Pos();
			
			break;
		}
		case Client::CPlayer::State_Skill: {
			//Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
			m_pMeshCom->Set_AniAngle(265.f);
			m_fSpeed = m_fInitSpeed;
			switch (m_Attack_State)
			{
			case Client::CPlayer::StateA_SkillQ:
				m_pMeshCom->Set_AnimationSet(123);
				m_AniSpeed = 1.2f;
				m_AniClip = AnimationClip::Ani_1;
				//Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
				break;
			case Client::CPlayer::StateA_SkillE: {
				m_pMeshCom->Set_AnimationSet(134);
				m_AniSpeed = 1.2f;
				break;
			}
			case Client::CPlayer::StateA_SkillF: {
				m_pMeshCom->Set_AnimationSet(130);
				m_AniSpeed = 1.3f;
				m_bCheck[bCheck::bCheck_Skill_F1] = false;
				m_bCheck[bCheck::bCheck_Skill_F2] = false;
				//Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
				break;
			}
			case Client::CPlayer::StateA_SkillZ: {
				m_pMeshCom->Set_AnimationSet(13);
				m_AniSpeed = 1.1f;
				//Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
				//Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
				break;
			}
			default:
				break;
			}
			m_State = _state;
			m_LongBow->Set_Pos();
			
			break;
		}
		case Client::CPlayer::State_JumpEnd: {
			m_pMeshCom->Set_AnimationSet(277);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_JumpComboEnd: {
			m_pMeshCom->Set_AnimationSet(155);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.6f;
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
			m_State = _state;
			//m_LongBow->Set_Pos();
			
			break;
		}
		case Client::CPlayer::State_Damaged: {
			if (m_fKnockBackPower < COLLDOWNPOWER) {
				m_pMeshCom->Set_AnimationSet(160);
				m_AniSpeed = 1.5f;
			}
			else {  //강한어택 맞음
				m_pMeshCom->Set_AnimationSet(183);
				m_bCheck[bCheck::bCheck_DamagedUp] = false;
				m_AniSpeed = 1.4f;
			}
			m_fSpeed = m_fInitSpeed;
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
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &m_vDir);
	_vec3	vPos, vDir, vDir2;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
	D3DXVec3Normalize(&vDir, &vDir);
	Key_DoubleInput(fTimeDelta);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// 피격 당할때
	if (m_State == State::State_Damaged) { ////////////////////////////////////////////////////////////////////////////// 피격
		if (m_fKnockBackPower > COLLDOWNPOWER) {		//강한 공격을 맞고 다운
			if (!m_bCheck[bCheck::bCheck_DamagedUp]) {
				if (m_pMeshCom->Is_AnimationSetEnd(0.4f)) {
					m_bCheck[bCheck::bCheck_DamagedUp] = true;
					m_pMeshCom->Set_AnimationSet(245);
				}
				else {
					if (m_pMeshCom->Get_AnimationTrackPos() < 0.68f) {
						m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_fKnockBackDir * fTimeDelta * m_fSpeed * 1.5f * m_fKnockBackPower), &m_dwNaviIndex));
					}
					m_fSpeed -= m_fInitSpeed * fTimeDelta * 2.5f;
					return;
				}
			}
			else {
				if (m_pMeshCom->Is_AnimationSetEnd(0.9f)) {
					Set_StateToAnimation(State::State_Idle);
				}
				return;
			}
		}
		else {											//약한 공격을 맞고 약간 넉백
			if (m_pMeshCom->Is_AnimationSetEnd()) {
				Set_StateToAnimation(State::State_Idle);
			}
			else {
				m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_fKnockBackDir * fTimeDelta * m_fSpeed * 1.5f * m_fKnockBackPower), &m_dwNaviIndex));
				m_fSpeed -= m_fInitSpeed * fTimeDelta * 2.5f;
				return;
			}
		}
	}
	else if (m_State == State::State_Skill) { /////////////////////////////////////////////////////////////////////////// Skill

		Event_Skill(fTimeDelta, pNaviMeshCom, vPos, vDir);

	}
	else if (m_State == State::State_Attack) { ////////////////////////////////////////////////////////////////////////// Attack
		if (m_bCheck[bCheck::bCheck_MouseL_Already]) {
			if (m_pMeshCom->Is_AnimationSetEnd(0.85f)) {
				Set_StateToAnimation(State::State_Attack);
				m_bCheck[bCheck::bCheck_MouseL_Already] = false;
				Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
			}
		}
		else if (m_pMeshCom->Is_AnimationSetEnd(0.35f)) {
			Set_StateToAnimation(State::State_Idle);
			m_Attack_State = Attack_State::StateA_None;
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
		}
		else if((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) && (m_Attack_State == Attack_State::StateA_Basic1 || m_Attack_State == Attack_State::StateA_Basic2)){
			float aniPos = m_pMeshCom->Get_AnimationTrackPos();

			if (aniPos > 0.35f) {
				m_bCheck[bCheck::bCheck_MouseL_Already] = true;
			}
		}
		//공격모션 동안 움직임
		float trackPos = m_pMeshCom->Get_AnimationTrackPos();
		if (trackPos < 0.3f && trackPos > 0.1f) {
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
		}
		if (trackPos > 0.1f && trackPos < 0.7f) {
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
		}
		else {
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
		}
	}
	else if (m_State == State::State_JumpComboEnd) { ////////////////////////////////////////////////////////////////////// JumpComboEnd
		if (m_pMeshCom->Is_AnimationSetEnd(0.05f)) {
			Set_StateToAnimation(State::State_Idle);
		}
		Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
		return;
	}
	else if (m_State == State::State_Dash) { ///////////////////////////////////////////////////////////////////////////// Dash
		if (m_pMeshCom->Is_AnimationSetEnd(0.7f))//m_TimeCheck[TimeCheck::TimeCheck_Dash] < 0.f)
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
		else if (m_EnumDir == EnumDir::Up) {
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(265.f);
		}
		else if (m_EnumDir == EnumDir::UpLeft) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(220.f);
		}
		else if (m_EnumDir == EnumDir::UpRight) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(310.f);
		}
		else if (m_EnumDir == EnumDir::Down) {
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(85.f);
		}
		else if (m_EnumDir == EnumDir::DownLeft) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(130.f);
		}
		else if (m_EnumDir == EnumDir::DownRight) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(40.f);
		}
		else if (m_EnumDir == EnumDir::Left) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(175.f);
		}
		else if (m_EnumDir == EnumDir::Right) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(355.f);
		}

	}
	//////////////////////////////////////////////////////////////////// 실제 키 입력부분
	else if ((Engine::Get_DIKeyState(DIK_F) & 0x80) && m_TimeCheck[TimeCheck_Cool_F] <= 0.f) {
		m_TimeCheck[TimeCheck_Cool_F] = COOLTIME_F;
		m_Attack_State = Attack_State::StateA_SkillF;
		Set_StateToAnimation(State::State_Skill);
	}
	else if ((Engine::Get_DIKeyState(DIK_E) & 0x80) && m_TimeCheck[TimeCheck_Cool_E] <= 0.f) {
		m_TimeCheck[TimeCheck_Cool_E] = COOLTIME_E;
		m_Attack_State = Attack_State::StateA_SkillE;
		Set_StateToAnimation(State::State_Skill);
	}
	else if ((Engine::Get_DIKeyState(DIK_Q) & 0x80) && m_TimeCheck[TimeCheck_Cool_Q] <= 0.f) {
		m_TimeCheck[TimeCheck_Cool_Q] = COOLTIME_Q;
		m_Attack_State = Attack_State::StateA_SkillQ;
	Set_StateToAnimation(State::State_Skill);
	}
	else if ((Engine::Get_DIKeyState(DIK_Z) & 0x80)) {
		m_Attack_State = Attack_State::StateA_SkillZ;
		Set_StateToAnimation(State::State_Skill);
	}
	//////////////////////////////////////////////////////////////////// 좌클릭공격
	else if ((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) && !m_bCheck[bCheck::bCheck_MouseL])
	{
		m_bCheck[bCheck::bCheck_MouseL] = true;
		Set_StateToAnimation(State::State_Attack, vPos, vDir, fTimeDelta);
		m_bCheck[bCheck::bCheck_MouseL_Already] = false;

	}
	//////////////////////////////////////////////////////////////////// 무기 체인지
	else if ((Engine::Get_DIKeyState(DIK_TAB) & 0x80) && m_TimeCheck[TimeCheck_Cool_Tab] <= 0.f) {
		Change_Weapon();
	}
	//////////////////////////////////////////////////////////////////// 이동
	else if (Engine::Get_DIKeyState(DIK_W) & 0x80)
	{
		if (m_bCheck[bCheck::bCheck_DBKeyW]) {
			if (m_bCheck[bCheck::bCheck_DBKeyA]) {
				m_EnumDir = EnumDir::UpLeft;
			}
			else if (m_bCheck[bCheck::bCheck_DBKeyD]) {
				m_EnumDir = EnumDir::UpRight;
			}
			else {
				m_EnumDir = EnumDir::Up;
			}
			Set_StateToAnimation(State::State_Dash);
			m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
		}
		else {
			if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
				vDir2 = { -vDir.z, 0.f, vDir.x };
				vDir2 += vDir;
				D3DXVec3Normalize(&vDir2, &vDir2);
				m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

				m_pMeshCom->Set_AniAngle(220.f);
				Set_StateToAnimation(State::State_Move);
			}
			else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
				vDir2 = { vDir.z, 0.f, -vDir.x };
				vDir2 += vDir;
				D3DXVec3Normalize(&vDir2, &vDir2);
				m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

				m_pMeshCom->Set_AniAngle(310.f);
				Set_StateToAnimation(State::State_Move);
			}
			else {
				m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

				m_pMeshCom->Set_AniAngle(265.f);
				Set_StateToAnimation(State::State_Move);
			}
		}
	}
	else if (Engine::Get_DIKeyState(DIK_S) & 0x80)
	{
		if (m_bCheck[bCheck::bCheck_DBKeyS]) {
			if (m_bCheck[bCheck::bCheck_DBKeyA]) {
				m_EnumDir = EnumDir::DownLeft;
			}
			else if (m_bCheck[bCheck::bCheck_DBKeyD]) {
				m_EnumDir = EnumDir::DownRight;
			}
			else {
				m_EnumDir = EnumDir::Down;
			}
			Set_StateToAnimation(State::State_Dash);
			m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
		}
		else if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(175.f);
			Set_StateToAnimation(State::State_MoveSA);
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(355.f);
			Set_StateToAnimation(State::State_MoveSD);
		}
		else {
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(85.f);
			Set_StateToAnimation(State::State_Move);
		}
	}
	else if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
		m_State = State::State_Move;
		if (m_bCheck[bCheck::bCheck_DBKeyA]) {
			if (m_bCheck[bCheck::bCheck_DBKeyW]) {
				m_EnumDir = EnumDir::UpLeft;
			}
			else if (m_bCheck[bCheck::bCheck_DBKeyS]) {
				m_EnumDir = EnumDir::DownLeft;
			}
			else {
				m_EnumDir = EnumDir::Left;
			}
			Set_StateToAnimation(State::State_Dash);
			m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
		} 
		else {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(175.f);
			Set_StateToAnimation(State::State_Move);
		}
	}
	else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
		m_State = State::State_Move;
		if (m_bCheck[bCheck::bCheck_DBKeyD]) {
			if (m_bCheck[bCheck::bCheck_DBKeyW]) {
				m_EnumDir = EnumDir::UpRight;
			}
			else if (m_bCheck[bCheck::bCheck_DBKeyS]) {
				m_EnumDir = EnumDir::DownRight;
			}
			else {
				m_EnumDir = EnumDir::Right;
			}
			Set_StateToAnimation(State::State_Dash);
			m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
		}
		else {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(355.f);
			Set_StateToAnimation(State::State_Move);
		}
	}

	//if (Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80)
	//{
	//	_vec3	vPickPos = PickUp_OnTerrain();
	//	m_pTransformCom->Pick_Pos(&vPickPos, m_fSpeed, fTimeDelta);
	//	
	//}
	//if (Engine::Get_DIMouseState(Engine::DIM_RB) & 0x80)
	//{
	//	m_pMeshCom->Set_AnimationSet(30);
	//}
	//if (!m_bCheck[bCheck::bCheck_MoveAni] && true == m_pMeshCom->Is_AnimationSetEnd() && m_AniEnd) {
	//	m_AniSpeed = 1.f;
	//	//m_pMeshCom->Set_AniAngle(265.f);
	//	m_pMeshCom->Set_AnimationSet(243);
	//	m_AniEnd = false;
	//}
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
		if (m_pMeshCom->Is_AnimationSetEnd(0.3f)) {
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
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &m_vDir);
	_vec3	vPos, vDir, vDir2;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
	D3DXVec3Normalize(&vDir, &vDir);
	Key_DoubleInput(fTimeDelta);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 피격 당할때
	if (m_State == State::State_Damaged) { ////////////////////////////////////////////////////////////////////////////// 피격
		if (m_fKnockBackPower > COLLDOWNPOWER) {		//강한 공격을 맞고 다운
			if (!m_bCheck[bCheck::bCheck_DamagedUp]) {
				if (m_pMeshCom->Is_AnimationSetEnd(0.4f)) {
					m_bCheck[bCheck::bCheck_DamagedUp] = true;
					m_pMeshCom->Set_AnimationSet(245);
				}
				else {
					if (m_pMeshCom->Get_AnimationTrackPos() < 0.68f) {
						m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_fKnockBackDir * fTimeDelta * m_fSpeed * 1.5f * m_fKnockBackPower), &m_dwNaviIndex));
					}
					m_fSpeed -= m_fInitSpeed * fTimeDelta * 2.5f;
					return;
				}
			}
			else {
				if (m_pMeshCom->Is_AnimationSetEnd(0.9f)) {
					Set_StateToAnimation(State::State_Idle);
				}
				return;
			}
		}
		else {											//약한 공격을 맞고 약간 넉백
			if (m_pMeshCom->Is_AnimationSetEnd()) {
				Set_StateToAnimation(State::State_Idle);
			}
			else {
				m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(m_fKnockBackDir * fTimeDelta * m_fSpeed * 1.5f * m_fKnockBackPower), &m_dwNaviIndex));
				m_fSpeed -= m_fInitSpeed * fTimeDelta * 2.5f;
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
				if (m_pMeshCom->Is_AnimationSetEnd(1.06f)) {
					Set_StateToAnimation(State::State_Attack, vPos, vDir, fTimeDelta);
						m_bCheck[bCheck::bCheck_MouseL_Already] = false;
				}
			}
			else if (m_Attack_State == Attack_State::StateA_Basic2) {
				if (m_pMeshCom->Is_AnimationSetEnd(1.6f)) {
					Set_StateToAnimation(State::State_Attack, vPos, vDir, fTimeDelta);
					m_bCheck[bCheck::bCheck_MouseL_Already] = false;
				}
			}
		}
		else if (m_Attack_State == Attack_State::StateA_Basic1 && m_pMeshCom->Is_AnimationSetEnd(0.8f)) {
			Set_StateToAnimation(State::State_Idle);
			m_Attack_State = Attack_State::StateA_None;
		}
		else if (m_Attack_State == Attack_State::StateA_Basic2 && m_pMeshCom->Is_AnimationSetEnd(1.3f)) {
			Set_StateToAnimation(State::State_Idle);
			m_Attack_State = Attack_State::StateA_None;
		}
		else if (m_Attack_State == Attack_State::StateA_Basic3 && m_pMeshCom->Is_AnimationSetEnd(1.8f)) {
			Set_StateToAnimation(State::State_Idle);
			m_Attack_State = Attack_State::StateA_None;
		}
		else if ((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) && (m_Attack_State == Attack_State::StateA_Basic1 || m_Attack_State == Attack_State::StateA_Basic2)) {
			float aniPos = m_pMeshCom->Get_AnimationTrackPos();

			if (aniPos > 0.35f) {
				m_bCheck[bCheck::bCheck_MouseL_Already] = true;
			}
		}
		//공격모션 동안 움직임
		float trackPos = m_pMeshCom->Get_AnimationTrackPos();
		if (trackPos < 0.3f && trackPos > 0.1f) {
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
		}
		if (trackPos > 0.1f && trackPos < 0.7f) {
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
		}
		else {
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
		}
	}
	else if (m_State == State::State_JumpComboEnd) { ////////////////////////////////////////////////////////////////////// JumpComboEnd
		if (m_pMeshCom->Is_AnimationSetEnd(0.05f)) {
			Set_StateToAnimation(State::State_Idle);
		}
		Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
		return;
	}
	else if (m_State == State::State_Dash) { ///////////////////////////////////////////////////////////////////////////// Dash
		if (m_pMeshCom->Is_AnimationSetEnd(0.7f))//m_TimeCheck[TimeCheck::TimeCheck_Dash] < 0.f)
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
		else if (m_EnumDir == EnumDir::Up) {
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(265.f);
		}
		else if (m_EnumDir == EnumDir::UpLeft) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(220.f);
		}
		else if (m_EnumDir == EnumDir::UpRight) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(310.f);
		}
		else if (m_EnumDir == EnumDir::Down) {
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(85.f);
		}
		else if (m_EnumDir == EnumDir::DownLeft) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(130.f);
		}
		else if (m_EnumDir == EnumDir::DownRight) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(40.f);
		}
		else if (m_EnumDir == EnumDir::Left) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(175.f);
		}
		else if (m_EnumDir == EnumDir::Right) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
			m_pMeshCom->Set_AniAngle(355.f);
		}

	}
	//////////////////////////////////////////////////////////////////// 실제 키 입력부분
	else if ((Engine::Get_DIKeyState(DIK_F) & 0x80) && m_TimeCheck[TimeCheck_Cool_F] <= 0.f) {
		m_TimeCheck[TimeCheck_Cool_F] = COOLTIME_F;
		m_Attack_State = Attack_State::StateA_SkillF;
		Set_StateToAnimation(State::State_Skill);
	}
	else if ((Engine::Get_DIKeyState(DIK_E) & 0x80) && m_TimeCheck[TimeCheck_Cool_E] <= 0.f) {
		m_TimeCheck[TimeCheck_Cool_E] = COOLTIME_E;
		m_Attack_State = Attack_State::StateA_SkillE;
		Set_StateToAnimation(State::State_Skill);
	}
	else if ((Engine::Get_DIKeyState(DIK_Q) & 0x80) && m_TimeCheck[TimeCheck_Cool_Q] <= 0.f) {
		m_TimeCheck[TimeCheck_Cool_Q] = COOLTIME_Q;
		m_Attack_State = Attack_State::StateA_SkillQ;
		Set_StateToAnimation(State::State_Skill);
	}
	else if ((Engine::Get_DIKeyState(DIK_Z) & 0x80)) {
		m_Attack_State = Attack_State::StateA_SkillZ;
		Set_StateToAnimation(State::State_Skill);
	}
	//////////////////////////////////////////////////////////////////// 좌클릭공격
	else if ((Engine::Get_DIMouseState(Engine::DIM_RB) & 0x80) && !m_bCheck[bCheck::bCheck_MouseL])
	{
		m_bCheck[bCheck::bCheck_MouseL] = true;
		Set_StateToAnimation(State::State_Attack, vPos, vDir, fTimeDelta);
		m_bCheck[bCheck::bCheck_MouseL_Already] = false;

	}
	//////////////////////////////////////////////////////////////////// 무기 체인지
	else if ((Engine::Get_DIKeyState(DIK_TAB) & 0x80) && m_TimeCheck[TimeCheck_Cool_Tab] <= 0.f) {
		Change_Weapon();
	}
	//////////////////////////////////////////////////////////////////// 이동
	else if (Engine::Get_DIKeyState(DIK_W) & 0x80)
	{
		if (m_bCheck[bCheck::bCheck_DBKeyW]) {
			if (m_bCheck[bCheck::bCheck_DBKeyA]) {
				m_EnumDir = EnumDir::UpLeft;
			}
			else if (m_bCheck[bCheck::bCheck_DBKeyD]) {
				m_EnumDir = EnumDir::UpRight;
			}
			else {
				m_EnumDir = EnumDir::Up;
			}
			Set_StateToAnimation(State::State_Dash);
			m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
		}
		else {
			if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
				vDir2 = { -vDir.z, 0.f, vDir.x };
				vDir2 += vDir;
				D3DXVec3Normalize(&vDir2, &vDir2);
				m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

				m_pMeshCom->Set_AniAngle(220.f);
				Set_StateToAnimation(State::State_Move);
			}
			else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
				vDir2 = { vDir.z, 0.f, -vDir.x };
				vDir2 += vDir;
				D3DXVec3Normalize(&vDir2, &vDir2);
				m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

				m_pMeshCom->Set_AniAngle(310.f);
				Set_StateToAnimation(State::State_Move);
			}
			else {
				m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

				m_pMeshCom->Set_AniAngle(265.f);
				Set_StateToAnimation(State::State_Move);
			}
		}
	}
	else if (Engine::Get_DIKeyState(DIK_S) & 0x80)
	{
		if (m_bCheck[bCheck::bCheck_DBKeyS]) {
			if (m_bCheck[bCheck::bCheck_DBKeyA]) {
				m_EnumDir = EnumDir::DownLeft;
			}
			else if (m_bCheck[bCheck::bCheck_DBKeyD]) {
				m_EnumDir = EnumDir::DownRight;
			}
			else {
				m_EnumDir = EnumDir::Down;
			}
			Set_StateToAnimation(State::State_Dash);
			m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
		}
		else if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(175.f);
			Set_StateToAnimation(State::State_MoveSA);
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(355.f);
			Set_StateToAnimation(State::State_MoveSD);
		}
		else {
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(85.f);
			Set_StateToAnimation(State::State_Move);
		}
	}
	else if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
		m_State = State::State_Move;
		if (m_bCheck[bCheck::bCheck_DBKeyA]) {
			if (m_bCheck[bCheck::bCheck_DBKeyW]) {
				m_EnumDir = EnumDir::UpLeft;
			}
			else if (m_bCheck[bCheck::bCheck_DBKeyS]) {
				m_EnumDir = EnumDir::DownLeft;
			}
			else {
				m_EnumDir = EnumDir::Left;
			}
			Set_StateToAnimation(State::State_Dash);
			m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
		}
		else {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(175.f);
			Set_StateToAnimation(State::State_Move);
		}
	}
	else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
		m_State = State::State_Move;
		if (m_bCheck[bCheck::bCheck_DBKeyD]) {
			if (m_bCheck[bCheck::bCheck_DBKeyW]) {
				m_EnumDir = EnumDir::UpRight;
			}
			else if (m_bCheck[bCheck::bCheck_DBKeyS]) {
				m_EnumDir = EnumDir::DownRight;
			}
			else {
				m_EnumDir = EnumDir::Right;
			}
			Set_StateToAnimation(State::State_Dash);
			m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
		}
		else {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(355.f);
			Set_StateToAnimation(State::State_Move);
		}
	}

	//if (Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80)
	//{
	//	_vec3	vPickPos = PickUp_OnTerrain();
	//	m_pTransformCom->Pick_Pos(&vPickPos, m_fSpeed, fTimeDelta);
	//	
	//}
	//if (Engine::Get_DIMouseState(Engine::DIM_RB) & 0x80)
	//{
	//	m_pMeshCom->Set_AnimationSet(30);
	//}
	//if (!m_bCheck[bCheck::bCheck_MoveAni] && true == m_pMeshCom->Is_AnimationSetEnd() && m_AniEnd) {
	//	m_AniSpeed = 1.f;
	//	//m_pMeshCom->Set_AniAngle(265.f);
	//	m_pMeshCom->Set_AnimationSet(243);
	//	m_AniEnd = false;
	//}
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
		if (m_pMeshCom->Is_AnimationSetEnd(0.3f)) {
			Set_StateToAnimation(State::State_Idle);
		}
		return;
	}
	// Idle 모션
	else if (m_State == State::State_Idle) {
		Set_StateToAnimation(State::State_Idle);
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
				//m_pMeshCom->Set_AnimationSet(273);
				m_JumpIdleState = JumpIdleAni::JumpIdle_JumpUp;
				m_AniSpeed = 1.f;
				m_pMeshCom->Set_AnimationSet(275);
				break;
			}
			case JumpIdleAni::JumpIdle_JumpUp: {
				m_AniSpeed = 1.f;
				m_pMeshCom->Set_AnimationSet(275);
				break;
			}
			case JumpIdleAni::JumpIdle_JumpDown: {
				m_AniSpeed = 1.f;
				m_pMeshCom->Set_AnimationSet(279);
				break;
			}
			}
			m_fSpeed = m_fInitSpeed;
			break;
		}
		case Client::CPlayer::State_Dash: {
			m_pMeshCom->Set_AnimationSet(266);
			m_fSpeed = 3.5f * m_fInitSpeed;
			m_AniSpeed = 1.8f;
			break;
		}
		case Client::CPlayer::State_Attack: {
			m_Attack_State = (Attack_State)((int)m_Attack_State + 1);
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
			m_pMeshCom->Set_AniAngle(265.f);
			//if (m_Attack_State == Attack_State::StateA_End)
			//	m_Attack_State = Attack_State::StateA_Basic1;
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.1f;
			m_fJumpAccel = 1.2f;
			switch (m_Attack_State)
			{
			case Client::CPlayer::StateA_Basic1:
				m_pMeshCom->Set_AnimationSet(154);
				break;
			case Client::CPlayer::StateA_Basic2:
				m_pMeshCom->Set_AnimationSet(153);
				break;
			case Client::CPlayer::StateA_Basic3:
				m_pMeshCom->Set_AnimationSet(152);
				break;
			case Client::CPlayer::StateA_Basic4:
				m_pMeshCom->Set_AnimationSet(150);
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
			m_pMeshCom->Set_AnimationSet(277);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_Damaged: {
			m_pMeshCom->Set_AnimationSet(183);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.4f;
			if (m_fJumpAccel < 1.42f) {
				m_fJumpAccel = 1.42f;
			}
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
			if (m_WeaponEquip == Weapon_Equip::Weapon_Hammer)
				m_LongBow->Set_Pos();
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
			m_Attack_State = Attack_State::StateA_None;
			switch (m_JumpIdleState)
			{
			case JumpIdleAni::JumpIdle_JumpStart: {
				//m_pMeshCom->Set_AnimationSet(273);
				m_JumpIdleState = JumpIdleAni::JumpIdle_JumpUp;
				m_AniSpeed = 1.f;
				m_pMeshCom->Set_AnimationSet(275);
				break;
			}
			case JumpIdleAni::JumpIdle_JumpUp: {
				m_AniSpeed = 1.f;
				m_pMeshCom->Set_AnimationSet(275);
				break;
			}
			case JumpIdleAni::JumpIdle_JumpDown: {
				m_AniSpeed = 1.f;
				m_pMeshCom->Set_AnimationSet(279);
				break;
			}
			}
			m_fSpeed = m_fInitSpeed;
			break;
		}
		case Client::CPlayer::State_Dash: {
			m_pMeshCom->Set_AnimationSet(266);
			m_fSpeed = 3.5f * m_fInitSpeed;
			m_AniSpeed = 1.8f;
			break;
		}
		case Client::CPlayer::State_Attack: {
			m_Attack_State = (Attack_State)((int)m_Attack_State + 1);
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
			m_pMeshCom->Set_AniAngle(265.f);
			//if (m_Attack_State == Attack_State::StateA_End)
			//	m_Attack_State = Attack_State::StateA_Basic1;
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.1f;
			m_fJumpAccel = 1.2f;
			switch (m_Attack_State)
			{
			case Client::CPlayer::StateA_Basic1:
				m_pMeshCom->Set_AnimationSet(154);
				break;
			case Client::CPlayer::StateA_Basic2:
				m_pMeshCom->Set_AnimationSet(153);
				break;
			case Client::CPlayer::StateA_Basic3:
				m_pMeshCom->Set_AnimationSet(152);
				break;
			case Client::CPlayer::StateA_Basic4:
				m_pMeshCom->Set_AnimationSet(150);
				Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
				break;
			default:
				break;
			}
			if (m_WeaponEquip == Weapon_Equip::Weapon_Hammer) {
				m_State = _state;
				m_LongBow->Set_Pos();
			}
			break;
		}
		case Client::CPlayer::State_JumpEnd: {
			m_pMeshCom->Set_AnimationSet(277);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.f;
			break;
		}
		case Client::CPlayer::State_Damaged: {
			m_pMeshCom->Set_AnimationSet(183);
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.4f;
			if (m_fJumpAccel < 1.42f) {
				m_fJumpAccel = 1.42f;
			}
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
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &m_vDir);
	_vec3	vPos, vDir, vDir2;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
	D3DXVec3Normalize(&vDir, &vDir);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (m_State == State::State_Attack) {
		if (m_Attack_State == Attack_State::StateA_Basic4){
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed * 1.4f), &m_dwNaviIndex));
		}
		else {
			if (m_bCheck[bCheck::bCheck_MouseL_Already]) {
				if (m_Attack_State == Attack_State::StateA_Basic1) {
					if (m_pMeshCom->Is_AnimationSetEnd(0.5f)) {
						Set_StateToAnimation_Jump(State::State_Attack);
						m_bCheck[bCheck::bCheck_MouseL_Already] = false;
						Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
					}
				}
				else if (m_Attack_State == Attack_State::StateA_Basic2) {
					if (m_pMeshCom->Is_AnimationSetEnd(0.2f)) {
						Set_StateToAnimation_Jump(State::State_Attack);
						m_bCheck[bCheck::bCheck_MouseL_Already] = false;
						Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
					}
				}
				else if (m_Attack_State == Attack_State::StateA_Basic3) {
					if (m_pMeshCom->Is_AnimationSetEnd(0.2f)) {
						Set_StateToAnimation_Jump(State::State_Attack);
						m_bCheck[bCheck::bCheck_MouseL_Already] = false;
						Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
					}
				}
			}
			else if (m_Attack_State == Attack_State::StateA_Basic1 && m_pMeshCom->Is_AnimationSetEnd(0.2f)) {
				Set_StateToAnimation_Jump(State::State_Idle);
			}
			else if (m_Attack_State == Attack_State::StateA_Basic2 && m_pMeshCom->Is_AnimationSetEnd(0.02f)) {
				Set_StateToAnimation_Jump(State::State_Idle);
			}
			else if (m_Attack_State == Attack_State::StateA_Basic3 && m_pMeshCom->Is_AnimationSetEnd(0.05f)) {
				Set_StateToAnimation_Jump(State::State_Idle);
			}
			else if ((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) &&
				(m_Attack_State == Attack_State::StateA_Basic1 || m_Attack_State == Attack_State::StateA_Basic2 || m_Attack_State == Attack_State::StateA_Basic3)) {
				float aniPos = m_pMeshCom->Get_AnimationTrackPos();

				if (aniPos > 0.35f) {
					m_bCheck[bCheck::bCheck_MouseL_Already] = true;
				}
			}
			//공격모션 동안 움직임
			float trackPos = m_pMeshCom->Get_AnimationTrackPos();
			if (trackPos < 0.3f && trackPos > 0.15f) {
				m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
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
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
			
			m_pMeshCom->Set_AniAngle(220.f);
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(310.f);
		}
		else {
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(265.f);
		}
		Set_StateToAnimation_Jump(State::State_Idle);
	}
	else if (Engine::Get_DIKeyState(DIK_S) & 0x80)
	{
		if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(175.f);
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(355.f);
		}
		else {
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(85.f);
		}
		Set_StateToAnimation_Jump(State::State_Idle);
	}
	else if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
		vDir2 = { -vDir.z, 0.f, vDir.x };
		m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

		m_pMeshCom->Set_AniAngle(175.f);
		Set_StateToAnimation_Jump(State::State_Idle);
	}
	else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
		vDir2 = { vDir.z, 0.f, -vDir.x };
		m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

		m_pMeshCom->Set_AniAngle(355.f);
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
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &m_vDir);
	_vec3	vPos, vDir, vDir2;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
	D3DXVec3Normalize(&vDir, &vDir);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (m_State == State::State_Attack) {
		if (m_Attack_State == Attack_State::StateA_Basic4) {
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed * 1.4f), &m_dwNaviIndex));
		}
		else {
			if (m_bCheck[bCheck::bCheck_MouseL_Already]) {
				if (m_Attack_State == Attack_State::StateA_Basic1) {
					if (m_pMeshCom->Is_AnimationSetEnd(0.5f)) {
						Set_StateToAnimation_Jump(State::State_Attack);
						m_bCheck[bCheck::bCheck_MouseL_Already] = false;
						Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
					}
				}
				else if (m_Attack_State == Attack_State::StateA_Basic2) {
					if (m_pMeshCom->Is_AnimationSetEnd(0.2f)) {
						Set_StateToAnimation_Jump(State::State_Attack);
						m_bCheck[bCheck::bCheck_MouseL_Already] = false;
						Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
					}
				}
				else if (m_Attack_State == Attack_State::StateA_Basic3) {
					if (m_pMeshCom->Is_AnimationSetEnd(0.2f)) {
						Set_StateToAnimation_Jump(State::State_Attack);
						m_bCheck[bCheck::bCheck_MouseL_Already] = false;
						Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = false;
					}
				}
			}
			else if (m_Attack_State == Attack_State::StateA_Basic1 && m_pMeshCom->Is_AnimationSetEnd(0.2f)) {
				Set_StateToAnimation_Jump(State::State_Idle);
			}
			else if (m_Attack_State == Attack_State::StateA_Basic2 && m_pMeshCom->Is_AnimationSetEnd(0.02f)) {
				Set_StateToAnimation_Jump(State::State_Idle);
			}
			else if (m_Attack_State == Attack_State::StateA_Basic3 && m_pMeshCom->Is_AnimationSetEnd(0.05f)) {
				Set_StateToAnimation_Jump(State::State_Idle);
			}
			else if ((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) &&
				(m_Attack_State == Attack_State::StateA_Basic1 || m_Attack_State == Attack_State::StateA_Basic2 || m_Attack_State == Attack_State::StateA_Basic3)) {
				float aniPos = m_pMeshCom->Get_AnimationTrackPos();

				if (aniPos > 0.35f) {
					m_bCheck[bCheck::bCheck_MouseL_Already] = true;
				}
			}
			//공격모션 동안 움직임
			float trackPos = m_pMeshCom->Get_AnimationTrackPos();
			if (trackPos < 0.3f && trackPos > 0.15f) {
				m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
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
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(220.f);
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(310.f);
		}
		else {
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(265.f);
		}
		Set_StateToAnimation_Jump(State::State_Idle);
	}
	else if (Engine::Get_DIKeyState(DIK_S) & 0x80)
	{
		if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(175.f);
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(355.f);
		}
		else {
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_fSpeed), &m_dwNaviIndex));

			m_pMeshCom->Set_AniAngle(85.f);
		}
		Set_StateToAnimation_Jump(State::State_Idle);
	}
	else if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
		vDir2 = { -vDir.z, 0.f, vDir.x };
		m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

		m_pMeshCom->Set_AniAngle(175.f);
		Set_StateToAnimation_Jump(State::State_Idle);
	}
	else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
		vDir2 = { vDir.z, 0.f, -vDir.x };
		m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed), &m_dwNaviIndex));

		m_pMeshCom->Set_AniAngle(355.f);
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

	m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);
	Set_StateToAnimation(State::State_Idle);

	m_dwNaviIndex = 0;
	//m_pNaviMeshCom->Set_NaviIndex(0);

	return S_OK;
}
Client::_int Client::CPlayer::Update_Object(const _float& fTimeDelta)
{

	Calc_Time(fTimeDelta);
	if (m_JumpIdleState != JumpIdleAni::JumpIdle_None) {
		if(m_WeaponEquip == Weapon_Equip::Weapon_Hammer)
			Hammer_Key_InputOfJump(fTimeDelta);
		else if (m_WeaponEquip == Weapon_Equip::Weapon_LongBow)
			LongBow_Key_InputOfJump(fTimeDelta);
		Jump_Control(fTimeDelta);
	}
	else {
		SetUp_OnTerrain();
		if (m_WeaponEquip == Weapon_Equip::Weapon_Hammer)
			Hammer_Key_Input(fTimeDelta);
		else if (m_WeaponEquip == Weapon_Equip::Weapon_LongBow)
			LongBow_Key_Input(fTimeDelta);
	}
	//
	//CameraControl(fTimeDelta);
	//
	Set_RenderCollSphere();
	if (Engine::Get_DIKeyState(DIK_F11) & 0x80) {
		m_pTransformCom->Set_Pos(&Engine::_vec3{ 54.f, 0.f, 54.f });
	}

	CUnit_D::Update_Object(fTimeDelta);
	m_pMeshCom->Play_Animation(fTimeDelta * m_AniSpeed);

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}

void Client::CPlayer::Render_Object(void)
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
void CPlayer::Add_LookAtY(float lookat)
{
	if (m_LookAtY + lookat > 7.5f) {
		m_LookAtY = 7.5f;
	}
	else if (m_LookAtY + lookat < -2.5f) {
		m_LookAtY = -2.5f;
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
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPosition, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	if (m_State == State::State_Idle || (m_WeaponEquip == Weapon_Equip::Weapon_Hammer && m_Attack_State == Attack_State::StateA_Basic4)) {
		if (m_State == State::State_Idle) {
			if (m_fJumpAccel < 1.42f) {
				m_fJumpAccel += 2.4f * fTimeDelta;
			}
		}
		else if(m_Attack_State == Attack_State::StateA_Basic4){
			if (m_fJumpAccel < 1.64f) {
				m_fJumpAccel += 3.2f * fTimeDelta;
			}
		}
		float gravity = m_fJumpPower * m_fJumpAccel - (GRAVITY * m_fJumpAccel * m_fJumpAccel * 0.5f);

		float* beforePosY = &m_pTransformCom->m_vInfo[Engine::INFO_POS].y;
		float afterPosY = gravity + *beforePosY;

		if (fHeight > afterPosY) {
			m_JumpIdleState = JumpIdleAni::JumpIdle_None;
			if (m_State == State::State_Idle)
				Set_StateToAnimation(State::State_JumpEnd);
			else if (m_Attack_State == Attack_State::StateA_Basic4)
				Set_StateToAnimation(State::State_JumpComboEnd);
			//m_pMeshCom->Is_Loop_Change(FALSE);
			m_fJumpAccel = 0.f;
			*beforePosY = fHeight;
		}
		else {
			if (gravity >= 0) {
				//if(m_JumpIdleState != JumpIdleAni::JumpIdle_JumpStart)
				//m_JumpIdleState = JumpIdleAni::JumpIdle_JumpUp;
			}
			else
				m_JumpIdleState = JumpIdleAni::JumpIdle_JumpDown;
			*beforePosY = afterPosY;
		}
	}
}

void CPlayer::Event_Skill(float fTimeDelta, Engine::CNaviMesh* pNaviMeshCom, _vec3 vPos, _vec3 vDir) {
	if (m_Attack_State == Attack_State::StateA_SkillF) {
		if (m_pMeshCom->Is_AnimationSetEnd(0.35f)) {
			Set_StateToAnimation(State::State_Idle);
		}
		float trackPos = m_pMeshCom->Get_AnimationTrackPos();
		if (!m_bCheck[bCheck::bCheck_Skill_F1] && trackPos > 0.7f) {
			m_bCheck[bCheck::bCheck_Skill_F1] = true;
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
		}
		else if (!m_bCheck[bCheck::bCheck_Skill_F2] && trackPos > 2.f) {
			m_bCheck[bCheck::bCheck_Skill_F2] = true;
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_VecDamagedObject.clear();
		}
		if (1.6f < trackPos && trackPos < 2.3f) {
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed * 0.5f), &m_dwNaviIndex));
		}
	}
	else if (m_Attack_State == Attack_State::StateA_SkillE) {
		if (m_pMeshCom->Is_AnimationSetEnd(0.3f)) {
			Set_StateToAnimation(State::State_Idle);
		}
		float trackPos = m_pMeshCom->Get_AnimationTrackPos();
		if (trackPos > 0.7f) {
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
		}
	}
	else if (m_Attack_State == Attack_State::StateA_SkillQ) {
		if (m_AniClip == AnimationClip::Ani_1) {
			if (m_pMeshCom->Is_AnimationSetEnd(0.05f)) {
				m_AniClip = AnimationClip::Ani_2;
				m_pMeshCom->Set_AnimationSet(124);
			}
		}
		else if (m_AniClip == AnimationClip::Ani_2) {
			bool colCheck;
			m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed * 4.f), &m_dwNaviIndex, &colCheck));
			if (colCheck) {
				m_AniClip = AnimationClip::Ani_3;
				Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
				m_pMeshCom->Set_AnimationSet(125);
			}
		}
		else if (m_AniClip == AnimationClip::Ani_3) {
			if (m_pMeshCom->Is_AnimationSetEnd(0.66f)) {
				Set_StateToAnimation(State::State_Idle);
			}
		}
	}
	else if (m_Attack_State == Attack_State::StateA_SkillZ) {
		if (m_pMeshCom->Is_AnimationSetEnd(0.1f)) {
			Set_StateToAnimation(State::State_Idle);
		}
		float trackPos = m_pMeshCom->Get_AnimationTrackPos();
		if (trackPos > 0.7f) {
			Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
		}
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

		//
		_vec3 _colUnitPos = _colUnit->m_pTransformCom->m_vInfo[Engine::INFO_POS];
		_vec3 _myUnitPos = m_pTransformCom->m_vInfo[Engine::INFO_POS];
		float _Radius = (_colUnit->m_pTransformCom->m_vScale.x * _colSphere->m_pTransformCom->m_vScale.x);
		_vec3 CollDir = _myUnitPos - _colUnitPos;
		CollDir.y = 0.f;
		D3DXVec3Normalize(&CollDir, &CollDir);
		Engine::CNaviMesh* pNaviMeshCom = dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->m_NaviTerrain->m_pNaviMeshCom;
		_vec3	vPos;
		m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
		m_pTransformCom->Set_Pos(&pNaviMeshCom->Move_OnNaviMesh(&vPos, &(CollDir * fTimeDelta * m_fSpeed), &m_dwNaviIndex));
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
					Get_BonePartCollider(CSphereCollider::BonePart_PlayerHammer)->m_WeaponAttack = true;
					m_pMeshCom->Set_AnimationSet(125);
				}
			}
		}
		else if (_colSphere->m_WeaponAttack) {
			if (m_TimeCheck[TimeCheck_Invin] <= 0.f) {
				m_fKnockBackPower = _colSphere->m_WeaponPower;
				Set_StateToAnimation(State::State_Damaged);
				m_TimeCheck[TimeCheck_Invin] = 4.f;
				m_fKnockBackDir = m_pTransformCom->m_vInfo[Engine::INFO_POS] - dynamic_cast<CUnit_D*>(_col)->m_pTransformCom->m_vInfo[Engine::INFO_POS];
				D3DXVec3Normalize(&m_fKnockBackDir, &m_fKnockBackDir);
			}
		}
	}
	//TODO: 점프해서 맞는거 아직 구현이 안되있음.
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
	m_TimeCheck[TimeCheck_Cool_Tab] = 2.f;
	if (m_WeaponEquip == Weapon_Equip::Weapon_Hammer) {
		m_Hammer->m_IsDead = true;
		m_Hammer = nullptr;
		Engine::CGameObject* pGameObject = CLongBow::Create(m_pGraphicDev);
		pLayer->Add_GameObject(L"CLongBow", pGameObject);

		m_WeaponEquip = Weapon_Equip::Weapon_LongBow;
	}
	else if (m_WeaponEquip == Weapon_Equip::Weapon_LongBow) {
		m_LongBow->m_IsDead = true;
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
	pUnit->m_pTransformCom->m_vInfo[Engine::INFO_POS] = _vPos + _vDir * 2.f;
	pUnit->m_pTransformCom->m_vInfo[Engine::INFO_POS].y += 1.2f + m_LookAtY * 0.2f;
	pUnit->m_vDir = _vDir;
	
	//pUnit->m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian((m_RocateY - 90.f) * 3.f)); // 절대 못구함 ㅅㅂ
	pUnit->m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(m_RocateY - 90.f)); // 합0도는 정상 합180도면 뒤바꿔야함
	pUnit->CUnit::Update_Object(fTimeDelta);
	pUnit->m_fAttack = m_fAttack;
	Engine::CLayer* pLayer = Engine::CManagement::GetInstance()->m_pScene->Get_Layer(Engine::CLayer::LayerName::Layer_Static);
	pLayer->Add_GameObject(L"Arrow", pGameObject);
}