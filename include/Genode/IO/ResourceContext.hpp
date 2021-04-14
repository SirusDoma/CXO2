#ifndef GENODE_IO_RESOURCE_CONTEXT_HPP
#define GENODE_IO_RESOURCE_CONTEXT_HPP

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <memory>

namespace Gx
{
    class ResourceManager;
    class ResourceContext
    {
    // TODO: Multiple resources for same type of resource, use vector for each? or use resource contianer / resource manager?
    // Resources must guaranteed loaded before hand
    public:
        std::string             Name;
        const sf::Texture*      Texture;
        const sf::Image*        Image;
        const sf::Font*         Font;
        const sf::SoundBuffer*  SoundBuffer;
        const sf::Shader*       Shader;
        const ResourceMetadata* Metadata;
        ResourceManager*        Resources;
    };
}

#endif