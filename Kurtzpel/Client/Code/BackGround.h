#ifndef BackGround_h__
#define BackGround_h__

#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CRenderer;
class CShader;
class CTransform;

END

BEGIN(Client)
class CBackGround : public Engine::CGameObject
{
private:
	explicit CBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBackGround(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;
	map<const _tchar*, Engine::CComponent*>*	Get_mapComStatic() { return	&m_mapComponent[Engine::ID_STATIC]; }

private:
	HRESULT		Add_Component(void);
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
private:
	Engine::CRcTex*				m_pBufferCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CTransform*			m_pTransformCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;

public:
	static CBackGround*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	Engine::CTexture*			m_pTextureCom = nullptr;

private:
	virtual void Free(void) override;

};

END
#endif // BackGround_h__
