#ifndef Stage_h__
#define Stage_h__

#include "Scene.h"
#include "Define.h"

#include "SkyBox.h"
#include "Terrain.h"
#include "Player.h"
#include "DynamicCamera.h"
#include "Monster.h"


#include "Stone.h"
#include "Sword.h"
#include "Equip_Top_01.h"
#include "Tree.h"
#include "Effect.h"
#include "UI.h"

BEGIN(Client)

class CStage : public Engine::CScene
{
private:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage(void);

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual _int LateUpdate_Scene(const _float& fTimeDelta);
	virtual void Render_Scene(void) override;

	CDynamicCamera* Get_DynamicCamera() { return m_Camera; }
	void CameraControl(_float fTimeDelta);
		
private:
	HRESULT		Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT		Ready_GameLogic_Dynamic_Layer(const _tchar* pLayerTag);
	HRESULT		Ready_GameLogic_Static_Layer(const _tchar* pLayerTag);
	HRESULT		Ready_UI_Layer(const _tchar* pLayerTag);

	HRESULT		Ready_LightInfo(void);
	CDynamicCamera* m_Camera = nullptr;

public:
	static CStage*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void) override;
	void Collision_Object();
};

END
#endif // Stage_h__
