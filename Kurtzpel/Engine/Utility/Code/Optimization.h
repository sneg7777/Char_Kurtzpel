#ifndef Optimization_h__
#define Optimization_h__

#include "Component.h"
#include "Frustum.h"
#include "QuadTree.h"

BEGIN(Engine)

class ENGINE_DLL COptimization : public CComponent
{
private:
	explicit COptimization(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit COptimization(const COptimization& rhs);
	virtual ~COptimization(void);

public:
	HRESULT				Ready_Optimization(_bool bChoice, const _ulong& dwCntX, const _ulong& dwCntZ);
	_bool				Is_InFrustumForObject(const _vec3* pWorldPos, const _float& fRadius);
	void				Is_InFrustumForTerrain(const _vec3* pVtxPos, 
												const _ulong& dwCntX, 
												const _ulong& dwCntZ, 
												Engine::INDEX32* pIndex, 
												_ulong* pTriCnt);
private:
	CFrustum*			m_pFrustum;
	CQuadTree*			m_pQuadTree;
	LPDIRECT3DDEVICE9	m_pGraphicDev;

public:
	static COptimization*			Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool bChoice, const _ulong& dwCntX, const _ulong& dwCntZ);
	virtual CComponent*				Clone(void);
	virtual void					Free(void);
};

END
#endif // Optimization_h__
