#include "stdafx.h"
#include "Player.h"
#include "Export_Function.h"
#include "DynamicCamera.h"
#include "SphereCollider.h"
#include "Hammer.h"

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
	m_fInitSpeed = 10.f;
	m_fJumpPower = 0.125f;
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

	pComponent = m_pNaviMeshCom = dynamic_cast<Engine::CNaviMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Navi"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Navi", pComponent);

	//
	m_pTransformCom->Set_Pos(&Engine::_vec3{ 3.f, 0.f, 3.f });
	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	Engine::CGameObject::Update_Object(1.f);

	//m_pMeshCom->Play_Animation(1.f);
	//Engine::CRenderer::GetInstance()->Render_GameObject(m_pGraphicDev);
	//
	Load_ColliderFile(L"../Bin/Resource/Mesh/DynamicMesh/Save/Player.dat");
	for (auto& sphere : m_VecSphereCollider)
	{
		sphere->m_pDynamicMesh = this;
	}
	return S_OK;
}

void Client::CPlayer::Set_StateToAnimation(State _state) {
	switch (_state)
	{
	case Client::CPlayer::State_Idle: {
		m_pMeshCom->Set_AnimationSet(243);
		m_AniSpeed = 1.f;
		if (m_WeaponEquip == Weapon_Equip::Weapon_Hammer)
			m_Hammer->Set_Pos();
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
		m_fSpeed = 3.5f * m_fInitSpeed;
		m_AniSpeed = 1.8f;
		break;
	}
	case Client::CPlayer::State_Attack: {
		m_Attack_State = (Attack_State)((int)m_Attack_State + 1);
		if (m_Attack_State == Attack_State::StateA_End)
			m_Attack_State = Attack_State::StateA_Basic1;
		m_fSpeed = m_fInitSpeed;
		m_AniSpeed = 1.3f;
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
		m_pMeshCom->Set_AnimationSet(160);
		m_fSpeed = m_fInitSpeed;
		m_AniSpeed = 1.5f;
		break;
	}
	case Client::CPlayer::State_End: {
		break;
	}
	default:
		break;
	}
	m_State = _state;
}

void Client::CPlayer::Key_Input(const _float& fTimeDelta)
{
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &m_vDir);
	_vec3	vPos, vDir, vDir2;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
	D3DXVec3Normalize(&vDir, &vDir);
	
	Key_DoubleInput(fTimeDelta);
	// 피격 당할때
	if (m_State == State::State_Damaged) {
		if (m_pMeshCom->Is_AnimationSetEnd()) {
			Set_StateToAnimation(State::State_Idle);
		}
		else {
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(-vDir * fTimeDelta * m_fSpeed * 1.5f)));
			m_fSpeed -= m_fInitSpeed * fTimeDelta * 2.5f;
			return;
		}
	}
	else if (m_State == State::State_Attack) {
		if (m_bCheck[bCheck::bCheck_MouseL_Already]) {
			if (m_pMeshCom->Is_AnimationSetEnd(0.65f)) {
				Set_StateToAnimation(State::State_Attack);
				m_bCheck[bCheck::bCheck_MouseL_Already] = false;
			}
		}
		else if (m_pMeshCom->Is_AnimationSetEnd(0.3f)) {
			Set_StateToAnimation(State::State_Idle);
			m_Attack_State = Attack_State::StateA_None;
		}
		else if((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) && (m_Attack_State == Attack_State::StateA_Basic1 || m_Attack_State == Attack_State::StateA_Basic2)){
			float aniPos = m_pMeshCom->Get_AnimationTrackPos();

			if (aniPos > 0.35f) {
				m_bCheck[bCheck::bCheck_MouseL_Already] = true;
			}
		}
	}
	else if (m_State == State::State_Dash) {
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
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed)));
			m_pMeshCom->Set_AniAngle(265.f);
		}
		else if (m_EnumDir == EnumDir::UpLeft) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));
			m_pMeshCom->Set_AniAngle(220.f);
		}
		else if (m_EnumDir == EnumDir::UpRight) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));
			m_pMeshCom->Set_AniAngle(310.f);
		}
		else if (m_EnumDir == EnumDir::Down) {
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_fSpeed)));
			m_pMeshCom->Set_AniAngle(85.f);
		}
		else if (m_EnumDir == EnumDir::DownLeft) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed)));
			m_pMeshCom->Set_AniAngle(130.f);
		}
		else if (m_EnumDir == EnumDir::DownRight) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed)));
			m_pMeshCom->Set_AniAngle(40.f);
		}
		else if (m_EnumDir == EnumDir::Left) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));
			m_pMeshCom->Set_AniAngle(175.f);
		}
		else if (m_EnumDir == EnumDir::Right) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));
			m_pMeshCom->Set_AniAngle(355.f);
		}

	}

	//////////////////////////////////////////////////////////////////// 좌클릭공격
	else if ((Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80) && !m_bCheck[bCheck::bCheck_MouseL])
	{
		m_bCheck[bCheck::bCheck_MouseL] = true;
		Set_StateToAnimation(State::State_Attack);
		m_bCheck[bCheck::bCheck_MouseL_Already] = false;

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
				m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));

				m_pMeshCom->Set_AniAngle(220.f);
				Set_StateToAnimation(State::State_Move);
			}
			else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
				vDir2 = { vDir.z, 0.f, -vDir.x };
				vDir2 += vDir;
				D3DXVec3Normalize(&vDir2, &vDir2);
				m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));

				m_pMeshCom->Set_AniAngle(310.f);
				Set_StateToAnimation(State::State_Move);
			}
			else {
				m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed)));

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
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed)));

			m_pMeshCom->Set_AniAngle(175.f);
			Set_StateToAnimation(State::State_MoveSA);
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed)));

			m_pMeshCom->Set_AniAngle(355.f);
			Set_StateToAnimation(State::State_MoveSD);
		}
		else {
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_fSpeed)));

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
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));

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
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));

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

	if ((m_State == State::State_Move || m_State == State::State_MoveSA || m_State == State::State_MoveSD)
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
		m_fSpeed = m_fInitSpeed;
		Set_StateToAnimation(State::State_Idle);
	}


}

void Client::CPlayer::Key_InputOfJump(const _float& fTimeDelta)
{
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &m_vDir);
	_vec3	vPos, vDir, vDir2;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
	D3DXVec3Normalize(&vDir, &vDir);

	switch (m_JumpIdleState)
	{
	case JumpIdleAni::JumpIdle_JumpStart: {
		//애니메이션 그전꺼를 저장하는거 땜에 좀 씹혀서 스킵함
		//m_pMeshCom->Set_AnimationSet(273);
		//이유를 알앗는데 귀찮네 Is_AnimationSetEnd() 사용법을 잘 몰라서 생긴 문제
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
	case JumpIdleAni::JumpIdle_End: {
		return;
		break;
	}
	default:
		break;
	}

	if (Engine::Get_DIKeyState(DIK_W) & 0x80)
	{
		if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));

			m_pMeshCom->Set_AniAngle(220.f);
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));

			m_pMeshCom->Set_AniAngle(310.f);
		}
		else {
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed)));

			m_pMeshCom->Set_AniAngle(265.f);
		}
	}
	else if (Engine::Get_DIKeyState(DIK_S) & 0x80)
	{
		if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed)));

			m_pMeshCom->Set_AniAngle(175.f);
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed)));

			m_pMeshCom->Set_AniAngle(355.f);
		}
		else {
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_fSpeed)));

			m_pMeshCom->Set_AniAngle(85.f);
		}
	}
	else if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
		vDir2 = { -vDir.z, 0.f, vDir.x };
		m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));

		m_pMeshCom->Set_AniAngle(175.f);
		
	}
	else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
		vDir2 = { vDir.z, 0.f, -vDir.x };
		m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));

		m_pMeshCom->Set_AniAngle(355.f);
		
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

	m_pNaviMeshCom->Set_NaviIndex(0);

	return S_OK;
}
Client::_int Client::CPlayer::Update_Object(const _float& fTimeDelta)
{

	Calc_Time(fTimeDelta);
	if (m_JumpIdleState != JumpIdleAni::JumpIdle_None) {
		Key_InputOfJump(fTimeDelta);
		Jump_Control(fTimeDelta);
	}
	else {
		SetUp_OnTerrain();
		Key_Input(fTimeDelta);
	}
	//
	//CameraControl(fTimeDelta);
	//

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

	m_pNaviMeshCom->Render_NaviMeshes();


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

	if(m_fJumpAccel < 1.42f)
		m_fJumpAccel += 2.4f * fTimeDelta;
	float gravity = m_fJumpPower * m_fJumpAccel - (GRAVITY * m_fJumpAccel * m_fJumpAccel * 0.5f);

	float* beforePosY = &m_pTransformCom->m_vInfo[Engine::INFO_POS].y;
	float afterPosY = gravity + *beforePosY;

	if (fHeight > afterPosY) {
		m_JumpIdleState = JumpIdleAni::JumpIdle_None;
		Set_StateToAnimation(State::State_JumpEnd);
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

void Client::CPlayer::Collision(Engine::CGameObject* _col)
{
	if (m_TimeCheck[TimeCheck_Invin] <= 0.f) {
		Set_StateToAnimation(State::State_Damaged);
		m_TimeCheck[TimeCheck_Invin] = 2.f;
	}
}