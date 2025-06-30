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
        auto it = m_loaders.find(typeid(sf::Texture));
        if (it == m_loaders.end() || it->second.empty())
            Register<sf::Texture, TextureLoader>();

        it = m_loaders.find(typeid(Font));
        if (it == m_loaders.end() || it->second.empty())
            Register<Font, FontLoader>();

        it = m_loaders.find(typeid(sf::SoundBuffer));
        if (it == m_loaders.end() || it->second.empty())
            Register<sf::SoundBuffer, SoundBufferLoader>();

        registered = true;
    }
}
