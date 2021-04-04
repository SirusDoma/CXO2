#ifndef GENODE_IO_RESOURCE_CONTEXT_HPP
#define GENODE_IO_RESOURCE_CONTEXT_HPP

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

#include <memory>

namespace Gx
{
    class ResourceManager;
    struct ResourceContext
    {
        std::string            Name;
        const sf::Texture*     Texture;
        const sf::Font*        Font;
        const sf::SoundBuffer* SoundBuffer;
        ResourceManager*       Resources;
    };
}

#endif