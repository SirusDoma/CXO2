#include <Genode/IO/ResourceLoaderFactory.hpp>

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

namespace Gx
{
    template<typename R, class L>
    void ResourceLoaderFactory::Register()
    {
        static_assert(std::is_base_of_v<ResourceLoader<R>, L>, "Parameter must be a Gx::ResourceLoader.");
        Remove<R>();

        auto factory = std::make_unique<LoaderFactory<R>>();
        factory->Create = [] { return std::make_unique<L>(); };

        m_loaders[typeid(R)] = std::move(factory);
    }

    template<typename R>
    void ResourceLoaderFactory::Register(std::function<std::unique_ptr<ResourceLoader<R>>()> loader)
    {
        Remove<R>();

        auto factory = std::make_unique<LoaderFactory<R>>();
        factory->Create = loader;

        m_loaders[typeid(R)] = std::move(factory);
    }

    template<typename R>
    bool ResourceLoaderFactory::Remove()
    {
        return m_loaders.erase(typeid(R)) != 0;
    }

    template<typename R>
    std::unique_ptr<ResourceLoader<R>> ResourceLoaderFactory::GetLoader()
    {
        EnsureDefaultLoadersRegistered();

        const auto it = m_loaders.find(typeid(R));
        if (it == m_loaders.end())
            return nullptr;

        auto factory = static_cast<LoaderFactory<R>*>(it->second.get());
        return factory->Create();
    }
}