#ifndef Stage_h__
#define Stage_h__

#include "Scene.h"
#include "Define.h"

#include "SkyBox.h"
#include "Terrain.h"
#include "DynamicCamera.h"
#include "Monster.h"


#include "Stone.h"
#include "Sword.h"
#include "Equip_Top_01.h"
#include "Tree.h"
#include "Effect.h"
#include "UI.h"

BEGIN(Client)
class CNaviTerrain;
class CArrowNaviTerrain;

class CStage : public Engine::CScene
{
protected:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage(void);

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual _int LateUpdate_Scene(const _float& fTimeDelta);
	virtual void Render_Scene(void) override;

	CDynamicCamera* Get_DynamicCamera() { return m_Camera; }
		
protected:
	virtual HRESULT		Ready_Environment_Layer(const _tchar* pLayerTag);
	virtual HRESULT		Ready_GameLogic_Dynamic_Layer(const _tchar* pLayerTag);
	virtual HRESULT		Ready_GameLogic_DynamicNoColl_Layer(const _tchar* pLayerTag);
	virtual HRESULT		Ready_GameLogic_Static_Layer(const _tchar* pLayerTag);
	virtual HRESULT		Ready_GameLogic_StaticNoColl_Layer(const _tchar* pLayerTag);
	virtual HRESULT		Ready_GameLogic_Decoration_Layer(const _tchar* pLayerTag);
	virtual HRESULT		Ready_UI_Layer(const _tchar* pLayerTag);

	virtual HRESULT		Ready_LightInfo(void);
public:
	CNaviTerrain*			m_NaviTerrain = nullptr;
	CArrowNaviTerrain*		m_ArrowNaviTerrain = nullptr;
	CTerrain*				m_Terrain = nullptr;
	static CStage*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	CDynamicCamera*			m_Camera = nullptr;
	virtual void Free(void) override;
	void Collision_OnlyDynamic_Object(const _float& fTimeDelta);
	void Collision_StaticToDynamic_Object(const _float& fTimeDelta);
	HRESULT Load_StaticObject(Engine::CLayer* _layer, const _tchar* pLayerTag);
	void CameraControl(_float fTimeDelta);

	static bool m_LightCheck;
};

END
#endif // Stage_h__
