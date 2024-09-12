#include <OTwo/Decorators/SceneGraph/SceneDirectorDecorator.hpp>

SceneDirectorDecorator::SceneDirectorDecorator(Gx::SceneDirector& director) :
    m_director(&director)
{
}

SceneDirectorDecorator SceneDirectorDecorator::Decorate(Gx::SceneDirector& director)
{
    return SceneDirectorDecorator(director);
}
