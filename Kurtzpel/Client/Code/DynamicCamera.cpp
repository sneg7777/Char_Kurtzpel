#include "stdafx.h"
#include "DynamicCamera.h"
#include "Export_Function.h"
#include "Player.h"
#include "NpcQuest_Manager.h"
#include "Stage.h"
#include "Random_Manager.h"
#include "CameraScene_Manager.h"

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)	
{

}

CDynamicCamera::~CDynamicCamera(void)
{

}

HRESULT Client::CDynamicCamera::Ready_Object(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;
	m_fFovY = fFovY;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	FAILED_CHECK_RETURN(Engine::CCamera::Ready_Object(), E_FAIL);
	
	return S_OK;
}

Client::CDynamicCamera* Client::CDynamicCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev,
	const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, 
	const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	CDynamicCamera*	pInstance = new CDynamicCamera(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar)))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void Client::CDynamicCamera::Key_Input(const _float& fTimeDelta)
{
	_matrix			matCamWorld;
	D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);

	/*if (Engine::Get_DIKeyState(DIK_W) & 0x80)
	{
		_vec3		vLook;
		memcpy(vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * 5.f * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}
	if (Engine::Get_DIKeyState(DIK_S) & 0x80)
	{
		_vec3		vLook;
		memcpy(vLook, &matCamWorld.m[2][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * 5.f * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Engine::Get_DIKeyState(DIK_A) & 0x80)
	{
		_vec3		vRight;
		memcpy(vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * 5.f * fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
	}

	if (Engine::Get_DIKeyState(DIK_D) & 0x80)
	{
		_vec3		vRight;
		memcpy(vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * 5.f * fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}*/
	
	if (Engine::Get_DIKeyState(DIK_LCONTROL) & 0x80)
	{
		if (true == m_bClick)
			return;

		m_bClick = true;

		if (true == m_bFix)
			m_bFix = false;

		else
			m_bFix = true;
	}
	else
		m_bClick = false;

	if (false == m_bFix)
		return;
}

void Client::CDynamicCamera::Mouse_Move(void)
{
	//if (CPlayer::GetInstance()->m_State == CPlayer::State_Dash)
	//	return;


	_matrix			matCamWorld;
	D3DXMatrixInverse(&matCamWorld, NULL, &m_matView);

	_long	dwMouseMove = 0;
	// 마우스를 위아래로 움직일 때
	if (dwMouseMove = Engine::Get_DIMouseMove(Engine::DIMS_Y))
	{
		//CPlayer* player = dynamic_cast<CPlayer*>(Engine::CManagement::GetInstance()->Get_GameObject(L"GameLogic", L"Player"));
		m_Player->Add_LookAtY(-dwMouseMove * 0.005f);
		/*_vec3		vRight;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

		_vec3	vLook = m_vAt - m_vEye;
		_matrix		matRot;
		D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.f));

		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;*/
	}

	CPlayer::State pState = CPlayer::GetInstance()->m_State;
	if (!(pState == CPlayer::State_Idle || pState == CPlayer::State_Move || pState == CPlayer::State_MoveSA || pState == CPlayer::State_MoveSD
		|| pState == CPlayer::State_JumpEnd || pState == CPlayer::State_Damaged) && (CPlayer::GetInstance()->m_LB_Arrow_Count == 0))
		return;

	if (dwMouseMove = Engine::Get_DIMouseMove(Engine::DIMS_X))
	{
		m_Player->Get_sComponent()->m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(dwMouseMove * 0.1f));
		m_Player->m_RocateY += dwMouseMove * 0.1f;
		if (m_Player->m_RocateY > 360.f)
			m_Player->m_RocateY -= 360.f;
		else if (m_Player->m_RocateY < 0.f)
			m_Player->m_RocateY += 360.f;
		/*_vec3		vUp = _vec3(0.f, 1.f, 0.f);
		
		_vec3	vLook = m_vAt - m_vEye;
		_matrix		matRot;
		D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove / 10.f));

		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;
		*/
	}

	
}

void Client::CDynamicCamera::Mouse_Fix(void)
{
	POINT	ptMouse{ WINCX >> 1, WINCY >> 1 };
	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);

}

void CDynamicCamera::Free(void)
{
	Engine::CCamera::Free();
}

Client::_int Client::CDynamicCamera::Update_Object(const _float& fTimeDelta)
{
	if (CCameraScene_Manager::Get_Instance()->Get_CameraScene() != 0) {

	}
	else if (CNpcQuest_Manager::Get_NpcQuestInfo()->m_PlayerTalk) {
		Move_NpcCamera(fTimeDelta);
	}
	else {
		Key_Input(fTimeDelta);
		Move_NpcCamera_No(fTimeDelta);

	

		if (true == m_bFix)
		{
			Mouse_Fix();
			Mouse_Move();
		}
		Shake_CameraMove(fTimeDelta);
	}
	_int iExit = Engine::CCamera::Update_Object(fTimeDelta);
	return iExit;
}

void CDynamicCamera::Move_NpcCamera(const _float& fTimeDelta)
{
	CUnit_D* pNpc = dynamic_cast<CUnit_D*>(Engine::CManagement::GetInstance()->m_pScene->Get_LayerObject(Engine::CLayer::Layer_Dynamic, Engine::CGameObject::UnitName::Npc));
	if (pNpc == nullptr)
		return;
	_vec3 vPos, vDir, vMoveDir;
	Engine::CTransform* pNpcTrans = pNpc->Get_sComponent()->m_pTransformCom;
	pNpcTrans->Get_Info(Engine::INFO_POS, &vPos);
	vPos.y += 2.f;
	pNpcTrans->Get_Info(Engine::INFO_LOOK, &vDir);
	//
	vMoveDir = (vPos + (vDir * 150.f)) - m_vEye;
	float dist = sqrtf(vMoveDir.x * vMoveDir.x + vMoveDir.y * vMoveDir.y + vMoveDir.z * vMoveDir.z);
	if (dist < 0.3f) {
		m_vEye = vPos + (vDir * 150.f);
	}
	else {
		D3DXVec3Normalize(&vMoveDir, &vMoveDir);
		m_vEye += vMoveDir * fTimeDelta * 10.f;
	}
	//
	m_vAt = vPos;
	
	return;
}

void CDynamicCamera::Move_NpcCamera_No(const _float& fTimeDelta)
{
	CPlayer* player = CPlayer::GetInstance();
	if (player == nullptr)
		return;

	_vec3	vEyePos, vEyeDir, vPlayerPos, vPlayerDir;
	player->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_POS, &vPlayerPos);
	player->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vPlayerDir);
	if (CNpcQuest_Manager::Get_NpcQuestInfo()->m_TalkEnd) {
		vEyePos = vPlayerPos - (vPlayerDir * player->m_CameraDist);
		vEyePos.y += 3.5f - player->m_LookAtY * 0.7f;

		vEyeDir = vEyePos - m_vEye;
		float dist = sqrtf(vEyeDir.x * vEyeDir.x + vEyeDir.y * vEyeDir.y + vEyeDir.z * vEyeDir.z);
		if (dist < 0.3f) {
			CNpcQuest_Manager::Get_NpcQuestInfo()->m_TalkEnd = false;
		}
		else {
			D3DXVec3Normalize(&vEyeDir, &vEyeDir);
			m_vEye += vEyeDir * fTimeDelta * 15.f;
		}
	}
	else {
		m_vEye = vPlayerPos - (vPlayerDir * player->m_CameraDist);
		m_vEye.y += 3.5f - player->m_LookAtY * 0.7f;
	}
	m_vAt = vPlayerPos + (vPlayerDir * 200.f);
	m_vAt.y += player->m_LookAtY;

	return;
}

void CDynamicCamera::Shake_CameraMove(const _float& fTimeDelta)
{
	if (m_ShakeTime <= 0.f)
		return;

	m_ShakeTime -= fTimeDelta;
	float shakePer = m_ShakeTime / m_InitShakeTime;
	m_ShakeX = (-0.5f * shakePer) + (CRandom_Manager::Random() % 20) * (0.05f * shakePer);
	m_ShakeY = (-0.5f * shakePer) + (CRandom_Manager::Random() % 20) * (0.05f * shakePer);

	m_vEye.x += m_ShakeX;
	m_vEye.y += m_ShakeY;

	return;
}