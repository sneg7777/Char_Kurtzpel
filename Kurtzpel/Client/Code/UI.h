#ifndef UI_h__
#define UI_h__

#include "GameObject.h"
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
class CUI : public Engine::CGameObject
{
public:
	enum UIKind {
		UIK_None,
		UIK_SkillZ_GH, UIK_SkillQ_GH, UIK_SkillE_GH, UIK_SkillF_GH, UIK_SkillLShift, UIK_SkillTab,
		UIK_SkillZ_LB, UIK_SkillQ_LB, UIK_SkillE_LB, UIK_SkillF_LB,
		UIK_SkillFrameZ, UIK_SkillFrameQ, UIK_SkillFrameE, UIK_SkillFrameF, UIK_FrameLShift, UIK_FrameTab,
		UIK_SkillFrameZCool, UIK_SkillFrameQCool, UIK_SkillFrameECool, UIK_SkillFrameFCool, UIK_FrameLShiftCool, UIK_FrameTabCool,
		UIK_KeyQ, UIK_KeyE, UIK_KeyF, UIK_KeyZ, UIK_KeyLShift, UIK_KeyTab,
		UIK_Player_Hp0, UIK_Player_Hp1, UIK_Player_Hp2, UIK_Player_Hp3,
		UIK_Monster_Hp0, UIK_Monster_Hp1, UIK_Monster_Hp2, UIK_Monster_Hp3,
		UIK_TipText, UIK_TalkText, UIK_Art, UIK_TipNpcTalk, UIK_TipPortal,
		UIK_End
	};
public:
	CUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT & pEffect);
	void Set_PosToSize(float fX, float fY, float fSizeX, float fSizeY) {
		m_fX = fX, m_fY = fY, m_fSizeX = fSizeX, m_fSizeY = fSizeY;
	}
protected:
	HRESULT		Add_Component(void);

public:
	Engine::CRcTex*				m_pBufferCom = nullptr;
	Engine::CTexture*			m_pTextureCom = nullptr;
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CTransform*			m_pTransformCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
public:
	UIKind						m_UIKind = UIKind::UIK_End;
	CPlayer::Weapon_Equip		m_Weapon = CPlayer::Weapon_Equip::Weapon_None;
protected:
	_matrix						m_matProj;
	_float						m_fX, m_fY;
	_float						m_fSizeX, m_fSizeY;
	_float						m_fAlpha = 0.f;
	bool						m_ShaderCheck = false;
public:
	static CUI*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif // UI_h__
