#include "Pipeline.h"

USING(Engine)

Engine::CPipeline::CPipeline(void)
{

}

Engine::CPipeline::~CPipeline(void)
{

}

void Engine::CPipeline::MakeTransformMatrix(_matrix* pMatrix, const _vec3* pRight, const _vec3* pUp, const _vec3* pLook, const _vec3* pPos)
{
	memcpy(&pMatrix->m[0][0], pRight, sizeof(_float) * 3);
	memcpy(&pMatrix->m[1][0], pUp, sizeof(_float) * 3);
	memcpy(&pMatrix->m[2][0], pLook, sizeof(_float) * 3);
	memcpy(&pMatrix->m[3][0], pPos, sizeof(_float) * 3);

}

void CPipeline::MyRotationX(_vec3 * pOut, _vec3 * pIn, _float fAngle)
{
	/*	1	0	0	0
		0	c	s	0
		0	-s	c	0
		0	0	0	1	*/

	_vec3	vTemp = *pIn;

	pOut->x = vTemp.x;
	pOut->y = vTemp.y * cosf(fAngle) - vTemp.z * sinf(fAngle);
	pOut->z = vTemp.y * sinf(fAngle) + vTemp.z * cosf(fAngle);

}

void CPipeline::MyRotationY(_vec3 * pOut, _vec3 * pIn, _float fAngle)
{
	/*	c	0	-s	0
		0	1	0	0
		s	0	c	0
		0	0	0	1	*/

	_vec3	vTemp = *pIn;

	pOut->x = vTemp.x * cosf(fAngle) + vTemp.z * sinf(fAngle);
	pOut->y = vTemp.y;
	pOut->z = vTemp.x * -sinf(fAngle) + vTemp.z * cosf(fAngle);
}



void CPipeline::MyRotationZ(_vec3 * pOut, _vec3 * pIn, _float fAngle)
{
	/*	c	s	0	0
		-s	c	0	0
		0	0	1	0
		0	0	0	1	*/

	_vec3	vTemp = *pIn;

	pOut->x = vTemp.x * cosf(fAngle) + vTemp.y * -sinf(fAngle);
	pOut->y = vTemp.x * sinf(fAngle) + vTemp.y * cosf(fAngle);
	pOut->z = vTemp.z;

}

void Engine::CPipeline::MakeViewSpaceMatrix(_matrix* pMatrix, 
											const _vec3* pEye, 
											const _vec3* pAt, 
											const _vec3* pUp)
{
	D3DXMatrixIdentity(pMatrix);

	_vec3 vRight	= _vec3(1.f, 0.f, 0.f);
	_vec3 vUp		= _vec3(0.f, 1.f, 0.f);
	_vec3 vLook		= _vec3(0.f, 0.f, 1.f);
	_vec3 vPos		= _vec3(0.f, 0.f, 0.f);

	// look 벡터를 구해주자.
	vLook = *pAt - *pEye;
	D3DXVec3Normalize(&vLook, &vLook);

	// RIght벡터를 구해주자.
	D3DXVec3Cross(&vRight, pUp, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);
	
	// up벡터를 구해주자.
	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	vPos = *pEye;

	// 카메라 행렬의 월드를 구성
	MakeTransformMatrix(pMatrix, &vRight, &vUp, &vLook, &vPos);

	// 뷰스페이스 변환 행렬 구성
	D3DXMatrixInverse(pMatrix, NULL, pMatrix);
}

void Engine::CPipeline::MakePerspectiveMatrix(_matrix* pMatrix, const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	D3DXMatrixIdentity(pMatrix);

	pMatrix->_11 = (1.f / tanf(fFovY / 2.f)) / fAspect;
	pMatrix->_22 = 1.f / tanf(fFovY / 2.f);
	pMatrix->_33 = fFar / (fFar - fNear);
	pMatrix->_43 = (-fNear * fFar) / (fFar - fNear);

	pMatrix->_34 = 1.f;
	pMatrix->_44 = 0.f;
}

