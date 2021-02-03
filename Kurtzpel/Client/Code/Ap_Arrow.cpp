#include "stdafx.h"
#include "Ap_Arrow.h"
#include "Export_Function.h"
#include "Player.h"
#include "EffectRcTex.h"
#include "SoundManager.h"

CAp_Arrow::CAp_Arrow(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit(pGraphicDev)
{
	m_sStat.m_fSpeed = 40.f;
	m_LifeTime = 2.5f;
}

CAp_Arrow::~CAp_Arrow(void)
{

}

HRESULT Client::CAp_Arrow::Add_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;
	CUnit::Add_Component();
	// Mesh
	pComponent = m_sComponent.m_pStaticMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_AP_Arrow"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);


	//m_pTransformCom->m_vInfo[Engine::INFO_POS] = { 5.f, -15.f, 10.f };
	m_sComponent.m_pTransformCom->m_vScale = { 0.015f, 0.015f, 0.015f };
	

	return S_OK;
}


CAp_Arrow* CAp_Arrow::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAp_Arrow*	pInstance = new CAp_Arrow(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CAp_Arrow::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CAp_Arrow::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_UnitName = UnitName::PlayerBullet;
	return S_OK;
}
Client::_int Client::CAp_Arrow::Update_Object(const _float& fTimeDelta)
{
	if (m_sStat.m_IsDead || m_LifeTime < 0.f) {
		return 1;
	}
	/*if (m_Effect) {
		_vec3	vPos;
		m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
		if (vPos.y < 1.5f) {
			CEffectRcTex::Create(m_pGraphicDev)->Set_Effect(true, vPos, 3.5f, L"Texture_Effect_LBHit01", 3, 3, 30.f);
			int voiceNumber = CRandom_Manager::Random() % 5 + 1;
			_tchar		szFileName[256] = L"";

			wsprintf(szFileName, L"Arrow%d.ogg", voiceNumber);
			SoundManager::PlayOverlapSound(szFileName, SoundChannel::PLAYER, 0.1f);
			return 1;
		}
	}*/
	m_LifeTime -= fTimeDelta;
	
	CUnit::Update_Object(fTimeDelta);

	_vec3	vPos, vPosAfter;
	m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	vPosAfter = vPos + m_sStat.m_vDir * m_sStat.m_fSpeed * 5.f;// * fTimeDelta;
	//m_pTransformCom->Set_Pos(&vPosAfter, true);
	m_sComponent.m_pTransformCom->Chase_Target(&vPosAfter, m_sStat.m_fSpeed, fTimeDelta);

	m_sComponent.m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(90.f), true);
	m_sComponent.m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(180.f), true);

	m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);

	return 0;
}
void Client::CAp_Arrow::Render_Object(void)
{
	
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	LPD3DXEFFECT	 pEffect = m_sComponent.m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);
	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
	pEffect->BeginPass(0);

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );
	
	m_sComponent.m_pStaticMeshCom->Render_Meshes(pEffect);

	pEffect->EndPass();
	pEffect->End();


	Engine::Safe_Release(pEffect);
	/*_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pColliderCom->Render_Collider(Engine::COL_TRUE, &matWorld);*/
}

HRESULT Client::CAp_Arrow::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
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

void Client::CAp_Arrow::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{
	if (_mySphere->m_BoneTeam == _colSphere->m_BoneTeam)
		return;
	if (_colSphere->m_BonePart == CSphereCollider::BonePart::BonePart_CollBody) {
		m_sStat.m_IsDead = true;
		Sound_RandomPlay(UnitSound::Sound_Arrow_Hit);
	}
}

void Client::CAp_Arrow::Create_Coll()
{
	CSphereCollider* sphereCol = CSphereCollider::Create(m_pGraphicDev);
	sphereCol->Set_Transform(1.f);
	sphereCol->m_pStaticThis = this;
	sphereCol->m_WeaponAttack = true;
	sphereCol->m_BoneTeam = CSphereCollider::BoneTeam::BoneTeam_Enemy;
	sphereCol->m_BonePart = CSphereCollider::BonePart::BonePart_Static;
	sphereCol->Engine::CGameObject::Update_Object(0.f);
	m_VecSphereCollider.emplace_back(sphereCol);
}

void Client::CAp_Arrow::Set_SpeedToLife(float _speed, float _life)
{
	m_sStat.m_fSpeed = _speed;
	if (_life > 0)
		m_LifeTime = _life;
}