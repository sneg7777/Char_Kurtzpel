#ifndef ArrowNaviMesh_h__
#define ArrowNaviMesh_h__

#include "Base.h"
#include "Mesh.h"
#include "Cell.h"

BEGIN(Engine)

class ENGINE_DLL CArrowNaviMesh : public CMesh
{
private:
	explicit CArrowNaviMesh(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CArrowNaviMesh(const CArrowNaviMesh& rhs);
	virtual ~CArrowNaviMesh(void);

public:
	void		Set_NaviIndex(const _ulong& dwIndex) { m_dwIndex = dwIndex; }

public:
	HRESULT		Ready_NaviMeshes(void);
	void		Render_NaviMeshes(void);

	_vec3			Move_OnNaviMesh(const _vec3* pTargetPos, const _vec3* pTargetDir, _ulong* _index, bool* _check = nullptr);

private:
	HRESULT		Link_Cell(void);

private:
	vector<CCell*>			m_vecCell;
	_ulong					m_dwIndex;

public:
	static CArrowNaviMesh*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent*		Clone(void);
	virtual void			Free(void);

};

END
#endif // NaviMesh_h__
