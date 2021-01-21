#ifndef NpcQuest_Manager_h__
#define NpcQuest_Manager_h__

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
class CNpcQuest_Manager
{
public:
	struct NpcQuestInfo
	{
		int		m_QuestNumber = 0;
		int		m_TalkNumber = 0;
		bool	m_PlayerColl = false;
		bool	m_PlayerTalk = false;
	};
public:
	static CNpcQuest_Manager* Get_Instance() {
		if (nullptr == m_pInstance) {
			m_pInstance = new CNpcQuest_Manager;
		}
		return m_pInstance;
	}
	static void Destroy_Instance();
private:
	CNpcQuest_Manager();
	~CNpcQuest_Manager();

public:
	HRESULT Ready_Object(LPDIRECT3DDEVICE9 pGraphicDev);
	_int Update_Object(const _float& fTimeDelta);
	void Render_Object(void);
	NpcQuestInfo* Get_NpcQuestInfo() { return &m_NpcQuestInfo; }
	
private:
	static CNpcQuest_Manager*				m_pInstance;
	LPDIRECT3DDEVICE9						m_pGraphicDev;
	NpcQuestInfo							m_NpcQuestInfo;
};

END
#endif // UI_h__
