#include <Genode/IO/Loaders/TextureLoader.hpp>
#include <Genode/IO/Loaders/FontLoader.hpp>
#include <Genode/IO/Loaders/SoundBufferLoader.hpp>

namespace
{
    void EnsureDefaultDeserializersRegistered()
    {
        static bool registered = false;
        if (!registered)
        {
            Gx::ResourceLoaderFactory::Register<sf::Texture,     Gx::priv::TextureLoader>();
            Gx::ResourceLoaderFactory::Register<sf::Font,        Gx::priv::FontLoader>();
            Gx::ResourceLoaderFactory::Register<sf::SoundBuffer, Gx::priv::SoundBufferLoader>();

            registered = true;
        }
    }
}

namespace Gx
{
    template<typename R, typename D>
    inline void ResourceLoaderFactory::Register()
    {
        Remove<R>();

        m_loaders[typeid(R)] = [] { return new D(); };
    }

    template<typename R>
    inline bool ResourceLoaderFactory::Remove()
    {
        return m_loaders.erase(typeid(R)) != 0;
    }
    
    template<typename R>
    inline ResourceLoader<R>* ResourceLoaderFactory::GetLoader()
    {
        EnsureDefaultDeserializersRegistered();

        auto iterator = m_loaders.find(typeid(R));
        if (iterator != m_loaders.end())
            return static_cast<ResourceLoader<R>*>(iterator->second());

        return nullptr;
    }
}