#include <Genode/IO/ResourceLoaderFactory.hpp>
#include <Genode/IO/Resource.hpp>

#include <Genode/Graphics/Font.hpp>
#include <Genode/System/Exception.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace Gx
{
    template<typename R, typename L>
    void ResourceLoaderFactory::Register()
    {
        static_assert(std::is_base_of_v<ResourceLoader<R>, L>, "Parameter L must be a Gx::ResourceLoader<R>");
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

    template<typename B, typename R>
    void ResourceLoaderFactory::RegisterDerived()
    {
        RegisterDerived<B, R>(std::function{[] { return std::make_unique<R>(); }});
    }

    template<typename B, typename R, typename ... Args>
    void ResourceLoaderFactory::RegisterDerived(const std::function<std::unique_ptr<R>(Args...)>& creator)
    {
        static_assert(std::is_base_of_v<B, R>, "Parameter R must be a B");
        static_assert(std::is_default_constructible_v<R>, "Parameter R must be default constructible");

        Remove<R>();

        auto factory = std::make_unique<LoaderFactory<B>>();
        factory->Create = [=]
        {
            const auto it = m_loaders.find(typeid(B));
            if (it == m_loaders.end())
                throw Exception("Base loader is not registered");

            auto loader = static_cast<LoaderFactory<B>*>(it->second.get())->Create();
            loader->SetResourceCreator(creator);

            return loader;
        };

        m_loaders[typeid(R)] = std::move(factory);
    }

    template<typename R>
    bool ResourceLoaderFactory::Remove()
    {
        return m_loaders.erase(typeid(R)) != 0;
    }

    template<typename R>
    std::unique_ptr<ResourceLoader<R>> ResourceLoaderFactory::CreateLoader()
    {
        EnsureDefaultLoadersRegistered();

        const auto it = m_loaders.find(typeid(R));
        if (it == m_loaders.end())
            return nullptr;

        auto factory = static_cast<LoaderFactory<R>*>(it->second.get());
        return factory->Create();
    }
}