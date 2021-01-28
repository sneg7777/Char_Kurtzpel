#ifndef DynamicCamera_h__
#define DynamicCamera_h__

#include "Camera.h"
#include "Define.h"

BEGIN(Client)
class CPlayer;
class CDynamicCamera : public Engine::CCamera
{
private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDynamicCamera(void);

public:
	HRESULT Ready_Object(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp,	
						 const _float& fFovY, const _float& fAspect, const _float& fNear,	const _float& fFar );

	virtual _int Update_Object(const _float& fTimeDelta) override;
	void		Set_ShakeTime(float _time) { m_InitShakeTime = m_ShakeTime = _time; }
	void		Set_PosToAt(_vec3 _eye, _vec3 _at) { m_vEye = _eye; m_vAt = _at; }
	_vec3*		Get_pPos() { return &m_vEye; }
	_vec3*		Get_pAt() { return &m_vAt; }
	void		Shake_CameraMove(const _float & fTimeDelta);
private:
	void		Key_Input(const _float& fTimeDelta);
	void		Mouse_Move(void);
	void		Mouse_Fix(void);

private:
	_bool		m_bClick = false;
	_bool		m_bFix = true;
	CPlayer*	m_Player = nullptr;
	_float		m_ShakeTime = 0.f;
	_float		m_InitShakeTime = 0.f;
	_float		m_ShakeX = 0.f;
	_float		m_ShakeY = 0.f;
public:
	static CDynamicCamera*	Create(LPDIRECT3DDEVICE9 pGraphicDev, 
									const _vec3* pEye, 
									const _vec3* pAt, 
									const _vec3* pUp, 
									const _float& fFovY = D3DXToRadian(60.f),
									const _float& fAspect = _float(WINCX) / WINCY, 
									const _float& fNear = 0.1f, 
									const _float& fFar = 1000.f);

	void		Set_pPlayer(CPlayer* _player) { m_Player = _player; }
private:
	void Move_NpcCamera(const _float & fTimeDelta);
	void Move_NpcCamera_No(const _float & fTimeDelta);
	void LB_SkillZScene(const _float& fTimeDelta);
	virtual void Free(void) override;


};

END
#endif // DynamicCamera_h__
