#include <CXO2/Decorators/SceneGraph/SceneDirectorDecorator.hpp>

namespace Cx
{
    SceneDirectorDecorator::SceneDirectorDecorator(Gx::SceneDirector& director) :
        m_director(&director)
    {
    }

    SceneDirectorDecorator SceneDirectorDecorator::Decorate(Gx::SceneDirector& director)
    {
        return SceneDirectorDecorator(director);
    }
}
