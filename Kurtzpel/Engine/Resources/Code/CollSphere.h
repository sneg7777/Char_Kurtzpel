#ifndef CollSphere_h__
#define CollSphere_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCollSphere : public CVIBuffer
{
protected:
	explicit CCollSphere(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCollSphere(const CCollSphere& rhs);
	virtual ~CCollSphere(void);

public:
	virtual HRESULT	Ready_Buffer(void);

	void Set_Color(D3DCOLOR color);

public:
	static CCollSphere* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void);
	virtual void Free(void);
	LPD3DXMESH	pMesh;

	bool colorGreenCheck = true;
	bool colorGreen = true;

	static bool pMeshRelese;
};
END

#endif