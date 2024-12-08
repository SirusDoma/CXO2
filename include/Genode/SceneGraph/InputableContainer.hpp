#pragma once

#include <SFML/Window/Event.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Inputable.hpp>

namespace Gx
{
    class InputableContainer : public virtual Node, public virtual Inputable
    {
    protected:
        InputableContainer() = default;
        bool Input(const sf::Event& ev) override;
    };
}
