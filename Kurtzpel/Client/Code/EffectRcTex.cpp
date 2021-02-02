#include "stdafx.h"
#include "EffectRcTex.h"
#include "Export_Function.h"
#include "Player.h"
#include "Stage_1.h"
#include "Stage_2.h"


CEffectRcTex::CEffectRcTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit(pGraphicDev)
{

}

CEffectRcTex::~CEffectRcTex(void)
{

}

HRESULT Client::CEffectRcTex::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

	//CUnit::Add_Component();

	//Mesh
	//pComponent = m_sComponent.m_pStaticMeshCom = dynamic_cast<Engine::CStaticMesh*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Mesh_GH_Trace03"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", pComponent);

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(Engine::Clone(Engine::RESOURCE_STATIC, L"Buffer_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	//Texture
	//pComponent = m_sComponent.m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, L"Texture_Effect_Portal"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);

	// Transform
	pComponent = m_sComponent.m_pTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Clone(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_sComponent.m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Safe_AddRef(pComponent);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Shader
	//pComponent = m_sComponent.m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Proto_Shader_Mesh"));
	pComponent = m_sComponent.m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Proto_Shader_EffectRcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	Engine::CGameObject::Update_Object(0.f);
	return S_OK;
}


CEffectRcTex* CEffectRcTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffectRcTex* pInstance = new CEffectRcTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Client::Safe_Release(pInstance);

	return pInstance;
}

void CEffectRcTex::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CEffectRcTex::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_UnitName = UnitName::Effect;
	//
	//_vec3 TempPos;
	//m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &TempPos);
	//TempPos.y = 0.5f;
	//m_sComponent.m_pTransformCom->Set_Pos(&TempPos, true);
	//
	return S_OK;
}
Client::_int Client::CEffectRcTex::Update_Object(const _float& fTimeDelta)
{
	if (m_sStat.m_IsDead) {
		return 1;
	}

	//m_sComponent.m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(1.f));
	m_EffectTime += fTimeDelta * m_EffectSpeed;
	if (m_EffectTime > m_uOffset * m_vOffset) {
		//m_EffectTime -= 16.f;
		m_sStat.m_IsDead = true;
	}
	//float effectSize =  0.01f + m_EffectTime * 0.001f;
	//m_sComponent.m_pTransformCom->m_vScale = { effectSize, effectSize, effectSize };
	
	//SetUp_OnTerrain();
	CUnit::Update_Object(0.f);
	m_sComponent.m_pRendererCom->Add_RenderGroup(Engine::RENDER_ALPHA, this);

	return 0;
}
void Client::CEffectRcTex::Render_Object(void)
{
	LPD3DXEFFECT	 pEffect = m_sComponent.m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
	pEffect->BeginPass(0);

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	m_pBufferCom->Render_Buffer();
	//m_sComponent.m_pStaticMeshCom->Render_Meshes(pEffect);
	//m_sComponent.m_pStaticMeshCom->Render_MeshesEffect(pEffect, m_sComponent.m_pTextureCom);

	pEffect->EndPass();
	pEffect->End();


	Engine::Safe_Release(pEffect);
	/*_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pColliderCom->Render_Collider(Engine::COL_TRUE, &matWorld);*/
}

HRESULT Client::CEffectRcTex::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix		matWorld, matView, matProj;

	//
	if (m_bill) {
		_vec3 vPos;
		m_sComponent.m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
		CGameObject::Compute_ViewZ(&vPos);

		_matrix		/*matWorld, matView,*/ matBill;

		D3DXMatrixIdentity(&matBill);

		m_sComponent.m_pTransformCom->Get_WorldMatrix(&matWorld);
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

		matBill._11 = matView._11;
		matBill._13 = matView._13;
		matBill._31 = matView._31;
		matBill._33 = matView._33;

		D3DXMatrixInverse(&matBill, NULL, &matBill);

		// 행렬의 곱셈순서를 주의할 것

		m_sComponent.m_pTransformCom->Set_WorldMatrix(&(matBill * matWorld));
	}
	//

	m_sComponent.m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetFloat("g_bIsDissolve", false);
	pEffect->SetFloat("g_uOffset", 1.f / m_uOffset);
	pEffect->SetFloat("g_vOffset", 1.f / m_vOffset);
	pEffect->SetFloat("g_uStep", (int)m_EffectTime % m_uOffset);
	pEffect->SetFloat("g_vStep", (int)m_EffectTime / m_vOffset);
	
	m_sComponent.m_pTextureCom->Set_Texture(pEffect, "g_BaseTexture");

	Engine::Throw_RenderTargetTexture(pEffect, L"Target_Depth", "g_DepthTexture");

	//pEffect->SetFloat("g_uOffset", 0.5f);
	//pEffect->SetFloat("g_vOffset", 0.33f);
	//pEffect->SetFloat("g_uStep", (int)m_EffectTime % 4);
	//pEffect->SetFloat("g_vStep", (int)m_EffectTime / 4);
	
	return S_OK;
}

void Client::CEffectRcTex::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{

}

void CEffectRcTex::Set_Effect(bool _bill, _vec3 _pos, float _scale, const _tchar* _pResourcesTag, int _uOffset, int _vOffset, float _speed, float _rotateX, float _rotateY, float _rotateZ)
{
	m_bill = _bill;
	m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS] = _pos;
	m_sComponent.m_pTransformCom->m_vInfo[Engine::INFO_POS].y += 0.2f;
	m_sComponent.m_pTransformCom->m_vScale = { _scale, _scale, _scale };
	CUnit::Update_Object(0.f);

	Engine::CComponent* pComponent = m_sComponent.m_pTextureCom = dynamic_cast<Engine::CTexture*>(Engine::Clone(Engine::RESOURCE_STAGE, _pResourcesTag));
	NULL_CHECK_RETURN(pComponent);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", pComponent);
	m_uOffset = _uOffset;
	m_vOffset = _vOffset;
	m_EffectSpeed = _speed;
	m_sComponent.m_pTransformCom->Rotation(Engine::ROT_X, D3DXToRadian(_rotateX));
	m_sComponent.m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(_rotateY));
	m_sComponent.m_pTransformCom->Rotation(Engine::ROT_Z, D3DXToRadian(_rotateZ));

	dynamic_cast<CStage*>(Engine::CManagement::GetInstance()->m_pScene)->Get_Layer(Engine::CLayer::LayerName::Layer_StaticNoColl)->Add_GameObject(L"EffectRcTex", this);
}