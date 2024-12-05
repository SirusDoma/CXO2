#include <Genode/IO/ResourceLoaderFactory.hpp>

#include <Genode/IO/Loaders/TextureLoader.hpp>
#include <Genode/IO/Loaders/FontLoader.hpp>
#include <Genode/IO/Loaders/SoundBufferLoader.hpp>

void Gx::ResourceLoaderFactory::BindContext(const Context& context)
{
    m_context = &context;
}

void Gx::ResourceLoaderFactory::EnsureDefaultLoadersRegistered()
{
    static bool registered = false;
    if (!registered)
    {
        if (m_loaders.find(typeid(sf::Texture)) == m_loaders.end())
            Register<sf::Texture, TextureLoader>();

        if (m_loaders.find(typeid(Font)) == m_loaders.end())
            Register<Font, FontLoader>();

        if (m_loaders.find(typeid(sf::SoundBuffer)) == m_loaders.end())
            Register<sf::SoundBuffer, SoundBufferLoader>();

        registered = true;
    }
}
