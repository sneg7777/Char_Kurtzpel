#include "stdafx.h"
#include "DynamicCamera.h"
#include "Export_Function.h"
#include "Player.h"
#include "NpcQuest_Manager.h"
#include "Stage.h"

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
	if (CNpcQuest_Manager::Get_Instance()->Get_NpcQuestInfo()->m_PlayerTalk) {
		CUnit_D* pNpc = dynamic_cast<CUnit_D*>(Engine::CManagement::GetInstance()->m_pScene->Get_LayerObject(Engine::CLayer::Layer_Dynamic, Engine::CGameObject::UnitName::Npc));
		if (pNpc == nullptr)
			return 0;
		_vec3 vPos, vDir, vMoveDir;
		Engine::CTransform* pNpcTrans = pNpc->Get_sComponent()->m_pTransformCom;
		pNpcTrans->Get_Info(Engine::INFO_POS, &vPos);
		vPos.y += 2.f;
		pNpcTrans->Get_Info(Engine::INFO_LOOK, &vDir);
		//
		vMoveDir = (vPos + (vDir * 150.f)) - m_vEye;
		float dist = sqrtf(vMoveDir.x * vMoveDir.x + vMoveDir.y * vMoveDir.y + vMoveDir.z * vMoveDir.z);
		if (dist < 0.5f) {
			m_vEye = vPos + (vDir * 150.f);
		}
		else {
			D3DXVec3Normalize(&vMoveDir, &vMoveDir);
			m_vEye += vMoveDir * fTimeDelta * 10.f;
		}
		//
		m_vAt = vPos;
	}
	else {
		Key_Input(fTimeDelta);


		CPlayer* player = CPlayer::GetInstance();
		if (player == nullptr)
			return 0;
		_vec3	vPos, vDir;
		player->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
		player->Get_sComponent()->m_pTransformCom->Get_Info(Engine::INFO_LOOK, &vDir);
		m_vEye = vPos - (vDir * player->m_CameraDist);
		m_vEye.y += 3.5f - player->m_LookAtY * 0.7f;
		m_vAt = vPos + (vDir * 200.f);
		m_vAt.y += player->m_LookAtY;

		if (true == m_bFix)
		{
			Mouse_Fix();
			Mouse_Move();
		}
	}
	_int iExit = Engine::CCamera::Update_Object(fTimeDelta);
	return iExit;
}
