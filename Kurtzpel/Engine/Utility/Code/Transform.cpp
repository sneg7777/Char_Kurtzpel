#include "Transform.h"

USING(Engine)

Engine::CTransform::CTransform(void)
	: m_vScale(1.f, 1.f, 1.f)
	, m_vAngle(0.f, 0.f, 0.f)
{
	ZeroMemory(m_vInfo, sizeof(_vec3) * INFO_END);
	D3DXMatrixIdentity(&m_matWorld);
}

Engine::CTransform::CTransform(const CTransform& rhs)
	: m_vScale(rhs.m_vScale)
	, m_vAngle(rhs.m_vAngle)
{
	for (_uint i = 0; i < INFO_END; ++i)
		m_vInfo[i] = rhs.m_vInfo[i];
	
	m_matWorld = rhs.m_matWorld;	
}

Engine::CTransform::~CTransform(void)
{

}

HRESULT CTransform::Ready_Transform(void)
{
	D3DXMatrixIdentity(&m_matWorld);

	for (_uint i = 0; i < INFO_END; ++i)
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));
	
	return S_OK;
}

_int CTransform::Update_Component(const _float & fTimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matNRotWorld);

	// 위치 벡터를 제외하고 나머지 벡터의 초기화 값을 세팅한 상황
	for (_uint i = 0; i < INFO_POS; ++i)
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));

	// 크기 변환 적용

	for (_uint i = 0; i < INFO_POS; ++i)
	{
		D3DXVec3Normalize(&m_vInfo[i], &m_vInfo[i]);
		m_vInfo[i] *= *(((_float*)&m_vScale + i));
	}
	for (_uint i = 0; i < INFO_END; ++i)
		memcpy(&m_matNRotWorld.m[i][0], m_vInfo[i], sizeof(_vec3));


	// 회전 적용
	_matrix		matRot[ROT_END];
	D3DXMatrixRotationX(&matRot[ROT_X], m_vAngle.x);
	D3DXMatrixRotationY(&matRot[ROT_Y], m_vAngle.y);
	D3DXMatrixRotationZ(&matRot[ROT_Z], m_vAngle.z);

	for (_uint i = 0; i < INFO_POS; ++i)
	{
		for (_uint j = 0; j < ROT_END; ++j)
		{
			D3DXVec3TransformNormal(&m_vInfo[i], &m_vInfo[i], &matRot[j]);
		}
	}

	/*CPipeline::MyRotationX(&m_vInfo[INFO_RIGHT], &m_vInfo[INFO_RIGHT], m_vAngle.x);
	CPipeline::MyRotationX(&m_vInfo[INFO_UP], &m_vInfo[INFO_UP], m_vAngle.x);
	CPipeline::MyRotationX(&m_vInfo[INFO_LOOK], &m_vInfo[INFO_LOOK], m_vAngle.x);

	CPipeline::MyRotationY(&m_vInfo[INFO_RIGHT], &m_vInfo[INFO_RIGHT], m_vAngle.y);
	CPipeline::MyRotationY(&m_vInfo[INFO_UP], &m_vInfo[INFO_UP], m_vAngle.y);
	CPipeline::MyRotationY(&m_vInfo[INFO_LOOK], &m_vInfo[INFO_LOOK], m_vAngle.y);

	CPipeline::MyRotationZ(&m_vInfo[INFO_RIGHT], &m_vInfo[INFO_RIGHT], m_vAngle.z);
	CPipeline::MyRotationZ(&m_vInfo[INFO_UP], &m_vInfo[INFO_UP], m_vAngle.z);
	CPipeline::MyRotationZ(&m_vInfo[INFO_LOOK], &m_vInfo[INFO_LOOK], m_vAngle.z);*/

	for (_uint i = 0; i < INFO_END; ++i)
		memcpy(&m_matWorld.m[i][0], &m_vInfo[i], sizeof(_vec3));
	
	return _int();
}

Engine::CComponent* Engine::CTransform::Clone(void)
{
	return new CTransform(*this);
}

void Engine::CTransform::Free(void)
{

}

Engine::CTransform* Engine::CTransform::Create(void)
{
	CTransform*	pInstance = new CTransform;

	if (FAILED(pInstance->Ready_Transform()))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CTransform::Set_Transform(LPDIRECT3DDEVICE9& pGraphicDev) const
{
	pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);
}

void Engine::CTransform::Set_Scale(const float& fX, const float& fY, const float& fZ)
{
	m_vScale.x = fX;
	m_vScale.y = fY;
	m_vScale.z = fZ;
}

void CTransform::Get_Info(INFO eType, _vec3 * pInfo)
{
	memcpy(pInfo, &m_matWorld.m[eType][0], sizeof(_vec3));
}

void CTransform::Move_Pos(const _vec3 * const pDir)
{
	m_vInfo[INFO_POS] += *pDir;
}

void CTransform::Move_Pos(const float & fX, const float & fY, const float & fZ)
{
	m_vInfo[INFO_POS].x = fX;
	m_vInfo[INFO_POS].y = fY;
	m_vInfo[INFO_POS].z = fZ;
}

void Engine::CTransform::Pick_Pos(const _vec3* pPos, const _float & fSpeed, const _float& fTimeDelta)
{
	_vec3		vDir = *pPos - m_vInfo[INFO_POS];
	D3DXVec3Normalize(&vDir, &vDir);

	m_vInfo[INFO_POS] += vDir * fSpeed * fTimeDelta;
}

void CTransform::Set_Pos(const _vec3* pPos, bool checkY)
{
	//m_vInfo[INFO_POS] = *pPos;
	m_vInfo[INFO_POS].x = pPos->x;
	m_vInfo[INFO_POS].z = pPos->z;
	if(checkY)
		m_vInfo[INFO_POS].y = pPos->y;
}

void CTransform::Set_Pos(const _float & fX, const _float & fY, const _float & fZ)
{
	m_vInfo[INFO_POS].x = fX;
	m_vInfo[INFO_POS].y = fY;
	m_vInfo[INFO_POS].z = fZ;

}

void CTransform::Rotation(ROTATION eType, const _float & fAngle, bool substitution)
{
	if(!substitution)
		*(((_float*)&m_vAngle) + eType) += fAngle;
	else
		*(((_float*)&m_vAngle) + eType) = fAngle;
}

void CTransform::Set_WorldMatrix(const _matrix * pMatrix)
{
	m_matWorld = *pMatrix;
}

void CTransform::Set_ParentMatrix(const _matrix * pParent)
{
	m_matWorld *= *pParent;
}
void CTransform::Chase_Target(const _vec3 * pTargetPos, const _float & fSpeed, const _float& fTimeDelta)
{
	_vec3		vDir = *pTargetPos - m_vInfo[INFO_POS];
	D3DXVec3Normalize(&vDir, &vDir);

	m_vInfo[INFO_POS] += vDir * fSpeed * fTimeDelta;

	_matrix		matRot, matScale, matTrans;

	matRot = *Compute_LookAtTarget(pTargetPos);

	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);
	D3DXMatrixTranslation(&matTrans, m_vInfo[INFO_POS].x,
									m_vInfo[INFO_POS].y,
									m_vInfo[INFO_POS].z);
	
	m_matWorld = matScale * matRot * matTrans;

}
// 임의의 축 회전을 하는 행렬을 생성하는 함수
const _matrix * CTransform::Compute_LookAtTarget(const _vec3 * pTargetPos)
{
	_vec3		vDir = *pTargetPos - m_vInfo[INFO_POS];
	_vec3		vAxis, vUp;
	_matrix		matRot;
		
	/*D3DXVec3Cross(&vAxis, &m_vInfo[INFO_UP], &vDir);
	D3DXVec3Normalize(&vUp, &m_vInfo[INFO_UP]);
	D3DXVec3Normalize(&vDir, &vDir);

	_float	fAngle = acosf(D3DXVec3Dot(&vUp, &vDir));

	D3DXMatrixRotationAxis(&matRot, &vAxis, fAngle);*/
	
	return D3DXMatrixRotationAxis(&matRot,
								D3DXVec3Cross(&vAxis, &m_vInfo[INFO_UP], &vDir), 
								acosf(D3DXVec3Dot(D3DXVec3Normalize(&vUp, &m_vInfo[INFO_UP]), 
												  D3DXVec3Normalize(&vDir, &vDir))));
}

void Engine::CTransform::Get_WorldMatrix(_matrix* pWorldMatrix) const
{
	*pWorldMatrix = m_matWorld;	
}

const _matrix * CTransform::Get_NRotWorldMatrix(_matrix * pWorld) const
{
	if (nullptr != pWorld)
		*pWorld = m_matNRotWorld;

	return &m_matNRotWorld;
}

const _matrix * CTransform::Get_WorldMatrix(void) const
{
	return &m_matWorld;
}

//void Engine::CTransform::Set_Transform(LPDIRECT3DDEVICE9& pGraphicDev, _matrix* pMatrix, D3DTRANSFORMSTATETYPE iFlag) const
//{
//	pGraphicDev->SetTransform(iFlag, pMatrix);
//}
