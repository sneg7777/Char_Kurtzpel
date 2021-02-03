#ifndef EffectRcTex_h__
#define EffectRcTex_h__

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
class CEffectRcTex : public CUnit {
protected:
	explicit CEffectRcTex(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffectRcTex(void);

public:
	virtual HRESULT Ready_Object(void) override;
	virtual _int Update_Object(const _float& fTimeDelta) override;
	virtual void Render_Object(void) override;
	//
	HRESULT		SetUp_ConstantTable(LPD3DXEFFECT& pEffect);
	virtual void Collision(CSphereCollider* _mySphere, CUnit* _col, CSphereCollider* _colSphere, const _float& fTimeDelta);
	//_pos : 위치
	//_scale : 크기(float)
	//_tchar : 리소스
	//_uOffset : 가로 이미지 수
	//_vOffset : 세로 이미지 수
	//_speed : 20.f 이미지 돌리는 스피드
	//_rotateX,Y,Z = 0.f 회전
	CEffectRcTex*	Set_Effect(bool _bill, _vec3 _pos, float _scale, const _tchar* _pResourcesTag, int _uOffset, int _vOffset, float _speed = 20.f
		, float _rotateX = 0.f, float _rotateY = 0.f, float _rotateZ = 0.f);
	
	//
private:
	HRESULT		Add_Component(void);

public:
	static CEffectRcTex* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free(void) override;

	float		m_EffectTime = 0;
	Engine::CRcTex* m_pBufferCom = nullptr;
	int m_uOffset = 0;
	int m_vOffset = 0;
	float m_EffectSpeed = 10.f;
	bool m_bill;

};

END
#endif
