#ifndef Effect_h__
#define Effect_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CRenderer;
class CTransform;
class CShader;

END

BEGIN(Client)
class CEffect : public Engine::CGameObject
{
private:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffect(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		Add_Component(void);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:
	Engine::CRcTex*				m_pBufferCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CTransform*			m_pTransformCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;

	_float						m_fFrame = 0.f;

public:
	static CEffect*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif // Effect_h__
