#include <Genode/IO/ResourceLoaderFactory.hpp>

#include <Genode/IO/Loaders/TextureLoader.hpp>
#include <Genode/IO/Loaders/FontLoader.hpp>
#include <Genode/IO/Loaders/SoundBufferLoader.hpp>

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
