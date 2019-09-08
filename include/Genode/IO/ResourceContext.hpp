#ifndef GENODE_RESOURCE_CONTEXT_HPP
#define GENODE_RESOURCE_CONTEXT_HPP

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

#include <memory>

namespace Gx
{
    struct ResourceContext
    {
        std::shared_ptr<sf::Texture> Texture;
        std::shared_ptr<sf::Font>    Font;
    };
}

#endif