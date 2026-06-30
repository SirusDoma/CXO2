#include <CXO2/IO/Loaders/SceneGraph/ObjectContainer.hpp>

namespace Cx
{
    ObjectContainer::ObjectContainer(Gx::Node* container, const bool importOnly) :
        m_container(container),
        m_importOnly(importOnly)
    {
    }

    ObjectContainer ObjectContainer::Decorate(State* container, const bool importOnly)
    {
        return ObjectContainer(container, importOnly);
    }

    ObjectContainer ObjectContainer::Decorate(Gx::Node* container)
    {
        return ObjectContainer(container, false);
    }

    const std::string& ObjectContainer::GetName() const
    {
        return m_container->GetName();
    }

    const Gx::Node& ObjectContainer::GetNode() const
    {
        return *m_container;
    }
}
