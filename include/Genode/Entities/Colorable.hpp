#pragma once

#include <SFML/Graphics/Color.hpp>

namespace Gx
{
    class Colorable
    {
    public:
        virtual void SetColor(const sf::Color& color) = 0;
        virtual const sf::Color& GetColor() const = 0;

    protected:
        virtual ~Colorable() = default;
    };
}
