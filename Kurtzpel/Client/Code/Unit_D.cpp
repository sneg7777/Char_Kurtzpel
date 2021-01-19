#include "stdafx.h"
#include "Unit_D.h"
#include "Export_Function.h"
#include "SphereCollider.h"
#include "Player.h"

CUnit_D::CUnit_D(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUnit(pGraphicDev)
{

}

CUnit_D::~CUnit_D(void)
{

}

HRESULT Client::CUnit_D::Add_Component(void)
{
	CUnit::Add_Component();


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
		sphereCol->m_pDynamicThis = this;
		sphereCol->m_FrameName = frameNameString;
		sphereCol->m_FrameNameCheck = true;
		m_VecSphereCollider.emplace_back(sphereCol);
		sphereCol->m_pTransformCom->m_vInfo[Engine::INFO_POS] = spherePos;
		sphereCol->m_pTransformCom->m_vScale = sphereScale;
	}
	

	
	CloseHandle(hFile);

	return S_OK;
}

void CUnit_D::Free(void)
{
	CUnit::Free();
}


HRESULT Client::CUnit_D::Ready_Object(void)
{
	CUnit::Ready_Object();

	return S_OK;
}
Client::_int Client::CUnit_D::Update_Object(const _float& fTimeDelta)
{
	CUnit::Update_Object(fTimeDelta);
	return 0;
}
void Client::CUnit_D::Render_Object(void)
{
	LPD3DXEFFECT	 pEffect = m_sComponent.m_pShaderCom->Get_EffectHandle();
	NULL_CHECK(pEffect);
	Engine::Safe_AddRef(pEffect);

	_uint	iMaxPass = 0;

	pEffect->Begin(&iMaxPass, 0);	// 현재 쉐이더 파일이 갖고 있는 최대 패스의 개수를 리턴, 사용하는 방식

	FAILED_CHECK_RETURN(SetUp_ConstantTable(pEffect), );

	m_sComponent.m_pMeshCom->Render_Meshes(pEffect);

	pEffect->End();

	Engine::Safe_Release(pEffect);
}

HRESULT Client::CUnit_D::SetUp_ConstantTable(LPD3DXEFFECT& pEffect)
{
	_matrix		matWorld, matView, matProj;

	m_sComponent.m_pTransformCom->Get_WorldMatrix(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	//////////////////////////////////////
	//const D3DLIGHT9* pLightInfo = Engine::Get_Light(0);

	//pEffect->SetVector("g_vLightDir", &_vec4(pLightInfo->Direction, 0.f));
	//D3DMATERIAL9			tMtrlInfo;
	//ZeroMemory(&tMtrlInfo, sizeof(D3DMATERIAL9));

	//tMtrlInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tMtrlInfo.Power = 20.f;

	//pEffect->SetVector("Color", (_vec4*)&tMtrlInfo.Diffuse);

	return S_OK;
}

void CUnit_D::Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta)
{

}

void CUnit_D::Init_BoneAttack(bool _attack) {
	for (auto& bone : m_VecSphereCollider)
	{
		bone->m_WeaponAttack = _attack;
	}
}