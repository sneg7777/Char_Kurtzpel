#ifndef CameraScene_Manager_h__
#define CameraScene_Manager_h__

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
class CDynamicCamera;
class CCameraScene_Manager
{
public:
	static CCameraScene_Manager* Get_Instance() {
		if (nullptr == m_pInstance) {
			m_pInstance = new CCameraScene_Manager;
		}
		return m_pInstance;
	}
	static void Destroy_Instance();
private:
	CCameraScene_Manager();
	~CCameraScene_Manager();

public:
	HRESULT Ready_Object(LPDIRECT3DDEVICE9 pGraphicDev);
	_int Update_Object(const _float& fTimeDelta);
	void Render_Object(void);
	void Update_CameraScene(const _float& fTimeDelta);
	void Update_CameraScene_QuestNumber_7(const _float& fTimeDelta);
	void Update_CameraScene_SkillZ_GH(const _float& fTimeDelta);
	void Update_CameraScene_SkillZ_LB(const _float & fTimeDelta);
	int	 Get_CameraScene() { return m_CameraSceneNumber; }
	void Set_CameraScene(float _number);

private:
	static CCameraScene_Manager*			m_pInstance;
	LPDIRECT3DDEVICE9						m_pGraphicDev;
	int										m_CameraSceneNumber = 0;
	float									m_CameraSceneTime = 0.f;
	CDynamicCamera*							m_Camera;
	_vec3									m_CameraPosInit = { 0.f, 0.f, 0.f };
	_vec3									m_CameraBeforeNomal = { 0.f, 0.f, 0.f };

};

END
#endif // UI_h__
