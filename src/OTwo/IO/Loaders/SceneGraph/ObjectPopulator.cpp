#include <OTwo/IO/Loaders/SceneGraph/ObjectPopulator.hpp>

ObjectPopulator::ObjectPopulator(Gx::Node *populator, const bool importOnly) :
    m_populator(populator),
    m_importOnly(importOnly)
{
}

ObjectPopulator ObjectPopulator::Decorate(State *populator, const bool importOnly)
{
    return ObjectPopulator(populator, importOnly);
}

ObjectPopulator ObjectPopulator::Decorate(Gx::Node *populator)
{
    return ObjectPopulator(populator, false);
}

const std::string &ObjectPopulator::GetName() const
{
    return m_populator->GetName();
}

