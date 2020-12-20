#include "Component.h"
USING(Engine)

Engine::CComponent::CComponent(void)
	: m_bClone(false)
{

}

Engine::CComponent::CComponent(const CComponent& rhs)
	:m_bClone(true)
{

}

Engine::CComponent::~CComponent(void)
{

}

Engine::_int Engine::CComponent::Update_Component(const _float& fTimeDelta)
{
	return 0;
}
