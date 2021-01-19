#ifndef Line_h__
#define Line_h__

#include "Engine_Define.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLine : public CBase
{
public:
	enum POINT { POINT_START, POINT_FINISH, POINT_END};
	enum COMPARE {		COMPARE_LEFT, COMPARE_RIGHT	};

private:
	explicit CLine(void);
	virtual ~CLine(void);

public:
	HRESULT		Ready_Line(const _vec2* pPointA, const _vec2* pPointB);
	COMPARE		Compare(const _vec2* pPos);
	_vec2		Get_vPoint(CLine::POINT point) { return m_vPoint[point]; }
	_vec2		Get_vNormal() { return m_vNormal; }
private:
	_vec2		m_vPoint[POINT_END];
	_vec2		m_vDirection;
	_vec2		m_vNormal;

public:
	static CLine*		Create(const _vec2* pPointA, const _vec2* pPointB);
	virtual void		Free(void);
};

END
#endif // Line_h__
