#include <Genode/IO/ResourceLoaderFactory.hpp>

#include <Genode/IO/Loaders/TextureLoader.hpp>
#include <Genode/IO/Loaders/FontLoader.hpp>
#include <Genode/IO/Loaders/SoundBufferLoader.hpp>

const Gx::Context* Gx::ResourceLoaderFactory::GetApplicationContext()
{
    return m_context;
}

void Gx::ResourceLoaderFactory::SetApplicationContext(const Context& context)
{
    m_context = &context;
}

void Gx::ResourceLoaderFactory::EnsureDefaultLoadersRegistered()
{
    static bool registered = false;
    if (!registered)
    {
        Register<sf::Texture,     TextureLoader>();
        Register<Font,            FontLoader>();
        Register<sf::SoundBuffer, SoundBufferLoader>();

        registered = true;
    }
}
