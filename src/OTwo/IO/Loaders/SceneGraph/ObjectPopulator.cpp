#include <OTwo/IO/Loaders/SceneGraph/ObjectPopulator.hpp>

ObjectPopulator::ObjectPopulator(Gx::Node *populator) :
    m_populator(populator)
{
}

ObjectPopulator ObjectPopulator::Decorate(Gx::Node *populator)
{
    return ObjectPopulator(populator);
}

