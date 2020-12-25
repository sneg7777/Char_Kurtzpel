#include "Scene.h"
#include "Layer.h"
USING(Engine)

Engine::CScene::CScene(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
{
	Safe_AddRef(m_pGraphicDev);
}

Engine::CScene::~CScene()
{

}

HRESULT Engine::CScene::Ready_Scene(void)
{
	return S_OK;
}

Engine::_int Engine::CScene::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = 0;

	for (auto& iter : m_mapLayer)
	{
		iExit = iter.second->Update_Layer(fTimeDelta);

		if (iExit & 0x80000000)
		{
			MSG_BOX("Layer Problem");
			return iExit;
		}
	}

	return iExit;
}

Engine::_int Engine::CScene::LateUpdate_Scene(const _float& fTimeDelta)
{
	_int iExit = 0;

	for (auto& iter : m_mapLayer)
	{
		iExit = iter.second->Update_Layer(fTimeDelta);

		if (iExit & 0x80000000)
		{
			MSG_BOX("Layer Problem");
			return iExit;
		}
	}

	return iExit;
}

void Engine::CScene::Render_Scene(void)
{
	
}

void Engine::CScene::Free(void)
{
	for_each(m_mapLayer.begin(), m_mapLayer.end(), CDeleteMap());
	m_mapLayer.clear();

	Safe_Release(m_pGraphicDev);
}

Engine::CComponent* Engine::CScene::Get_Component(const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag, COMPONENTID eID)
{
	auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

	if (iter == m_mapLayer.end())
		return nullptr;

	return iter->second->Get_Component(pObjTag, pComponentTag, eID);
}


Engine::CGameObject* Engine::CScene::Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag)
{
	auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

	if (iter == m_mapLayer.end())
		return nullptr;

	auto	iter2 = find_if((*iter->second->Get_mapObject()).begin(), (*iter->second->Get_mapObject()).end(), CTag_Finder(pObjTag));

	if (iter2 == (*(*iter).second->Get_mapObject()).end())
		return nullptr;

	return iter2->second;
}