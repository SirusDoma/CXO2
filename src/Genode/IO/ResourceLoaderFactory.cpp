#include <Genode/IO/ResourceLoaderFactory.hpp>

#include <Genode/IO/Loaders/TextureLoader.hpp>
#include <Genode/IO/Loaders/FontLoader.hpp>
#include <Genode/IO/Loaders/SoundBufferLoader.hpp>

void Gx::ResourceLoaderFactory::EnsureDefaultLoadersRegistered()
{
    static bool registered = false;
    if (!registered)
    {
        Gx::ResourceLoaderFactory::Register<sf::Texture,     Gx::TextureLoader>();
        Gx::ResourceLoaderFactory::Register<Font,            Gx::FontLoader>();
        Gx::ResourceLoaderFactory::Register<sf::SoundBuffer, Gx::SoundBufferLoader>();

        registered = true;
    }
}
