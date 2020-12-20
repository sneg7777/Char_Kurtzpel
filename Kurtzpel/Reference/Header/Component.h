#ifndef Component_h__
#define Component_h__

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CComponent : public CBase
{
protected:
	explicit CComponent(void);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent(void);

public:
	virtual _int	Update_Component(const _float& fTimeDelta);

protected:
	_bool		m_bClone;

public:
	virtual CComponent*		Clone(void)PURE;
protected:
	virtual void			Free(void)PURE;
};

END
#endif // Component_h__
