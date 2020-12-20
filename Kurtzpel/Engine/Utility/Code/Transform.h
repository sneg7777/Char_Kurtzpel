#ifndef Transform_h__
#define Transform_h__

#include "Component.h"
#include "Pipeline.h"

BEGIN(Engine)

class ENGINE_DLL CTransform : public CComponent
{
private:
	explicit CTransform(void);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform(void);

public:
	void			Set_Transform(LPDIRECT3DDEVICE9& pGraphicDev)const;
	//void			Set_Transform(LPDIRECT3DDEVICE9& pGraphicDev, _matrix* pMatrix, D3DTRANSFORMSTATETYPE iFlag)const;
	void			Set_Scale(const _float& fX, const _float& fY, const _float& fZ);
	void			Get_Info(INFO eType, _vec3* pInfo);
	
	void			Move_Pos(const _vec3* const pDir);
	void			Pick_Pos(const _vec3* pPos, const _float & fSpeed, const _float& fTimeDelta);
	void			Set_Pos(const _vec3* pPos);
	void			Set_Pos(const _float& fX, const _float& fY, const _float& fZ);
	void			Move_Pos(const float& fX, const float& fY, const float& fZ);
	void			Rotation(ROTATION eType, const _float& fAngle);
	
	void			Set_WorldMatrix(const _matrix* pMatrix);
	void			Set_ParentMatrix(const _matrix* pParent);

	void			Chase_Target(const _vec3* pTargetPos, const _float& fSpeed, const _float& fTimeDelta);
	const _matrix*	Compute_LookAtTarget(const _vec3* pTargetPos);
	void			Get_WorldMatrix(_matrix* pWorldMatrix)const;
	const _matrix*	Get_NRotWorldMatrix(_matrix* pWorld) const;
	const _matrix*	Get_WorldMatrix(void) const;
public:
	HRESULT			Ready_Transform(void);
	virtual _int	Update_Component(const _float& fTimeDelta);

public:
	_vec3			m_vInfo[INFO_END];
	_vec3			m_vScale;
	_vec3			m_vAngle;
	_matrix			m_matWorld;
	_matrix			m_matNRotWorld;

public:
	static		CTransform*		Create(void);
	virtual CComponent*		Clone(void);
private:	
	virtual		void		Free(void);

};

END
#endif // Transform_h__
