#ifndef Hammer_h__
#define Hammer_h__

#include "Unit.h"
#include "Define.h"

BEGIN(Engine)

class CStaticMesh;
class CTransform;
class CRenderer;
class CCalculator;
class CCollider;
class CShader;

END

BEGIN(Client)
class CHammer : public CUnit
{
public:
	enum HammerState {
		HammerState_Idle, HammerState_Attack, HammerState_End
	};
public:
	static CHammer* CHammer::GetInstance(void) {
		if (nullptr == m_pInstance) {
			return nullptr;
		}
		return m_pInstance;
	}
	static void CHammer::DestroyInstance(void) {
		if (nullptr != m_pInstance) {
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}
	static CHammer* m_pInstance;
private:
	explicit CHammer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CHammer(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;
	//
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	//
	void		Set_Pos();
	void		BoneAttach(string _frame);
private:
	HRESULT		Add_Component(void);

private:
	
	//_bool						m_bColl = false;
	const	_matrix*			m_pParentBoneMatrix = nullptr;
	const	_matrix*			m_pParentWorldMatrix = nullptr;
	Engine::CTexture*			m_pTexture = nullptr;
	float	m_RocationX = 0.f;
	float	m_RocationZ = 0.f;
	bool	m_IsTest = false;
	float	m_fDeltaTime = 0.f;
public:
	static CHammer*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif // Sword_h__
