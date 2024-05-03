#include <OTwo/IO/Loaders/SceneGraph/ObjectPopulator.hpp>

ObjectPopulator::ObjectPopulator(Gx::Node *populator, bool importOnly) :
    m_populator(populator),
    m_importOnly(importOnly)
{
}

ObjectPopulator ObjectPopulator::Decorate(Gx::Node *populator, bool importOnly)
{
    return { populator, importOnly };
}

