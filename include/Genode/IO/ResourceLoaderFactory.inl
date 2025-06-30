#pragma once

#include <Genode/IO/ResourceLoaderFactory.hpp>
#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Graphics/Font.hpp>
#include <Genode/System/Context.hpp>
#include <Genode/System/Exception.hpp>
#include <Genode/Utilities/StringHelper.hpp>

#include <string>

namespace Gx
{
    template<typename R, typename L>
    void ResourceLoaderFactory::Register()
    {
        Register<R, L>(StringHelper::GetTypeName<R>(false));
    }

    template<typename R>
    void ResourceLoaderFactory::Register(std::function<std::unique_ptr<ResourceLoader<R>>()> builder)
    {
        Register<R>(StringHelper::GetTypeName<R>(false), builder);
    }

    template<typename B, typename R>
    void ResourceLoaderFactory::RegisterDerived()
    {
        RegisterDerived<B, R>(StringHelper::GetTypeName<R>(false));
    }

    template<typename B, typename R, typename ... Args>
    void ResourceLoaderFactory::RegisterDerived(const std::function<std::unique_ptr<R>(const ResourceContext&, Args...)>& instantiator)
    {
        RegisterDerived<B, R>(StringHelper::GetTypeName<R>(false), instantiator);
    }

    template<typename R, typename L, typename U>
    void ResourceLoaderFactory::Register(const type_identity_t<U>& id)
    {
        static_assert(std::is_base_of_v<ResourceLoader<R>, L>, "Parameter L must be a Gx::ResourceLoader<R>");
        
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

        auto& loaders = m_loaders[typeid(R)];
        loaders[LoaderKey(id)] = std::move(factory);
    }

    template<typename R, typename U>
    void ResourceLoaderFactory::Register(const type_identity_t<U>& id, std::function<std::unique_ptr<ResourceLoader<R>>()> builder)
    {
        auto factory = std::make_unique<LoaderBuilder<R>>();
        factory->Instantiate = builder;

        auto& loaders = m_loaders[typeid(R)];
        loaders[LoaderKey(id)] = std::move(factory);
    }

    template<typename B, typename R, typename U>
    void ResourceLoaderFactory::RegisterDerived(const type_identity_t<U>& id)
    {
        RegisterDerived<B, R>(id, std::function<std::unique_ptr<R>(const ResourceContext&)>{[] (const ResourceContext&)
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

    template<typename B, typename R, typename U, typename ... Args>
    void ResourceLoaderFactory::RegisterDerived(const type_identity_t<U>& id, const std::function<std::unique_ptr<R>(const ResourceContext&, Args...)>& instantiator)
    {
        static_assert(std::is_base_of_v<B, R>, "Parameter R must be a B");

        auto factory = std::make_unique<LoaderBuilder<B>>();
        factory->Instantiate = [=]
        {
            const auto it = m_loaders.find(typeid(B));
            if (it == m_loaders.end() || it->second.empty())
                throw Exception("Base loader is not registered");

            auto baseFactory = static_cast<LoaderBuilder<B>*>(it->second.begin()->second.get());
            auto loader = baseFactory->Instantiate();
            loader->SetResourceInstantiator(instantiator);

            return loader;
        };

        auto& loaders = m_loaders[typeid(R)];
        loaders[LoaderKey(id)] = std::move(factory);
    }

    template<typename R>
    bool ResourceLoaderFactory::Remove()
    {
        return m_loaders.erase(typeid(R)) != 0;
    }

    template<typename R, typename U>
    bool ResourceLoaderFactory::Remove(const type_identity_t<U>& id)
    {
        const auto it = m_loaders.find(typeid(R));
        if (it == m_loaders.end())
            return false;

        auto& loaders = it->second;
        const bool removed = loaders.erase(LoaderKey(id)) != 0;

        if (loaders.empty())
            m_loaders.erase(it);
            
        return removed;
    }

    template<typename R>
    std::unique_ptr<ResourceLoader<R>> ResourceLoaderFactory::CreateLoader()
    {
        EnsureDefaultLoadersRegistered();

        const auto it = m_loaders.find(typeid(R));
        if (it == m_loaders.end())
            return nullptr;

        const auto& loaders = it->second;
        if (loaders.empty())
            return nullptr;
        
        if (loaders.size() == 1)
        {
            auto factory = static_cast<LoaderBuilder<R>*>(loaders.begin()->second.get());
            return factory->Instantiate();
        }
        
        throw Exception("Multiple loaders registered for type " + StringHelper::GetTypeName<R>(false) + ". " +
                        "Use CreateLoader<R, U>(id) to specify which one to use.");
    }

    template<typename R, typename U>
    std::unique_ptr<ResourceLoader<R>> ResourceLoaderFactory::CreateLoader(const type_identity_t<U>& id)
    {
        EnsureDefaultLoadersRegistered();

        const auto it = m_loaders.find(typeid(R));
        if (it == m_loaders.end())
            return nullptr;

        const auto& loaders = it->second;
        if (const auto loader = loaders.find(LoaderKey(id)); loader != loaders.end())
        {
            auto factory = static_cast<LoaderBuilder<R>*>(loader->second.get());
            return factory->Instantiate();
        }

        return nullptr;
    }
}
