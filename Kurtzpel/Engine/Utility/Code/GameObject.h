#ifndef GameObject_h__
#define GameObject_h__

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CGameObject : public CBase
{
public:
	enum EnumDir {
		Up, UpLeft, UpRight, Down, DownLeft, DownRight, Left, Right, Dir_End
	};
	enum UnitName {
		Player, PlayerBullet, Monster, Boss, MonsterBullet, NameEnd
	};
struct sStat {
	EnumDir		m_EnumDir = EnumDir::Dir_End;

	_vec3						m_vDir;
	_float						m_fInitSpeed = 10.f;
	_float						m_fSpeed = 7.5f;

	_float						m_fMaxHp = 0.f;
	_float						m_fHp = 0.f;
	_float						m_fMaxDelayHp = 0.f;
	_float						m_fDelayHp = 0.f;
	_float						m_fAttack = 0.f;
	_ulong						m_dwNaviIndex = 0.f;
	_float						m_fMaxMp = 0;
	_float						m_fMp = 0;
	_bool						m_IsDead = false;
	//
	_float						m_fMaxKnockBackHp = 0.f;
	_float						m_fKnockBackHp = 0.f;
	_float						m_fKnockBackPower = 1.f;
	_vec3						m_fKnockBackDir;
};
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGameObject(void);

public:
	CComponent*	   Get_Component(const _tchar* pComponentTag, COMPONENTID eID);
	_float			Get_ViewZ(void) const { return m_fViewZ; }

	void			Compute_ViewZ(const _vec3* pPos);
	void			Emplace_mapComponent(COMPONENTID _comId, _tchar* _name, CComponent* _com) {
		m_mapComponent[_comId].emplace(_name, _com);
	}
public:
	virtual HRESULT			Ready_Object(void) { return S_OK; };
	virtual _int			Update_Object(const _float& fTimeDelta);
	virtual void			Render_Object(void) {};
	virtual void			Collision(CGameObject* _col);
	sStat*					Get_sStat() { return &m_sStat; }
private:
	CComponent*		Find_Component(const _tchar* pComponentTag, COMPONENTID eID);

protected:
	LPDIRECT3DDEVICE9						m_pGraphicDev;
	map<const _tchar*, CComponent*>			m_mapComponent[ID_END];
	_float									m_fViewZ = 0.f;
	UnitName								m_UnitName = UnitName::NameEnd;
	sStat									m_sStat;
public:
	virtual void Free(void);


};
END
#endif // GameObject_h__
