#include "CollSphere.h"

bool Engine::CCollSphere::pMeshRelese = false;
USING(Engine)
Engine::CCollSphere::CCollSphere(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{

}

Engine::CCollSphere::CCollSphere(const CCollSphere& rhs)
	: CVIBuffer(rhs)
	, pMesh(rhs.pMesh)
{
	Safe_AddRef(pMesh);
	//Ready_Buffer();
}

Engine::CCollSphere::~CCollSphere(void)
{

}

HRESULT Engine::CCollSphere::Ready_Buffer(void)
{
	D3DXCreateSphere(m_pGraphicDev, 1.f, 12, 12, &pMesh, NULL);
	
	m_dwFVF = pMesh->GetFVF();
	if (!(m_dwFVF & D3DFVF_DIFFUSE))
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), m_dwFVF |= D3DFVF_DIFFUSE, m_pGraphicDev, &pMesh);
	}
	else
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), m_dwFVF, m_pGraphicDev, &pMesh);
	}
	
	//
	Set_Color(D3DCOLOR_ARGB(255, 8, 103, 1));


	m_dwTriCnt = pMesh->GetNumFaces();
	m_dwVtxCnt = pMesh->GetNumVertices();
	m_dwVtxSize = pMesh->GetNumBytesPerVertex();
	//for (int i = 0; i < m_dwTriCnt; i++)
	//{
	//	//인덱스 사이즈만큼 넘겨준다
	//}
	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	//FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	FAILED_CHECK_RETURN(pMesh->GetVertexBuffer(&m_pVB), E_FAIL);
	FAILED_CHECK_RETURN(pMesh->GetIndexBuffer(&m_pIB), E_FAIL);
	

	return S_OK;
}

//void CCollSphere::Render_Buffer(void)
//{
//	m_pGraphicDev->SetFVF(m_dwFVF);
//	pMesh->DrawSubset(0);
//}

void Engine::CCollSphere::Set_Color(D3DCOLOR color)
{
	void* pVertex = nullptr;
	pMesh->LockVertexBuffer(0, &pVertex);
	// 색상 정보 위치 찾기 (초록색으로 바꾸려고)
	D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
	ZeroMemory(decl, sizeof(D3DVERTEXELEMENT9) * MAX_FVF_DECL_SIZE);

	pMesh->GetDeclaration(decl);

	BYTE offset = 0;
	for (int i = 0; i < MAX_FVF_DECL_SIZE; i++)
	{
		if (decl[i].Usage == D3DDECLUSAGE_COLOR)
		{
			offset = (BYTE)decl[i].Offset;
			break;
		}
	}
	int vertexCount = pMesh->GetNumVertices();
	int vertexSize = D3DXGetFVFVertexSize(m_dwFVF);
	for (int i = 0; i < vertexCount; ++i)
	{
		DWORD* pColor = ((DWORD*)(((BYTE*)pVertex) + (i * vertexSize + offset)));
		*pColor = color;
	}

	pMesh->UnlockVertexBuffer();

	return;
}

Engine::CCollSphere* Engine::CCollSphere::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCollSphere* pInstance = new CCollSphere(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent* CCollSphere::Clone(void)
{
	return new CCollSphere(*this);
}

void Engine::CCollSphere::Free(void)
{
	CVIBuffer::Free();
	
	Safe_Release(pMesh);
	
}

