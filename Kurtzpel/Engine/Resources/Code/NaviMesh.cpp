#include "NaviMesh.h"
USING(Engine)

Engine::CNaviMesh::CNaviMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMesh(pGraphicDev)
	, m_dwIndex(0)
{
	
}

Engine::CNaviMesh::CNaviMesh(const CNaviMesh& rhs)
	: CMesh(rhs)
	, m_vecCell(rhs.m_vecCell)
	, m_dwIndex(rhs.m_dwIndex)
{
	for (auto& iter : m_vecCell)
		Safe_AddRef(iter);
}

Engine::CNaviMesh::~CNaviMesh(void)
{

}

HRESULT Engine::CNaviMesh::Ready_NaviMeshes(int number)
{
	HANDLE hFile;
	if (number == 2) {
		TCHAR szDataPath[MAX_PATH] = L"../Bin/saveStage2Navi.dat";
		hFile = CreateFile(szDataPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	}
	else {
		TCHAR szDataPath[MAX_PATH] = L"../Bin/saveNavi.dat";
		hFile = CreateFile(szDataPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	}

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;
	DWORD dwByte = 0;
	DWORD dwstrByte = 0;

	bool endCheck = false;
	while (true)
	{
		bool sphereOverlap = false;
		_vec3 vecPos[3];
		for (int i = 0; i < 3; i++)
		{
			ReadFile(hFile, &vecPos[i], sizeof(_vec3), &dwByte, nullptr); //세모 꼭짓점 3개 벡터 가져와주고

			if (0 == dwByte)
			{
				endCheck = true;
				break;
			}
		}
		if (endCheck)
			break;
		Engine::CCell* pCell = nullptr;
		pCell = Engine::CCell::Create(m_pGraphicDev, m_vecCell.size(), &vecPos[0], &vecPos[1], &vecPos[2]);
		NULL_CHECK_RETURN(pCell);
		m_vecCell.push_back(pCell);

	}
	CloseHandle(hFile);

	FAILED_CHECK_RETURN(Link_Cell(), E_FAIL);

	return S_OK;
}

void CNaviMesh::Render_NaviMeshes(void)
{
	for (auto& iter : m_vecCell)
		iter->Render_Cell();
}

_vec3 CNaviMesh::Move_OnNaviMesh(const _vec3* pTargetPos, const _vec3* pTargetDir, _ulong* _index, bool* _check)
{
	_vec3		vEndPos = *pTargetPos + *pTargetDir;


	_vec3		vTargetDir = { pTargetDir->x, 0.f, pTargetDir->z };

	//CCell::MOVING movecheck = m_vecCell[m_dwIndex]->CompareCell2(&vEndPos, &m_dwIndex, &vTargetDir);
	//if (CCell::MOVE == movecheck)
	//	return vEndPos;
	//else if (CCell::STOP == movecheck)
	//	return *pTargetPos + vTargetDir;

	CCell::MOVING movecheck = m_vecCell[*_index]->CompareCell(&vEndPos, _index);
	if (CCell::MOVE == m_vecCell[*_index]->CompareCell(&vEndPos, _index)) {
		if (_check != nullptr)
			*_check = false;
		return vEndPos;
	}
	else if (CCell::STOP == m_vecCell[*_index]->CompareCell(&vEndPos, _index)) {
		if (_check != nullptr)
			*_check = true;
		return *pTargetPos;
	}
}

HRESULT Engine::CNaviMesh::Link_Cell(void)
{
	for (_ulong i = 0; i < m_vecCell.size(); ++i)
	{
		for (_ulong j = 0; j < m_vecCell.size(); ++j)
		{
			if(i == j)
				continue;

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_AB) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_A),
													m_vecCell[i]->Get_Point(CCell::POINT_B),	
													m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_AB, m_vecCell[j]);
				continue;
			}

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_BC) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_B),
													m_vecCell[i]->Get_Point(CCell::POINT_C),
													m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_BC, m_vecCell[j]);
				continue;
			}

			if (nullptr == m_vecCell[i]->Get_Neighbor(CCell::NEIGHBOR_CA) &&
				true == m_vecCell[j]->Compare_Point(m_vecCell[i]->Get_Point(CCell::POINT_C),
													m_vecCell[i]->Get_Point(CCell::POINT_A),
													m_vecCell[i]))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_CA, m_vecCell[j]);
				continue;
			}

		}
	}
	
	return S_OK;
}

Engine::CNaviMesh* Engine::CNaviMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, int number)
{
	CNaviMesh*		pInstance = new CNaviMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_NaviMeshes(number)))
		Safe_Release(pInstance);

	return pInstance;
}

Engine::CComponent* Engine::CNaviMesh::Clone(void)
{
	return new CNaviMesh(*this);
}

void Engine::CNaviMesh::Free(void)
{

	for_each(m_vecCell.begin(), m_vecCell.end(), CDeleteObj());
	m_vecCell.clear();


	CMesh::Free();
}

