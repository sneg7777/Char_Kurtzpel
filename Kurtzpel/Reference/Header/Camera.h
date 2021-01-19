#ifndef Camera_h__
#define Camera_h__

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera : public CGameObject
{
protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCamera(void);

public:
	virtual HRESULT			Ready_Object(void);
	virtual _int			Update_Object(const _float& fTimeDelta);
	_vec3* Get_pPos()		{ return &m_vEye; }
	_vec3* Get_pAt()		{ return &m_vAt; }
protected:
	_matrix			m_matView, m_matProj;
	_vec3			m_vEye, m_vAt, m_vUp;
	_float			m_fFovY, m_fAspect, m_fNear, m_fFar;

protected:
	virtual void Free(void);
};


END
#endif // Camera_h__
