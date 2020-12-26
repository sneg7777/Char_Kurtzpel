#include "stdafx.h"
#include "Player.h"
#include "Export_Function.h"
#include "DynamicCamera.h"
#include "SphereCollider.h"

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
	m_fJumpPower = 0.16f;
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

void Client::CPlayer::Key_Input(const _float& fTimeDelta)
{
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &m_vDir);
	_vec3	vPos, vDir, vDir2;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
	D3DXVec3Normalize(&vDir, &vDir);

	Key_DoubleInput(fTimeDelta);
	if (m_State == State::State_Damaged) {
		if (m_pMeshCom->Is_AnimationSetEnd()) {
			m_State = State::State_Idle;
			m_fSpeed = m_fInitSpeed;
		}
		else {
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(-vDir * fTimeDelta * m_fSpeed * 1.5f)));
			m_fSpeed -= m_fInitSpeed * fTimeDelta * 2.5f;
			return;
		}
	}
	if (m_State == State::State_Dash) {
		m_bCheck[bCheck::bCheck_MoveAni] = true;
		
		if (m_TimeCheck[TimeCheck::TimeCheck_Dash] < 0.f)
		{
			m_State = State::State_Idle;
			m_bCheck[bCheck::bCheck_KeyW] = false;
			m_bCheck[bCheck::bCheck_KeyA] = false;
			m_bCheck[bCheck::bCheck_KeyS] = false;
			m_bCheck[bCheck::bCheck_KeyD] = false;
			m_bCheck[bCheck::bCheck_DBKeyW] = false;
			m_bCheck[bCheck::bCheck_DBKeyA] = false;
			m_bCheck[bCheck::bCheck_DBKeyS] = false;
			m_bCheck[bCheck::bCheck_DBKeyD] = false;
			m_fSpeed = m_fInitSpeed;
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

		m_fSpeed = 3.5f * m_fInitSpeed;
		m_AniSpeed = 1.5f;
		m_pMeshCom->Set_AnimationSet(266);
	}
	else if (Engine::Get_DIKeyState(DIK_W) & 0x80)
	{
		m_bCheck[bCheck::bCheck_MoveAni] = true;
		m_State = State::State_Move;
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
			m_State = State::State_Dash;
			m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
		}
		else {
			if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
				vDir2 = { -vDir.z, 0.f, vDir.x };
				vDir2 += vDir;
				D3DXVec3Normalize(&vDir2, &vDir2);
				m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));

				m_AniSpeed = 1.f;
				//m_pMeshCom->Set_AniAngle(175.f);
				//m_pMeshCom->Set_AnimationSet(256);
				m_pMeshCom->Set_AniAngle(220.f);
				m_pMeshCom->Set_AnimationSet(260);
			}
			else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
				vDir2 = { vDir.z, 0.f, -vDir.x };
				vDir2 += vDir;
				D3DXVec3Normalize(&vDir2, &vDir2);
				m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));

				m_AniSpeed = 1.f;
				//m_pMeshCom->Set_AniAngle(355.f);
				//m_pMeshCom->Set_AnimationSet(258);
				m_pMeshCom->Set_AniAngle(310.f);
				m_pMeshCom->Set_AnimationSet(260);
			}
			else {
				m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * m_fSpeed)));

				m_AniSpeed = 1.f;
				m_pMeshCom->Set_AniAngle(265.f);
				m_pMeshCom->Set_AnimationSet(260);
				//m_bCheck[bCheck::bCheck_RunW] = true;
			}
		}
	}
	else if (Engine::Get_DIKeyState(DIK_S) & 0x80)
	{
		m_bCheck[bCheck::bCheck_MoveAni] = true;
		m_State = State::State_Move;
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
			m_State = State::State_Dash;
			m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
		}
		else if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed)));

			m_AniSpeed = 1.f;
			m_pMeshCom->Set_AniAngle(175.f);
			m_pMeshCom->Set_AnimationSet(258);
		}
		else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			vDir2 += vDir;
			D3DXVec3Normalize(&vDir2, &vDir2);
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * -m_fSpeed)));

			m_AniSpeed = 1.f;
			m_pMeshCom->Set_AniAngle(355.f);
			m_pMeshCom->Set_AnimationSet(256);
		}
		else {
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir * fTimeDelta * -m_fSpeed)));

			m_AniSpeed = 1.f;
			m_pMeshCom->Set_AniAngle(85.f);
			m_pMeshCom->Set_AnimationSet(260);
		}
	}
	else if (Engine::Get_DIKeyState(DIK_A) & 0x80) {
		m_bCheck[bCheck::bCheck_MoveAni] = true;
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
			m_State = State::State_Dash;
			m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
		} 
		else {
			vDir2 = { -vDir.z, 0.f, vDir.x };
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));

			m_AniSpeed = 1.f;
			m_pMeshCom->Set_AniAngle(175.f);
			m_pMeshCom->Set_AnimationSet(260);
		}
	}
	else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
		m_bCheck[bCheck::bCheck_MoveAni] = true;
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
			m_State = State::State_Dash;
			m_TimeCheck[TimeCheck::TimeCheck_Dash] = 0.4f;
		}
		else {
			vDir2 = { vDir.z, 0.f, -vDir.x };
			m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));

			m_AniSpeed = 1.f;
			m_pMeshCom->Set_AniAngle(355.f);
			m_pMeshCom->Set_AnimationSet(260);
		}
	}

	if (Engine::Get_DIMouseState(Engine::DIM_LB) & 0x80)
	{
		_vec3	vPickPos = PickUp_OnTerrain();
		m_pTransformCom->Pick_Pos(&vPickPos, m_fSpeed, fTimeDelta);
		
	}
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
	if (m_State == State::State_Move && !(Engine::Get_DIKeyState(DIK_W) & 0x80) && !(Engine::Get_DIKeyState(DIK_S) & 0x80) && !(Engine::Get_DIKeyState(DIK_A) & 0x80) && !(Engine::Get_DIKeyState(DIK_D) & 0x80)) {
		m_State = State::State_Idle;
	}

	if (m_State != State::State_Dash && Engine::Get_DIKeyState(DIK_SPACE) & 0x80) {
		m_JumpIdleState = JumpIdleAni::JumpIdle_JumpStart;
	}
	// Jump 가 끝났을때 모션
	if (m_State == State::State_JumpEnd) {
		if (m_AniTime < 0.f) {
			m_State = State::State_Idle;
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.f;
			m_pMeshCom->Set_AnimationSet(243);
		}
		else {
			m_fSpeed = m_fInitSpeed;
			m_AniSpeed = 1.f;
			m_pMeshCom->Set_AnimationSet(277);
		}
		m_bCheck[bCheck::bCheck_MoveAni] = false;
		return;
	}
	// Idle 모션
	else if (m_State == State::State_Idle) {
		m_fSpeed = m_fInitSpeed;
		m_pMeshCom->Set_AnimationSet(243);
		m_bCheck[bCheck::bCheck_MoveAni] = false;
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
		/*m_AniSpeed = 1.f;
		m_pMeshCom->Set_AnimationSet(273);
		if (m_AniTime < 0.f)*/
			m_JumpIdleState = JumpIdleAni::JumpIdle_JumpUp;
		//
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
		m_bCheck[bCheck::bCheck_MoveAni] = true;

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
		m_bCheck[bCheck::bCheck_MoveAni] = true;

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
		m_bCheck[bCheck::bCheck_MoveAni] = true;

		vDir2 = { -vDir.z, 0.f, vDir.x };
		m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));

		m_pMeshCom->Set_AniAngle(175.f);
		
	}
	else if (Engine::Get_DIKeyState(DIK_D) & 0x80) {
		m_bCheck[bCheck::bCheck_MoveAni] = true;

		vDir2 = { vDir.z, 0.f, -vDir.x };
		m_pTransformCom->Set_Pos(&m_pNaviMeshCom->Move_OnNaviMesh(&vPos, &(vDir2 * fTimeDelta * m_fSpeed)));

		m_pMeshCom->Set_AniAngle(355.f);
		
	}


	//if (!m_bCheck[bCheck::bCheck_MoveAni] && true == m_pMeshCom->Is_AnimationSetEnd() && m_AniEnd) {
	//	m_AniSpeed = 1.f;
	//	//m_pMeshCom->Set_AniAngle(265.f);
	//	m_pMeshCom->Set_AnimationSet(243);
	//	m_AniEnd = false;
	//}

	//if (m_State == State::State_Idle && m_bCheck[bCheck::bCheck_MoveAni] && !(Engine::Get_DIKeyState(DIK_W) & 0x80) && !(Engine::Get_DIKeyState(DIK_S) & 0x80) && !(Engine::Get_DIKeyState(DIK_A) & 0x80) && !(Engine::Get_DIKeyState(DIK_D) & 0x80)) {
	//	m_AniEnd = true;
	//	m_fSpeed = m_fInitSpeed;
	//	m_pMeshCom->Set_AnimationSet(243);
	//	m_bCheck[bCheck::bCheck_MoveAni] = false;
	//}
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
	m_pMeshCom->Set_AnimationSet(243);

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
	if (m_AniTime > 0.f)
		m_AniTime -= fTimeDelta;
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
}

void Client::CPlayer::Jump_Control(const _float& fTimeDelta)
{
	_vec3	vPosition;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPosition, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	if(m_fJumpAccel < 2.2f)
		m_fJumpAccel += 3.2f * fTimeDelta;
	float gravity = m_fJumpPower * m_fJumpAccel - (GRAVITY * m_fJumpAccel * m_fJumpAccel * 0.5f);

	float* beforePosY = &m_pTransformCom->m_vInfo[Engine::INFO_POS].y;
	float afterPosY = gravity + *beforePosY;

	if (fHeight > afterPosY) {
		m_JumpIdleState = JumpIdleAni::JumpIdle_None;
		m_AniTime = 0.6f;
		//m_pMeshCom->Is_Loop_Change(FALSE);
		m_State = State::State_JumpEnd;
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
		m_State = State::State_Damaged;
		m_pMeshCom->Set_AnimationSet(160);
		m_TimeCheck[TimeCheck_Invin] = 2.f;
		m_AniSpeed = 1.5f;
	}
}