#include "QuadTree.h"

#include "Frustum.h"
USING(Engine)

Engine::CQuadTree::CQuadTree(void)
{
	ZeroMemory(m_pChild, sizeof(CQuadTree*) * CHILD_END);
	ZeroMemory(m_pNeighbor, sizeof(CQuadTree*) * NEIGHBOR_END);
}

CQuadTree::CQuadTree(const CQuadTree & rhs)
{
	for (_ulong i = 0; i < 4; ++i)
	{
		m_pChild[i] = rhs.m_pChild[i];
		m_pNeighbor[i] = rhs.m_pNeighbor[i];
	}
}

Engine::CQuadTree::~CQuadTree(void)
{

}

HRESULT Engine::CQuadTree::Ready_QuadTree(const _ulong& dwCntX, const _ulong& dwCntZ)
{
	m_dwCorner[CORNER_LT] = dwCntX * dwCntZ - dwCntX;
	m_dwCorner[CORNER_RT] = dwCntX * dwCntZ - 1;
	m_dwCorner[CORNER_LB] = 0;
	m_dwCorner[CORNER_RB] = dwCntX - 1;

	m_dwCenter = (m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_RT] + m_dwCorner[CORNER_LB] + m_dwCorner[CORNER_RB]) >> 2;

	Make_ChildQuadTree();

	return S_OK;
}

void Engine::CQuadTree::Culling_ForTerrain(CFrustum* pFrustum,
											const _vec3* pVtxPos,
											INDEX32* pIndex, 
											_ulong* pTriCnt)
{
	m_fRadius = D3DXVec3Length(&(pVtxPos[m_dwCorner[CORNER_LT]] - pVtxPos[m_dwCenter]));

	_bool bIsIn = pFrustum->Is_InFrustum(&pVtxPos[m_dwCenter], m_fRadius);

	if (true == bIsIn)
	{
		// 최하위 쿼드트리까 쪼개졌다면
		//if (nullptr == m_pChild[0])
		if(true == LevelOfDetail(pFrustum->Get_GraphicDev(), pVtxPos))
		{
			_bool		bNeighborDraw[4] = { true, true, true, true };

			for (_ulong i = 0; i < NEIGHBOR_END; ++i)
			{
				if (nullptr != m_pNeighbor[i])
					bNeighborDraw[i] = m_pNeighbor[i]->LevelOfDetail(pFrustum->Get_GraphicDev(), pVtxPos);
			}



			_bool	bIsVertexInFrustum[3] = { false };

			// 오른쪽 위 삼각형
			bIsVertexInFrustum[0] = pFrustum->Is_InFrustum(&pVtxPos[m_dwCorner[CORNER_LT]]);
			bIsVertexInFrustum[1] = pFrustum->Is_InFrustum(&pVtxPos[m_dwCorner[CORNER_RT]]);
			bIsVertexInFrustum[2] = pFrustum->Is_InFrustum(&pVtxPos[m_dwCorner[CORNER_RB]]);

			if (true == bIsVertexInFrustum[0] ||
				true == bIsVertexInFrustum[1] ||
				true == bIsVertexInFrustum[2])
			{
				if (bNeighborDraw[NEIGHBOR_TOP] && bNeighborDraw[NEIGHBOR_RIGHT])
				{
					// 더이상 쪼갤 수 없는 단위일 때는
					if (nullptr == m_pChild[0])
					{
						pIndex[*pTriCnt]._0 = m_dwCorner[CORNER_LT];
						pIndex[*pTriCnt]._1 = m_dwCorner[CORNER_RT];
						pIndex[*pTriCnt]._2 = m_dwCorner[CORNER_RB];
						(*pTriCnt)++;
					}
					else
					{
						pIndex[*pTriCnt]._0 = m_dwCorner[CORNER_LT];
						pIndex[*pTriCnt]._1 = m_dwCorner[CORNER_RT];
						pIndex[*pTriCnt]._2 = m_dwCenter;
						(*pTriCnt)++;

						pIndex[*pTriCnt]._0 = m_dwCorner[CORNER_RT];
						pIndex[*pTriCnt]._1 = m_dwCorner[CORNER_RB];
						pIndex[*pTriCnt]._2 = m_dwCenter;
						(*pTriCnt)++;
					}

				}
				else
				{
					if (false == bNeighborDraw[NEIGHBOR_TOP])
					{
						pIndex[*pTriCnt]._0 = m_dwCorner[CORNER_LT];
						pIndex[*pTriCnt]._1 = (m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_RT]) >> 1;
						pIndex[*pTriCnt]._2 = m_dwCenter;
						(*pTriCnt)++;

						pIndex[*pTriCnt]._0 = (m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_RT]) >> 1;
						pIndex[*pTriCnt]._1 = m_dwCorner[CORNER_RT];
						pIndex[*pTriCnt]._2 = m_dwCenter;
						(*pTriCnt)++;
					}
					else
					{
						pIndex[*pTriCnt]._0 = m_dwCorner[CORNER_LT];
						pIndex[*pTriCnt]._1 = m_dwCorner[CORNER_RT];
						pIndex[*pTriCnt]._2 = m_dwCenter;
						(*pTriCnt)++;
					}

					if (false == bNeighborDraw[NEIGHBOR_RIGHT])
					{
						pIndex[*pTriCnt]._0 = m_dwCorner[CORNER_RT];
						pIndex[*pTriCnt]._1 = (m_dwCorner[CORNER_RT] + m_dwCorner[CORNER_RB]) >> 1;
						pIndex[*pTriCnt]._2 = m_dwCenter;
						(*pTriCnt)++;

						pIndex[*pTriCnt]._0 = (m_dwCorner[CORNER_RT] + m_dwCorner[CORNER_RB]) >> 1;
						pIndex[*pTriCnt]._1 = m_dwCorner[CORNER_RB];
						pIndex[*pTriCnt]._2 = m_dwCenter;
						(*pTriCnt)++;
					}
					else
					{
						pIndex[*pTriCnt]._0 = m_dwCorner[CORNER_RT];
						pIndex[*pTriCnt]._1 = m_dwCorner[CORNER_RB];
						pIndex[*pTriCnt]._2 = m_dwCenter;
						(*pTriCnt)++;
					}
				}			
			}

			// 왼쪽 아래 삼각형
			bIsVertexInFrustum[0] = pFrustum->Is_InFrustum(&pVtxPos[m_dwCorner[CORNER_LT]]);
			bIsVertexInFrustum[1] = pFrustum->Is_InFrustum(&pVtxPos[m_dwCorner[CORNER_RB]]);
			bIsVertexInFrustum[2] = pFrustum->Is_InFrustum(&pVtxPos[m_dwCorner[CORNER_LB]]);

			if (true == bIsVertexInFrustum[0] ||
				true == bIsVertexInFrustum[1] ||
				true == bIsVertexInFrustum[2])
			{
				if (bNeighborDraw[NEIGHBOR_BOTTOM] && bNeighborDraw[NEIGHBOR_LEFT])
				{
					if (nullptr == m_pChild[0])
					{
						pIndex[*pTriCnt]._0 = m_dwCorner[CORNER_LT];
						pIndex[*pTriCnt]._1 = m_dwCorner[CORNER_RB];
						pIndex[*pTriCnt]._2 = m_dwCorner[CORNER_LB];
						(*pTriCnt)++;
					}

					else
					{
						pIndex[*pTriCnt]._0 = m_dwCorner[CORNER_LT];
						pIndex[*pTriCnt]._1 = m_dwCenter;
						pIndex[*pTriCnt]._2 = m_dwCorner[CORNER_LB];
						(*pTriCnt)++;

						pIndex[*pTriCnt]._0 = m_dwCenter;
						pIndex[*pTriCnt]._1 = m_dwCorner[CORNER_RB];
						pIndex[*pTriCnt]._2 = m_dwCorner[CORNER_LB];
						(*pTriCnt)++;
					}
				
				}

				else
				{
					if (false == bNeighborDraw[NEIGHBOR_BOTTOM])
					{
						pIndex[*pTriCnt]._0 = m_dwCorner[CORNER_LB];
						pIndex[*pTriCnt]._1 = m_dwCenter;
						pIndex[*pTriCnt]._2 = (m_dwCorner[CORNER_LB] + m_dwCorner[CORNER_RB]) >> 1;
						(*pTriCnt)++;

						pIndex[*pTriCnt]._0 = m_dwCenter;
						pIndex[*pTriCnt]._1 = m_dwCorner[CORNER_RB];
						pIndex[*pTriCnt]._2 = (m_dwCorner[CORNER_LB] + m_dwCorner[CORNER_RB]) >> 1;
						(*pTriCnt)++;
					}
					else
					{
						pIndex[*pTriCnt]._0 = m_dwCorner[CORNER_LB];
						pIndex[*pTriCnt]._1 = m_dwCenter;
						pIndex[*pTriCnt]._2 = m_dwCorner[CORNER_RB];
						(*pTriCnt)++;
					}

					if (false == bNeighborDraw[NEIGHBOR_LEFT])
					{
						pIndex[*pTriCnt]._0 = m_dwCorner[CORNER_LT];
						pIndex[*pTriCnt]._1 = m_dwCenter;
						pIndex[*pTriCnt]._2 = (m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_LB]) >> 1;
						(*pTriCnt)++;

						pIndex[*pTriCnt]._0 = m_dwCenter;
						pIndex[*pTriCnt]._1 = m_dwCorner[CORNER_LB];
						pIndex[*pTriCnt]._2 = (m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_LB]) >> 1;
						(*pTriCnt)++;
					}
					else
					{
						pIndex[*pTriCnt]._0 = m_dwCorner[CORNER_LT];
						pIndex[*pTriCnt]._1 = m_dwCenter;
						pIndex[*pTriCnt]._2 = m_dwCorner[CORNER_LB];
						(*pTriCnt)++;
					}
				}
			}
			return;
		}

		for (_ulong i = 0; i < CHILD_END; ++i)
		{
			if (nullptr != m_pChild[i])
				m_pChild[i]->Culling_ForTerrain(pFrustum, pVtxPos, pIndex, pTriCnt);
		}
	}
}

HRESULT Engine::CQuadTree::Ready_Neighbor(void)
{
	Make_NeighborQuadTree();

	return S_OK;
}

void Engine::CQuadTree::Make_NeighborQuadTree(void)
{
	if (nullptr == m_pChild[0])
		return;


	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RT];
	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_LB];

	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LT];
	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_RB];

	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RB];
	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_LT];
	
	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LB];
	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_RT];

	if (nullptr != m_pNeighbor[NEIGHBOR_LEFT])
	{
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RT];
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_RIGHT])
	{
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_TOP])
	{
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_LB];
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_RB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_BOTTOM])
	{
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_RT];
	}
	
	for (_ulong i = 0; i < NEIGHBOR_END; ++i)
		m_pChild[i]->Make_NeighborQuadTree();
	
}


_bool CQuadTree::LevelOfDetail(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 * pVtxPos)
{
	_matrix		matCamWorld;

	pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);
	D3DXMatrixInverse(&matCamWorld, NULL, &matCamWorld);

	_vec3	vCamPos;
	memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));

	_float	fDistance = D3DXVec3Length(&(vCamPos - pVtxPos[m_dwCenter]));

	_float	fWidth = D3DXVec3Length(&(pVtxPos[m_dwCorner[CORNER_RT]] - pVtxPos[m_dwCorner[CORNER_LT]]));

	return fDistance * 0.1f > fWidth;
}


void Engine::CQuadTree::Make_ChildCorner(CHILD eType, const _ulong& dwPLT, 
	const _ulong& dwPRT, const _ulong& dwPLB,
	const _ulong& dwPRB, const _ulong& dwPCenter,
	_ulong* pLT, _ulong* pRT, _ulong* pLB, _ulong* pRB)
{
	_ulong	dwLT, dwRT, dwLB, dwRB;
	_ulong	dwLC, dwRC, dwTC, dwBC;

	dwLC = (dwPLT + dwPLB) >> 1;
	dwRC = (dwPRT + dwPRB) >> 1;
	dwTC = (dwPLT + dwPRT) >> 1;
	dwBC = (dwPLB + dwPRB) >> 1;

	switch (eType)
	{
	case CHILD_LT:
		dwLT = dwPLT;
		dwRT = dwTC;
		dwLB = dwLC;
		dwRB = dwPCenter;
		break;

	case CHILD_RT:
		dwLT = dwTC;
		dwRT = dwPRT;
		dwLB = dwPCenter;
		dwRB = dwRC;
		break;

	case CHILD_LB:
		dwLT = dwLC;
		dwRT = dwPCenter;
		dwLB = dwPLB;
		dwRB = dwBC;
		break;

	case CHILD_RB:
		dwLT = dwPCenter;
		dwRT = dwRC;
		dwLB = dwBC;
		dwRB = dwPRB;
		break;

	}
		
	*pLT = dwLT;
	*pRT = dwRT;
	*pLB = dwLB;
	*pRB = dwRB;
}

void Engine::CQuadTree::Make_ChildQuadTree(void)
{
	if (1 == m_dwCorner[CORNER_RT] - m_dwCorner[CORNER_LT])
		return;

	for (_ulong i = 0; i < CHILD_END; ++i)
	{
		_ulong dwLT, dwRT, dwLB, dwRB;

		Make_ChildCorner(CHILD(i),
			m_dwCorner[CORNER_LT],  
			m_dwCorner[CORNER_RT], 
			m_dwCorner[CORNER_LB], 
			m_dwCorner[CORNER_RB],
			m_dwCenter, 
			&dwLT, &dwRT, &dwLB, &dwRB);

		m_pChild[i] = Create(dwLT, dwRT, dwLB, dwRB);

		if (nullptr != m_pChild[i])
			m_pChild[i]->Make_ChildQuadTree();
	}

}

void Engine::CQuadTree::Set_Corner(const _ulong& dwLT, const _ulong& dwRT, 
	const _ulong& dwLB, const _ulong& dwRB)
{

	m_dwCorner[CORNER_LT] = dwLT;
	m_dwCorner[CORNER_RT] = dwRT;
	m_dwCorner[CORNER_LB] = dwLB;
	m_dwCorner[CORNER_RB] = dwRB;

	m_dwCenter = (m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_RT] + m_dwCorner[CORNER_LB] + m_dwCorner[CORNER_RB]) >> 2;
}

Engine::CQuadTree* Engine::CQuadTree::Create(const _ulong& dwCntX, const _ulong& dwCntZ)
{
	CQuadTree*	pInstance = new CQuadTree;

	if (FAILED(pInstance->Ready_QuadTree(dwCntX, dwCntZ)))
		Safe_Release(pInstance);

	if (FAILED(pInstance->Ready_Neighbor()))
		Safe_Release(pInstance);

	return pInstance;
}

Engine::CQuadTree* Engine::CQuadTree::Create(const _ulong& dwLT, const _ulong& dwRT, const _ulong& dwLB, const _ulong& dwRB)
{
	CQuadTree* pInstance = new CQuadTree;

	pInstance->Set_Corner(dwLT, dwRT, dwLB, dwRB);

	return pInstance;
}

void Engine::CQuadTree::Free(void)
{
	for (_ulong i = 0; i < CHILD_END; ++i)
		Safe_Release(m_pChild[i]);
}

