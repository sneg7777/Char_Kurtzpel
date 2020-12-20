#ifndef MainApp_h__
#define MainApp_h__

#include "Define.h"
#include "Base.h"
#include "Engine_Define.h"
#include "Export_Function.h"

BEGIN(Engine)

class CGraphicDev;
class CManagement;

END
BEGIN(Client)

class CMainApp : public CBase
{
private: // 생성자, 소멸자
	explicit CMainApp(void);
	virtual ~CMainApp(void);

	// public 함수, 변수
public:
	HRESULT		Ready_MainApp(void);
	_int		Update_MainApp(const _float& fTimeDelta);
	void		Render_MainApp(void);
	// protected 함수, 변수
	// private 함수, 변수
private:
	HRESULT		SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
	HRESULT		Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement** ppManagement);

private:
	Engine::CGraphicDev*		m_pDeviceClass = nullptr;
	Engine::CManagement*		m_pManagementClass = nullptr;
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;

	_ulong		m_dwRenderCnt = 0;
	_tchar		m_szFPS[256];
	_float		m_fTime = 0.f;

	// 생성, 소멸 함수
public:
	static CMainApp*	Create(void);
	virtual void		Free(void);

};

END
#endif // MainApp_h__
