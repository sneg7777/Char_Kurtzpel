#ifndef Layer_h__
#define Layer_h__

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CLayer : public CBase
{
private:
	explicit CLayer(void);
	virtual ~CLayer();

public:
	CComponent*	   Get_Component(const _tchar* pObjTag, const _tchar* pComponentTag, COMPONENTID eID);
	multimap<const _tchar*, CGameObject*>* Get_mapObject() { return &m_mapObject; }
public:
	HRESULT		Add_GameObject(const _tchar* pObjTag, CGameObject* pGameObject);
	HRESULT		Ready_Layer(void);
	_int		Update_Layer(const _float& fTimeDelta);

private:
	multimap<const _tchar*, CGameObject*>			m_mapObject;

public:
	static CLayer*		Create(void);
	virtual void		Free(void);
};

END
#endif // Layer_h__
