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
		bool	m_NpcColl = false;
		bool	m_PlayerTalk = false;
		int		m_AttackCount = 0;
		int		m_SkillQCount = 0;
		int		m_SkillECount = 0;
		int		m_SkillFCount = 0;
		int		m_RollingCount = 0;
		//bool	m_ClearEffect = false;
		int		m_DeadApostle = 0;
		int		m_DeadTwoHand = 0;
		bool	m_WeaponChange = true;
		bool	m_TalkEnd = false; // 카메라이동쪽
		bool	m_PortalColl = false;
		bool	m_MapMove = false;
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
	void QusetProgress(const _float& fTimeDelta);
	void Portal_MapMove();
	static NpcQuestInfo* Get_NpcQuestInfo() { return &m_pInstance->m_NpcQuestInfo; }
	
private:
	static CNpcQuest_Manager*				m_pInstance;
	LPDIRECT3DDEVICE9						m_pGraphicDev;
	NpcQuestInfo							m_NpcQuestInfo;
};

END
#endif // UI_h__
