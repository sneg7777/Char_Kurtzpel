#ifndef Terrain_h__
#define Terrain_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CTerrainTex;
class CTexture;
class CRenderer;
class CTransform;
class COptimization;
class CShader;

END

BEGIN(Client)
struct sComponent_Terrain {
	Engine::CTerrainTex* m_pBufferCom = nullptr;

	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CTexture* m_pFilterCom = nullptr;
	Engine::CTexture* m_pAuraCom = nullptr;

	Engine::CRenderer* m_pRendererCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::COptimization* m_pOptimizationCom = nullptr;
	Engine::CShader* m_pShaderCom = nullptr;
};
class CTerrain : public Engine::CGameObject
{
private:
	explicit CTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTerrain(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;
	sComponent_Terrain* Get_sComponent_Terrain() { return &m_sComponent; }

private:
	HRESULT		Add_Component(void);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:
	sComponent_Terrain			m_sComponent;

	Engine::INDEX32* m_pIndex = nullptr;
	_ulong						m_dwTriCnt = 0;

public:
	static CTerrain* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif // Terrain_h__
