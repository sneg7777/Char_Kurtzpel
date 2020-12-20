#ifndef DynamicMesh_h__
#define DynamicMesh_h__

#include "Mesh.h"
#include "HierarchyLoader.h"
#include "AniCtrl.h"

BEGIN(Engine)

class ENGINE_DLL CDynamicMesh : public CMesh
{
private:
	explicit CDynamicMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDynamicMesh(const CDynamicMesh& rhs);
	virtual ~CDynamicMesh(void);

public:
	HRESULT		Ready_Meshes(const _tchar* pFilePath, const _tchar* pFileName);
	void		Render_Meshes(void);
	void		Set_Animationset(const _uint& iIndex);
	void		Play_Animation(const _double& TimeDelta);

private:
	void		Update_FrameMatrices(D3DXFRAME_DERIVED* pFrame, const _matrix* pParentMatrix);
	void		SetUp_FrameMatricesPointer(D3DXFRAME_DERIVED* pFrame);

private:
	D3DXFRAME*			m_pRootFrame;
	CHierarchyLoader*	m_pLoader;
	CAniCtrl*			m_pAniCtrl;
	list<D3DXMESHCONTAINER_DERIVED*>		m_MeshContainerList;

public:
	static CDynamicMesh* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName);
	virtual CComponent* Clone(void) override;
	virtual void Free(void);
};

END
#endif // DynamicMesh_h__


//typedef struct _D3DXFRAME
//{
//	LPSTR                   Name;
//	D3DXMATRIX              TransformationMatrix;
//
//	LPD3DXMESHCONTAINER     pMeshContainer;
//
//	struct _D3DXFRAME       *pFrameSibling;
//	struct _D3DXFRAME       *pFrameFirstChild;
//
//} D3DXFRAME, *LPD3DXFRAME;
//
//
/*
typedef struct _D3DXMESHCONTAINER
{
	LPSTR                   Name;
	D3DXMESHDATA            MeshData;
	LPD3DXMATERIAL          pMaterials;
	LPD3DXEFFECTINSTANCE    pEffects;
	DWORD                   NumMaterials;
	DWORD                  *pAdjacency;
	LPD3DXSKININFO          pSkinInfo;

	struct _D3DXMESHCONTAINER *pNextMeshContainer;

} D3DXMESHCONTAINER, *LPD3DXMESHCONTAINER;*/