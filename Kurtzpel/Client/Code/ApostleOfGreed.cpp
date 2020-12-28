#include "stdafx.h"
#include "ApostleOfGreed.h"
#include "Export_Function.h"
#include "DynamicCamera.h"
#include "SphereCollider.h"
#include "Player.h"

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
	m_fInitSpeed = 6.f;
	m_fSpeed = m_fInitSpeed;
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

	pComponent = m_pNaviMeshCom = dynamic_cast<Engine::CNaviMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_Navi"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Navi", pComponent);
	//
	m_pTransformCom->Set_Pos(&Engine::_vec3{ 25.f, 0.f, 25.f });
	//m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);
	Engine::CGameObject::Update_Object(1.f);

	//m_pMeshCom->Play_Animation(1.f);

	Load_ColliderFile(L"../Bin/Resource/Mesh/DynamicMesh/Save/ApostleOfGreed.dat");
	for (auto& sphere : m_VecSphereCollider)
	{
		sphere->m_pDynamicMesh = this;
	}
	m_pTransformCom->Set_Pos(&_vec3(5.f, 0.f, 5.f));
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

	m_pTransformCom->Set_Scale(0.01f, 0.01f, 0.01f);
	m_pMeshCom->Set_AnimationSet(25);

	m_pNaviMeshCom->Set_NaviIndex(0);

	return S_OK;
}

Client::_int Client::CApostleOfGreed::Update_Object(const _float& fTimeDelta)
{

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

	m_pNaviMeshCom->Render_NaviMeshes();


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

}

void Client::CApostleOfGreed::Pattern(_float fTimeDelta)
{
	float playerTodisTance = PlayerSearchDistance();
	CPlayer* player = CPlayer::GetInstance();

	if (m_State == State::State_Attack1) {
		if (!m_pMeshCom->Is_AnimationSetEnd()) {
			return;
		}
		else {
			m_State = State::State_Wait;
			m_pMeshCom->Set_AnimationSet(25);
		}
	}
	else if (playerTodisTance < 10.f) {
		m_isSearch = true;
		
		if (playerTodisTance < 5.5f) {
			m_State = State::State_Attack1;
			m_pMeshCom->Set_AnimationSet(17);
		}
		else {
			m_State = State::State_Move;
			m_pMeshCom->Set_AnimationSet(28);
		}
		
	}
	else {
		m_isSearch = false;
	}

	if (m_isSearch) {
		if (m_State == State::State_Move) {
			m_vDir = m_pPlayerTrans->m_vInfo[Engine::INFO_POS] - m_pTransformCom->m_vInfo[Engine::INFO_POS];
			_vec3 afterPos = m_pTransformCom->m_vInfo[Engine::INFO_POS] + (m_vDir * fTimeDelta * m_fSpeed);
			//m_pTransformCom->Set_Pos(&afterPos);
			m_pMeshCom->Set_AnimationSet(28);
			//
			_vec3 monsterPos = m_pTransformCom->m_vInfo[Engine::INFO_POS];
			_vec3 playerPos = m_pPlayerTrans->m_vInfo[Engine::INFO_POS];
			_vec3 monsterDir = playerPos - monsterPos;
			D3DXVec3Normalize(&monsterDir, &monsterDir);

			_vec3 monsterLook = m_pTransformCom->m_vInfo[Engine::INFO_LOOK];
			D3DXVec3Normalize(&monsterLook, &monsterLook);
			float radian = D3DXVec3Dot(&monsterDir, &monsterLook);
			float angle = D3DXToDegree(radian);
			_vec3 monsterRight = m_pTransformCom->m_vInfo[Engine::INFO_RIGHT];
			D3DXVec3Normalize(&monsterLook, &monsterRight);
			float radian2 = D3DXVec3Dot(&monsterDir, &monsterRight);
			float angle2 = D3DXToDegree(radian2);

			// 뒤쪽
			if (angle < 0.f) {
				// 오른쪽
				if (angle2 >= 0.f) {
					m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(-40.f) * fTimeDelta);
				}
				// 왼쪽
				else {
					m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(40.f) * fTimeDelta);
				}
			}
			// 앞쪽
			else {
				// 오른쪽
				if (angle2 >= 0.f) {
					m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(-40.f) * fTimeDelta);
				}
				// 왼쪽
				else {
					m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(40.f) * fTimeDelta);
				}
			}


			//_vec3 m1 = m_pPlayerTrans->m_vInfo[Engine::INFO_POS];
			//_vec3 p1 = m_pTransformCom->m_vInfo[Engine::INFO_POS];
			//float cosAngle = acosf(D3DXVec3Dot(&p1, &m1) / (D3DXVec3Length(&p1) * D3DXVec3Length(&m1)));
			//cosAngle = D3DXToDegree(cosAngle);
			//
			//float angle = (p1.x * m1.y - p1.y * m1.x > 0.0f) ? cosAngle : -cosAngle;
			//m_pMeshCom->Set_AniAngle(D3DXToRadian(angle));
			//
		}
		else if (m_State == State::State_Attack1) {
			m_pMeshCom->Set_AnimationSet(17);
		}
	}
	else {
		m_pMeshCom->Set_AnimationSet(25);
	}
}

void Client::CApostleOfGreed::Collision(Engine::CGameObject* _col)
{
	int i = 0;
}