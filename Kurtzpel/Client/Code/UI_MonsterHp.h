#ifndef UI_MonsterHp_h__
#define UI_MonsterHp_h__

#include "UI.h"
#include "Define.h"
#include "Player.h"
BEGIN(Engine)

class CRcTex;
class CTexture;
class CRenderer;
class CTransform;
class CShader;

END

BEGIN(Client)
class CUI_MonsterHp : public CUI
{
private:
	explicit CUI_MonsterHp(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI_MonsterHp(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
private:
	HRESULT		Add_Component(void);

public:
	static CUI_MonsterHp*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif // UI_h__
