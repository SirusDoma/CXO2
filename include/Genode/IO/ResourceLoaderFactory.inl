#pragma once

#include <Genode/IO/ResourceLoaderFactory.hpp>
#include <Genode/Graphics/Font.hpp>
#include <Genode/System/Context.hpp>
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

        auto factory = std::make_unique<LoaderBuilder<R>>();
        factory->Instantiate = []
        {
            std::unique_ptr<L> loader;
            if constexpr (!std::is_default_constructible_v<L>)
            {
                if (m_context)
                    loader = m_context->Create<L>();
                else
                    throw Exception(std::string(typeid(L).name()) + " is not constructible without application context");
            }
            else
                loader = std::make_unique<L>();

            loader->SetResourceInstantiator(std::function{[] (const ResourceContext& ctx)
            {
                if constexpr (!std::is_default_constructible_v<R>)
                {
                    if (m_context)
                        return m_context->Create<R>();

                    throw Exception(std::string(typeid(R).name()) + " loader is not constructible without application context");
                }
                else
                    return std::make_unique<R>();
            }});

            return loader;
        };

        m_loaders[typeid(R)] = std::move(factory);
    }

    template<typename R>
    void ResourceLoaderFactory::Register(std::function<std::unique_ptr<ResourceLoader<R>>()> builder)
    {
        Remove<R>();

        auto factory = std::make_unique<LoaderBuilder<R>>();
        factory->Instantiate = builder;

        m_loaders[typeid(R)] = std::move(factory);
    }

    template<typename B, typename R>
    void ResourceLoaderFactory::RegisterDerived()
    {
        RegisterDerived<B, R>(std::function<std::unique_ptr<R>(const ResourceContext&)>{[] (const ResourceContext&)
        {
            if constexpr (!std::is_default_constructible_v<R>)
            {
                if (m_context)
                    return m_context->Create<R>();

                throw Exception(std::string(typeid(R).name()) + " loader is not constructible without application context");
            }
            else
                return std::make_unique<R>();
        }});
    }

    template<typename B, typename R, typename ... Args>
    void ResourceLoaderFactory::RegisterDerived(const std::function<std::unique_ptr<R>(const ResourceContext&, Args...)>& instantiator)
    {
        static_assert(std::is_base_of_v<B, R>, "Parameter R must be a B");

        Remove<R>();

        auto factory = std::make_unique<LoaderBuilder<B>>();
        factory->Instantiate = [=]
        {
            const auto it = m_loaders.find(typeid(B));
            if (it == m_loaders.end())
                throw Exception("Base loader is not registered");

            auto loader = static_cast<LoaderBuilder<B>*>(it->second.get())->Instantiate();
            loader->SetResourceInstantiator(instantiator);

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

        auto factory = static_cast<LoaderBuilder<R>*>(it->second.get());
        return factory->Instantiate();
    }
}