#include "stdafx.h"
#include "SphereCollider.h"
#include "Export_Function.h"
#include "ApostleOfGreed.h"
#include "Unit_D.h"


CSphereCollider::CSphereCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CSphereCollider::~CSphereCollider(void)
{

}

HRESULT CSphereCollider::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

	// buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CCollSphere*>(Engine::Clone(Engine::RESOURCE_STATIC, L"Buffer_CollSphere"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", pComponent);

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(Engine::Clone(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_Transform", pComponent);

	// Renderer
	pComponent = m_pRendererCom = Engine::Get_Renderer();
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	Safe_AddRef(pComponent);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Renderer", pComponent);

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>(Engine::Clone(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Calculator", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Proto_Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);

	//m_pBufferCom->Set_Color(D3DCOLOR_ARGB(255, 0, 200, 0));

	
	return S_OK;
}

CSphereCollider* CSphereCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSphereCollider* pInstance = new CSphereCollider(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CSphereCollider::Free(void)
{
	Engine::CGameObject::Free();
}

HRESULT CSphereCollider::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CSphereCollider::Update_Object(const _float& fTimeDelta)
{
	if (m_Dead)
		return 1;
	
	m_pTransformCom->m_vScale;
	m_pTransformCom->m_vInfo[Engine::INFO_POS];
	if (nullptr == m_pParentBoneMatrix)
	{
		if (m_pDynamicMesh == nullptr)
			return 0;
		if (!m_FrameNameCheck)
			return 0;
		
		//다이나믹을 안쓰고 C스타일의 강제 형변환을 써서 m_pMeshCom 를 참조해도 된다함.
		//if(nullptr == dynamic_cast<CMonster*>(m_pDynamicMesh));
		//	return 0;
		
		const Engine::D3DXFRAME_DERIVED* pFrame = ((Client::CUnit_D*)m_pDynamicMesh)->m_pMeshCom->Get_FrameByName(m_FrameName.c_str());

		m_pParentBoneMatrix = &pFrame->CombinedTransformationMatrix;

		Engine::CTransform* pPlayerTransCom = ((Client::CUnit_D*)m_pDynamicMesh)->m_pTransformCom;
		
		NULL_CHECK_RETURN(pPlayerTransCom, 0);
		m_pParentWorldMatrix = pPlayerTransCom->Get_WorldMatrix();
	}

	Engine::CGameObject::Update_Object(fTimeDelta);

	m_pTransformCom->Set_ParentMatrix(&(*m_pParentBoneMatrix * *m_pParentWorldMatrix));


	//m_bColl = Collision_ToObject(L"GameLogic", L"Player");

	m_pRendererCom->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

	return 0;
}
void CSphereCollider::Render_Object(void)
{
	m_pTransformCom->Set_Transform(m_pGraphicDev);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pBufferCom->Render_Buffer();
	//m_pTextureCom->Render_Texture(0);
	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
	//NULL_CHECK(pEffect);
	//Engine::Safe_AddRef(pEffect);

	//_uint	iMaxPass = 0;

	//pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식
	//pEffect->BeginPass(0);

	//FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	//m_pBufferCom->Render_Buffer(pEffect);

	//pEffect->EndPass();
	//pEffect->End();


	//Engine::Safe_Release(pEffect);
}
