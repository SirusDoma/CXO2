#ifndef GENODE_SCENEGRAPH_INPUTABLE_CONTAINER_HPP
#define GENODE_SCENEGRAPH_INPUTABLE_CONTAINER_HPP

#include <SFML/Window/Event.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Inputable.hpp>

namespace Gx
{
    class InputableContainer : public virtual Node, public Inputable
    {
    public:
        virtual ~InputableContainer();

    protected:
        InputableContainer();
        virtual bool Input(sf::Event ev);
    };
}

#endif
