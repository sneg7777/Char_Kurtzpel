#ifndef UI_Manager_h__
#define UI_Manager_h__

#include "GameObject.h"
#include "Define.h"
#include "UI.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CRenderer;
class CTransform;
class CShader;

END

BEGIN(Client)
class CUI;
class CUI_Manager
{
public:
	static CUI_Manager* Get_Instance() {
		if (nullptr == m_pInstance) {
			m_pInstance = new CUI_Manager;
		}
		return m_pInstance;
	}
	static void Destroy_Instance();
private:
	CUI_Manager();
	~CUI_Manager();

public:
	HRESULT Ready_Object(LPDIRECT3DDEVICE9 pGraphicDev);
	_int Update_Object(const _float& fTimeDelta);
	void Render_Object(void);

	void Ready_SkillIcon();
	void Init_SkillIcon(CUI* pUI, int number);
	CUI* Get_UI(CUI::UIKind _uiKind);
	CUnit* Get_DamagedEnemy() { return m_pDamagedEnemy; }
	void Set_DamagedEnemy(CUnit* _unit) { m_pDamagedEnemy = _unit; }
	float Get_DamagedTime() { return m_DamagedInitTime; }
	void Set_DamagedTime(float _time) { m_DamagedInitTime = _time; }
	vector<CUI*>							m_ListUI;

private:
	static CUI_Manager*						m_pInstance;
	LPDIRECT3DDEVICE9						m_pGraphicDev;
	CUnit*									m_pDamagedEnemy = nullptr;

	_float									m_DamagedInitTime = 0.f;
};

END
#endif // UI_h__
