#include "Calculator.h"
#include "TerrainTex.h"
#include "Transform.h"

USING(Engine)

Engine::CCalculator::CCalculator(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	Safe_AddRef(m_pGraphicDev);
}

Engine::CCalculator::CCalculator(const CCalculator& rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
{
	Safe_AddRef(m_pGraphicDev);
}

Engine::CCalculator::~CCalculator(void)
{

}

HRESULT Engine::CCalculator::Ready_Calculator(void)
{
	return S_OK;
}

Engine::_float Engine::CCalculator::Compute_HeightOnTerrain(const _vec3* pPos,
															const _vec3* pTerrainVtxPos,
															const _ulong& dwCntX, 
															const _ulong& dwCntZ,
															const _ulong& dwVtxItv)
{
	_ulong	dwIndex = _ulong(pPos->z / dwVtxItv) * dwCntX + _ulong(pPos->x / dwVtxItv);

	_float	fRatioX = (pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x) / dwVtxItv;
	_float	fRatioZ = (pTerrainVtxPos[dwIndex + dwCntX].z - pPos->z) / dwVtxItv;

	//D3DXPLANE	Plane;

	//// 오른쪽 위 삼각형
	//if (fRatioX > fRatioZ)
	//{
	//	D3DXPlaneFromPoints(&Plane, 
	//		&pTerrainVtxPos[dwIndex + dwCntX],
	//		&pTerrainVtxPos[dwIndex + dwCntX + 1],
	//		&pTerrainVtxPos[dwIndex + 1]);
	//}

	//else
	//{
	//	D3DXPlaneFromPoints(&Plane,
	//		&pTerrainVtxPos[dwIndex + dwCntX],
	//		&pTerrainVtxPos[dwIndex + 1],
	//		&pTerrainVtxPos[dwIndex]);
	//}
	///*ax + by + cz + d 
	//by = -ax - cz - d
	//y = (-ax - cz - d) / b*/

	//_float fY = (-Plane.a * pPos->x - Plane.c * pPos->z - Plane.d) / Plane.b;
	//
	//return fY;


	_float	fHeight[4] = {
		pTerrainVtxPos[dwIndex + dwCntX].y,
		pTerrainVtxPos[dwIndex + dwCntX + 1].y,
		pTerrainVtxPos[dwIndex + 1].y,
		pTerrainVtxPos[dwIndex ].y
	};

	if (fRatioX > fRatioZ)
	{
		return fHeight[0] + (fHeight[1] - fHeight[0]) * fRatioX + (fHeight[2] - fHeight[1]) * fRatioZ;
	}
	else
	{
		return fHeight[0] + (fHeight[2] - fHeight[3]) * fRatioX + (fHeight[3] - fHeight[0]) * fRatioZ;
	}

}



Engine::CCalculator* Engine::CCalculator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCalculator*	pInstance = new CCalculator(pGraphicDev);

	if (FAILED(pInstance->Ready_Calculator()))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CCalculator::Free(void)
{
	Safe_Release(m_pGraphicDev);
}

Engine::CComponent* Engine::CCalculator::Clone(void)
{
	return new CCalculator(*this);
}

Engine::_vec3 Engine::CCalculator::Picking_OnTerrain(HWND hWnd,
	 CTerrainTex* pTerrainBufferCom, 
	 CTransform* pTerrainTransformCom)
{
	POINT		ptMouse{ 0 };

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vec3	vMousePos;

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);

	// 뷰포트 -> 투영

	vMousePos.x = (ptMouse.x / (ViewPort.Width * 0.5f)) - 1.f;
	vMousePos.y = (ptMouse.y / -(ViewPort.Height * 0.5f)) + 1.f;
	vMousePos.z = 0.f;

	// L * W * V * P * (P^-1)
	// L * W * V

	// 투영 -> 뷰 스페이스
	_matrix	matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	// 뷰 스페이스 -> 월드
	_matrix	matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	_vec3	vRayPos, vRayDir;


	vRayPos = _vec3(0.f, 0.f, 0.f);
	vRayDir = vMousePos - vRayPos;

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	// 월드 -> 로컬
	_matrix	matWorld;
	pTerrainTransformCom->Get_WorldMatrix(&matWorld);
	D3DXMatrixInverse(&matWorld, NULL, &matWorld);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	_ulong	dwVtxCntX = pTerrainBufferCom->Get_VtxCntX();
	_ulong	dwVtxCntZ = pTerrainBufferCom->Get_VtxCntZ();

	const _vec3*	pTerrainVtxPos = pTerrainBufferCom->Get_VtxPos();

	_ulong	dwVtxIdx[3];
	_float	fU, fV, fDist;

	for (_ulong i = 0; i < dwVtxCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwVtxCntX - 1; ++j)
		{
			_ulong dwIndex = i * dwVtxCntX + j;

			// 오른쪽 위
			dwVtxIdx[0] = dwIndex + dwVtxCntX;
			dwVtxIdx[1] = dwIndex + dwVtxCntX + 1;
			dwVtxIdx[2] = dwIndex + 1;

			// V1 + U(V2 - V1) + V(V3 - V1)
			if (D3DXIntersectTri(&pTerrainVtxPos[dwVtxIdx[1]],
				&pTerrainVtxPos[dwVtxIdx[2]],
				&pTerrainVtxPos[dwVtxIdx[0]], &vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				return _vec3(pTerrainVtxPos[dwVtxIdx[1]].x + fU * (pTerrainVtxPos[dwVtxIdx[2]].x - pTerrainVtxPos[dwVtxIdx[1]].x),
							 0.f, 
							 pTerrainVtxPos[dwVtxIdx[1]].z + fU * (pTerrainVtxPos[dwVtxIdx[0]].z - pTerrainVtxPos[dwVtxIdx[1]].z));
			}
			
			// 왼쪽 아래
			dwVtxIdx[0] = dwIndex + dwVtxCntX;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			// V1 + U(V2 - V1) + V(V3 - V1)
			if (D3DXIntersectTri(&pTerrainVtxPos[dwVtxIdx[2]],
				&pTerrainVtxPos[dwVtxIdx[0]],
				&pTerrainVtxPos[dwVtxIdx[1]], &vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				return _vec3(pTerrainVtxPos[dwVtxIdx[2]].x + fU * (pTerrainVtxPos[dwVtxIdx[1]].x - pTerrainVtxPos[dwVtxIdx[2]].x),
						0.f,
						pTerrainVtxPos[dwVtxIdx[2]].z + fU * (pTerrainVtxPos[dwVtxIdx[0]].z - pTerrainVtxPos[dwVtxIdx[2]].z));
			}

		}
	}	

	return _vec3(0.f, 0.f, 0.f);
}

_bool CCalculator::Collision_AABB(const _vec3 * pDestMin, const _vec3 * pDestMax, 
								const _matrix * pDestWorld, const _vec3 * pSourMin, 
								const _vec3 * pSourMax, const _matrix * pSourWorld)
{
	_vec3	vDestMin, vDestMax, vSourMin, vSourMax;
	_float	fMin, fMax;

	D3DXVec3TransformCoord(&vDestMin, pDestMin, pDestWorld);
	D3DXVec3TransformCoord(&vDestMax, pDestMax, pDestWorld);

	D3DXVec3TransformCoord(&vSourMin, pSourMin, pSourWorld);
	D3DXVec3TransformCoord(&vSourMax, pSourMax, pSourWorld);

	// x축
	fMin = max(vDestMin.x, vSourMin.x);
	fMax = min(vDestMax.x, vSourMax.x);

	if (fMax < fMin)
		return false;

	// y축
	fMin = max(vDestMin.y, vSourMin.y);
	fMax = min(vDestMax.y, vSourMax.y);

	if (fMax < fMin)
		return false;

	// z축
	fMin = max(vDestMin.z, vSourMin.z);
	fMax = min(vDestMax.z, vSourMax.z);

	if (fMax < fMin)
		return false;


	return true;
}

_bool CCalculator::Collision_OBB(const _vec3 * pDestMin, const _vec3 * pDestMax, const _matrix * pDestWorld, const _vec3 * pSourMin, const _vec3 * pSourMax, const _matrix * pSourWorld)
{
	OBB		tObb[2];
	ZeroMemory(tObb, sizeof(OBB) * 2);

	Set_Point(&tObb[0], pDestMin, pDestMax);
	Set_Point(&tObb[1], pSourMin, pSourMax);

	for (_uint i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&tObb[0].vPoint[i], &tObb[0].vPoint[i], pDestWorld);
		D3DXVec3TransformCoord(&tObb[1].vPoint[i], &tObb[1].vPoint[i], pSourWorld);
	}

	D3DXVec3TransformCoord(&tObb[0].vCenter, &tObb[0].vCenter, pDestWorld);
	D3DXVec3TransformCoord(&tObb[1].vCenter, &tObb[1].vCenter, pSourWorld);

	for (_uint i = 0; i < 2; ++i)
		Set_Axis(&tObb[i]);

	_float		fDistance[3];		// 1. 첫 번째 obb에서 임의의 축으로 투영한 길이의 합
									// 2. 두 번째 obb에서 임의의 축으로 투영한 길이의 합
									// 3. 두 obb의 중점을 이은 벡터를 각 축에 투영한 길이의 합

	for (_uint i = 0; i < 2; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			fDistance[0] = fabs(D3DXVec3Dot(&tObb[0].vProjAxis[0], &tObb[i].vAxis[j])) +
						   fabs(D3DXVec3Dot(&tObb[0].vProjAxis[1], &tObb[i].vAxis[j])) +
						   fabs(D3DXVec3Dot(&tObb[0].vProjAxis[2], &tObb[i].vAxis[j]));


			fDistance[1] = fabs(D3DXVec3Dot(&tObb[1].vProjAxis[0], &tObb[i].vAxis[j])) +
						   fabs(D3DXVec3Dot(&tObb[1].vProjAxis[1], &tObb[i].vAxis[j])) +
						   fabs(D3DXVec3Dot(&tObb[1].vProjAxis[2], &tObb[i].vAxis[j]));

			fDistance[2] = fabs(D3DXVec3Dot(&(tObb[1].vCenter - tObb[0].vCenter), &tObb[i].vAxis[j]));

			if (fDistance[0] + fDistance[1] < fDistance[2])
				return false;
		}
	}

	return true;
}

void CCalculator::Set_Point(OBB * pObb, const _vec3 * pMin, const _vec3 * pMax)
{
	pObb->vPoint[0] = _vec3(pMin->x, pMax->y, pMin->z);
	pObb->vPoint[1] = _vec3(pMax->x, pMax->y, pMin->z);
	pObb->vPoint[2] = _vec3(pMax->x, pMin->y, pMin->z);
	pObb->vPoint[3] = _vec3(pMin->x, pMin->y, pMin->z);

	pObb->vPoint[4] = _vec3(pMin->x, pMax->y, pMax->z);
	pObb->vPoint[5] = _vec3(pMax->x, pMax->y, pMax->z);
	pObb->vPoint[6] = _vec3(pMax->x, pMin->y, pMax->z);
	pObb->vPoint[7] = _vec3(pMin->x, pMin->y, pMax->z);

	pObb->vCenter = (*pMin + *pMax) * 0.5f;
}

void CCalculator::Set_Axis(OBB * pObb)
{
	pObb->vProjAxis[0] = (pObb->vPoint[2] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;
	pObb->vProjAxis[1] = (pObb->vPoint[0] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;
	pObb->vProjAxis[2] = (pObb->vPoint[7] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;

	pObb->vAxis[0] = pObb->vPoint[2] - pObb->vPoint[3];
	pObb->vAxis[1] = pObb->vPoint[0] - pObb->vPoint[3];
	pObb->vAxis[2] = pObb->vPoint[7] - pObb->vPoint[3];

	for (_uint i = 0; i < 3; ++i)
		D3DXVec3Normalize(&pObb->vAxis[i], &pObb->vAxis[i]);
}

