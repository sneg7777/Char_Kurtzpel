#include "stdafx.h"
#include "Unit_D.h"
#include "Export_Function.h"
#include "SphereCollider.h"
#include "Player.h"

CUnit_D::CUnit_D(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{

}

CUnit_D::~CUnit_D(void)
{
	for (auto& sphere : m_VecSphereCollider)
	{
		Engine::Safe_Release(sphere);
	}
}

HRESULT Client::CUnit_D::Add_Component(void)
{
	Engine::CComponent* pComponent = nullptr;

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

	// Collider 
	//pComponent = m_pColliderCom = Engine::CCollider::Create(m_pGraphicDev, m_pMeshCom->Get_VtxPos(), m_pMeshCom->Get_NumVtx(), m_pMeshCom->Get_Stride());
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Collider", pComponent);

	// Optimization
	//pComponent = m_pOptimizationCom = dynamic_cast<Engine::COptimization*>(Engine::Clone(L"Proto_Optimization"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Optimization", pComponent);

	// Shader
	pComponent = m_pShaderCom = dynamic_cast<Engine::CShader*>(Engine::Clone(L"Proto_Shader_Mesh"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", pComponent);


	return S_OK;
}


_int CUnit_D::Load_ColliderFile(_tchar* pFilePath)
{
	//TCHAR szDataPath[MAX_PATH] = L"../Bin/save.dat";
	_tchar		szDataPath[256] = L"";

	wsprintf(szDataPath, pFilePath);

	HANDLE hFile = CreateFile(szDataPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;
	DWORD dwByte = 0;
	DWORD dwstrByte = 0;

	//
	_vec3 vecPos, vecAng, vecScal;
	TCHAR meshName[MAX_PATH] = L"";
	_int meshNameSize;
	ReadFile(hFile, &meshNameSize, sizeof(_int), &dwByte, nullptr);
	ReadFile(hFile, &meshName, meshNameSize, &dwByte, nullptr);
	ReadFile(hFile, &vecPos, sizeof(_vec3), &dwByte, nullptr);
	ReadFile(hFile, &vecScal, sizeof(_vec3), &dwByte, nullptr);
	ReadFile(hFile, &vecAng, sizeof(_vec3), &dwByte, nullptr);
	//
	_int sphereCnt = 0;
	ReadFile(hFile, &sphereCnt, sizeof(_int), &dwByte, nullptr);

	for (size_t i = 0; i < sphereCnt; i++)
	{
		_vec3 spherePos, sphereScale;
		TCHAR frameName[MAX_PATH] = L"";
		_int frameNameSize;
		ReadFile(hFile, &spherePos, sizeof(_vec3), &dwByte, nullptr);
		ReadFile(hFile, &sphereScale, sizeof(_vec3), &dwByte, nullptr);
		ReadFile(hFile, &frameNameSize, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &frameName, frameNameSize, &dwByte, nullptr);
		///////
		wstring frameNameWString = frameName;
		string frameNameString;
		frameNameString.assign(frameNameWString.begin(), frameNameWString.end());
		
		
		CSphereCollider* sphereCol = CSphereCollider::Create(m_pGraphicDev);
		sphereCol->m_pDynamicMesh = this;
		sphereCol->m_FrameName = frameNameString;
		sphereCol->m_FrameNameCheck = true;
		m_VecSphereCollider.emplace_back(sphereCol);
		sphereCol->m_pTransformCom->m_vInfo[Engine::INFO_POS] = spherePos;
		sphereCol->m_pTransformCom->m_vScale = sphereScale;
	}
	

	
	CloseHandle(hFile);

	return S_OK;
}

CSphereCollider* CUnit_D::Get_BonePartCollider(CSphereCollider::BonePart _bonePart)
{
	for (auto& col : m_VecSphereCollider)
	{
		if (col->m_BonePart == _bonePart)
			return col;
	}
	return nullptr;
}

void CUnit_D::Free(void)
{
	Engine::CGameObject::Free();
}


HRESULT Client::CUnit_D::Ready_Object(void)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(&_vec3(5.f, 0.f, 5.f));
	m_pTransformCom->Rotation(Engine::ROT_Y, D3DXToRadian(45.f));

	return S_OK;
}
Client::_int Client::CUnit_D::Update_Object(const _float& fTimeDelta)
{
	Engine::CGameObject::Update_Object(fTimeDelta);
	for (auto& sphere : m_VecSphereCollider)
	{
		sphere->m_pDynamicMesh = this;
		sphere->Update_Object(fTimeDelta);
	}
	this;
	return 0;
}
void Client::CUnit_D::Render_Object(void)
{
	LPD3DXEFFECT	 pEffect = m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	m_pMeshCom->Render_Meshes(pEffect);

	pEffect->End();

	Engine::Safe_Release(pEffect);
}

void Client::CUnit_D::SetUp_OnTerrain(void)
{
	_vec3	vPosition;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPosition);

	Engine::CTerrainTex* pTerrainBufferCom = dynamic_cast<Engine::CTerrainTex*>(Engine::Get_Component(L"Environment", L"Terrain", L"Com_Buffer", Engine::ID_STATIC));
	NULL_CHECK(pTerrainBufferCom);

	_float fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vPosition, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_pTransformCom->Move_Pos(vPosition.x, fHeight, vPosition.z);
}

HRESULT Client::CUnit_D::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
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

_bool CUnit_D::Collision_ToObject(const _tchar* pLayerTag, const _tchar* pObjTag)
{
	Engine::CCollider* pPlayerColliderCom = dynamic_cast<Engine::CCollider*>(Engine::Get_Component(pLayerTag, pObjTag, L"Com_Collider", Engine::ID_STATIC));
	NULL_CHECK_RETURN(pPlayerColliderCom, false);

	/*return m_pCalculatorCom->Collision_AABB(pPlayerColliderCom->Get_Min(),
											pPlayerColliderCom->Get_Max(),
											pPlayerColliderCom->Get_CollMatrix(),
											m_pColliderCom->Get_Min(),
											m_pColliderCom->Get_Max(),
											m_pColliderCom->Get_CollMatrix());*/

	return m_pCalculatorCom->Collision_OBB(pPlayerColliderCom->Get_Min(),
		pPlayerColliderCom->Get_Max(),
		pPlayerColliderCom->Get_CollMatrix(),
		m_pColliderCom->Get_Min(),
		m_pColliderCom->Get_Max(),
		m_pColliderCom->Get_CollMatrix());
}

void CUnit_D::Collision(CSphereCollider* _mySphere, Engine::CGameObject* _col, CSphereCollider* _colSphere)
{

}