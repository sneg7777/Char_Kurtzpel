#ifndef Stage_1_h__
#define Stage_1_h__

#include "Stage.h"
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

class CStage_1 : public CStage
{
private:
	explicit CStage_1(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage_1(void);

public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual _int LateUpdate_Scene(const _float& fTimeDelta);
	virtual void Render_Scene(void) override;
		
protected:
	virtual HRESULT		Ready_Environment_Layer(const _tchar* pLayerTag);
	virtual HRESULT		Ready_GameLogic_Dynamic_Layer(const _tchar* pLayerTag);
	virtual HRESULT		Ready_GameLogic_Static_Layer(const _tchar* pLayerTag);
	virtual HRESULT		Ready_GameLogic_StaticNoColl_Layer(const _tchar* pLayerTag);
	virtual HRESULT		Ready_GameLogic_Decoration_Layer(const _tchar* pLayerTag);
	virtual HRESULT		Ready_UI_Layer(const _tchar* pLayerTag);
	virtual HRESULT		Ready_LightInfo(void);

public:
	static CStage_1*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual void Free(void) override;
	HRESULT Load_StaticObject(Engine::CLayer* _layer, const _tchar* pLayerTag);
};

END
#endif // Stage_h__
