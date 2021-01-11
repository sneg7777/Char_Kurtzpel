#ifndef LongBow_h__
#define LongBow_h__

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
class CLongBow : public CUnit
{
public:
	enum LongBowState {
		LongBowState_Idle, LongBowState_Attack, LongBowState_End
	};
public:
	static CLongBow* CLongBow::GetInstance(void) {
		if (nullptr == m_pInstance) {
			return nullptr;
		}
		return m_pInstance;
	}
	static void CLongBow::DestroyInstance(void) {
		if (nullptr != m_pInstance) {
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}
	static CLongBow* m_pInstance;
private:
	explicit CLongBow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLongBow(void);

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
	_bool		Collision_ToObject(const _tchar* pLayerTag, const _tchar* pObjTag);

private:

	const	_matrix*			m_pParentBoneMatrix = nullptr;
	const	_matrix*			m_pParentWorldMatrix = nullptr;

	float	m_RocationX = 0.f;
	float	m_RocationY = 0.f;
	float	m_RocationZ = 0.f;
public:
	static CLongBow*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free(void) override;

};

END
#endif // Sword_h__
