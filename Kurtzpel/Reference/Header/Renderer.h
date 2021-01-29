#ifndef Renderer_h__
#define Renderer_h__

#include "Engine_Define.h"
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer : public CComponent
{
	DECLARE_SINGLETON(CRenderer)

private:
	explicit CRenderer(void);
	virtual ~CRenderer(void);

public:
	void	Add_RenderGroup(RENDERID eGroup, CGameObject* pGameObject);
	void	Render_GameObject(LPDIRECT3DDEVICE9 & pGraphicDev);
	void	Clear_RenderGroup(void);
	HRESULT	Ready_Renderer(LPDIRECT3DDEVICE9& pGraphicDev);
	void	Set_MotionBlur(float _power) { m_MotionBlurActive = true; m_MotionBlurPower = _power; }
	void	Dec_MotionPower(float fTimeDelta) { m_MotionBlurPower -= fTimeDelta; if (m_MotionBlurPower < 0.f) { m_MotionBlurActive = false; } }

private:
	void		Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_UI(LPDIRECT3DDEVICE9& pGraphicDev);

	void		Render_Deferred(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_LightAcc(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Render_Blend(LPDIRECT3DDEVICE9& pGraphicDev);
	
private:
	list<CGameObject*>				m_RenderGroup[RENDER_END];
	LPDIRECT3DVERTEXBUFFER9			m_pVB;
	LPDIRECT3DINDEXBUFFER9			m_pIB;

	bool							m_MotionBlurActive;
	float							m_MotionBlurPower;
private:
	virtual CComponent*	Clone() { return nullptr; }
	virtual void Free(void);
};

END
#endif // Renderer_h__
