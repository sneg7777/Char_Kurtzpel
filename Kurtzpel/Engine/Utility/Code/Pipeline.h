#ifndef Pipeline_h__
#define Pipeline_h__

#include "Engine_Define.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CPipeline : public CBase
{
private:
	explicit CPipeline(void);
	virtual ~CPipeline(void);

public:
	static void MakeTransformMatrix(_matrix* pMatrix, 
									const _vec3* pRight,
									const _vec3* pUp,
									const _vec3* pLook,
									const _vec3* pPos);
	
	static void MyRotationX(_vec3* pOut, _vec3* pIn, _float fAngle);
	static void MyRotationY(_vec3* pOut, _vec3* pIn, _float fAngle);
	static void MyRotationZ(_vec3* pOut, _vec3* pIn, _float fAngle);

	static void MakeViewSpaceMatrix(_matrix* pMatrix, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp);
	static void MakePerspectiveMatrix(_matrix* pMatrix, const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar);

};

END
#endif // PipeLine_h__
