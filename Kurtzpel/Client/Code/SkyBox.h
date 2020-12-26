#ifndef SkyBox_h__
#define SkyBox_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CCubeTex;
class CTexture;
class CRenderer;
class CTransform;
class CShader;

END

BEGIN(Client)
class CSkyBox : public Engine::CGameObject
{
private:
	explicit CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSkyBox(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;

private:
	HRESULT		Add_Component(void);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);

private:
	Engine::CCubeTex*				m_pBufferCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CTransform*			m_pTransformCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
public:
	static CSkyBox*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif // SkyBox_h__
