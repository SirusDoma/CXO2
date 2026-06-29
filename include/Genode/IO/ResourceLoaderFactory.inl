#pragma once

#include <Genode/IO/ResourceLoaderFactory.hpp>
#include <Genode/IO/Json.hpp>
#include <Genode/Graphics/Font.hpp>
#include <Genode/System/Context.hpp>
#include <Genode/System/Exception.hpp>
#include <Genode/Utilities/StringHelper.hpp>

#include <string>

namespace Gx
{
    template<typename B, typename R>
    class ResourceLoaderFactory::AdaptorLoader : public ResourceLoader<B>
    {
    public:
        AdaptorLoader() : m_loader(ResourceLoaderFactory::CreateLoader<R>()) {}

        template<typename U = std::string>
        explicit AdaptorLoader(const type_identity_t<U>& id) : m_loader(ResourceLoaderFactory::CreateLoader<R, U>(id)) {}

        ResourceLoader<R>* GetLoader()
        {
            return m_loader.get();
        }

        ResourcePtr<B> LoadFromFile(const std::string& fileName, const ResourceContext& ctx) const override
        {
            if (!m_loader)
                return nullptr;
            
            auto resource = m_loader->LoadFromFile(fileName, ctx);
            if (!resource)
                return nullptr;
                
            return Cast<B>(std::move(resource));
        }
        
        ResourcePtr<B> LoadFromMemory(void* data, std::size_t size, const ResourceContext& ctx) const override
        {
            if (!m_loader)
                return nullptr;

            auto resource = m_loader->LoadFromMemory(data, size, ctx);
            if (!resource)
                return nullptr;
                
            return Cast<B>(std::move(resource));
        }
        
        ResourcePtr<B> LoadFromStream(sf::InputStream& stream, const ResourceContext& ctx) const override
        {
            if (!m_loader)
                return nullptr;

            
            auto resource = m_loader->LoadFromStream(stream, ctx);
            if (!resource)
                return nullptr;
                
            return Cast<B>(std::move(resource));
        }

        ResourcePtr<B> LoadFromJson(const Json& json, const ResourceContext& ctx) const override
        {
            if (!m_loader)
                return nullptr;

            auto resource = m_loader->LoadFromJson(json, ctx);
            if (!resource)
                return nullptr;

            return Cast<B>(std::move(resource));
        }
        
    private:
        std::unique_ptr<ResourceLoader<R>> m_loader;
    };

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

    template<typename B, typename R, typename L>
    void ResourceLoaderFactory::Register()
    {
        Register<B, R, L>(StringHelper::GetTypeName<R>(false));
    }

    template<typename B, typename R>
    void ResourceLoaderFactory::Register(std::function<std::unique_ptr<ResourceLoader<R>>()> builder)
    {
        Register<B, R>(StringHelper::GetTypeName<R>(false), builder);
    }

    template<typename R, typename L>
    std::unique_ptr<ResourceLoaderFactory::LoaderBuilder<R>> ResourceLoaderFactory::CreateLoaderBuilder()
    {
        static_assert(std::is_base_of_v<ResourceLoader<R>, L>, "Parameter L must be a Gx::ResourceLoader<R>");
        
        auto factory = std::make_unique<LoaderBuilder<R>>();
        factory->Instantiate = []
        {
            std::unique_ptr<L> loader;
            if constexpr (!std::is_default_constructible_v<L>)
            {
                if (m_context)
                    loader = m_context->Instantiate<L>();
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
                        return m_context->Instantiate<R>();

                    throw Exception(std::string(typeid(R).name()) + " loader is not constructible without application context");
                }
                else
                    return std::make_unique<R>();
            }});

            return loader;
        };
        
        return factory;
    }

    template<typename R, typename L, typename U>
    void ResourceLoaderFactory::Register(const type_identity_t<U>& id)
    {
        auto factory = CreateLoaderBuilder<R, L>();
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

    template<typename B, typename R, typename L, typename U>
    void ResourceLoaderFactory::Register(const type_identity_t<U>& id)
    {
        static_assert(std::is_base_of_v<B, R>, "Parameter R must be a B");
        static_assert(std::is_base_of_v<ResourceLoader<R>, L>, "Parameter L must be a Gx::ResourceLoader<R>");

        auto factory = CreateLoaderBuilder<R, L>();
        auto& loaders = m_loaders[typeid(R)];
        loaders[LoaderKey(id)] = std::move(factory);

        auto baseFactory = std::make_unique<LoaderBuilder<B>>();
        baseFactory->Instantiate = [=]()
        {
            return std::make_unique<AdaptorLoader<B, R>>(id);
        };

        auto& baseLoaders = m_loaders[typeid(B)];
        baseLoaders[LoaderKey(id)] = std::move(baseFactory);
    }

    template<typename B, typename R, typename U>
    void ResourceLoaderFactory::Register(const type_identity_t<U>& id, std::function<std::unique_ptr<ResourceLoader<R>>()> builder)
    {
        static_assert(std::is_base_of_v<B, R>, "Parameter R must be a B");

        auto factory = std::make_unique<LoaderBuilder<R>>();
        factory->Instantiate = builder;
        auto& loaders = m_loaders[typeid(R)];
        loaders[LoaderKey(id)] = std::move(factory);

        auto baseFactory = std::make_unique<LoaderBuilder<B>>();
        baseFactory->Instantiate = [=]()
        {
            return std::make_unique<AdaptorLoader<B, R>>(id);
        };

        auto& baseLoaders = m_loaders[typeid(B)];
        baseLoaders[LoaderKey(id)] = std::move(baseFactory);
    }

    template<typename B, typename R>
    void ResourceLoaderFactory::Reuse()
    {
        Reuse<B, R>(StringHelper::GetTypeName<R>(false));
    }

    template<typename B, typename R, typename ... Args>
    void ResourceLoaderFactory::Reuse(const std::function<std::unique_ptr<R>(const ResourceContext&, Args...)>& instantiator)
    {
        Reuse<B, R>(StringHelper::GetTypeName<R>(false), instantiator);
    }

    template<typename B, typename R, typename U>
    void ResourceLoaderFactory::Reuse(const type_identity_t<U>& id)
    {
        Reuse<B, R, U>(id, std::function<std::unique_ptr<R>(const ResourceContext&)>{[] (const ResourceContext&)
        {
            if constexpr (!std::is_default_constructible_v<R>)
            {
                if (m_context)
                    return m_context->Instantiate<R>();

                throw Exception(std::string(typeid(R).name()) + " loader is not constructible without application context");
            }
            else
                return std::make_unique<R>();
        }});
    }

    template<typename B, typename R, typename U, typename ... Args>
    void ResourceLoaderFactory::Reuse(const type_identity_t<U>& id, const std::function<std::unique_ptr<R>(const ResourceContext&, Args...)>& instantiator)
    {
        static_assert(std::is_base_of_v<B, R>, "Parameter R must be a B");

        auto& loaders = m_loaders[typeid(R)];
        loaders[LoaderKey(id)] = std::make_unique<LoaderBuilder<R>>
        (
            [=]
            {
                auto loader = std::make_unique<AdaptorLoader<R, B>>(id);
                loader->GetLoader()->SetResourceInstantiator(instantiator);

                return loader;
            }
        );

        auto& baseLoaders = m_loaders[typeid(B)];
        if (const auto it = baseLoaders.find(LoaderKey(StringHelper::GetTypeName<B>(false))); it != baseLoaders.end() && baseLoaders.find(LoaderKey(id)) == baseLoaders.end())
        {
            if (auto builder = dynamic_cast<LoaderBuilder<B>*>(it->second.get()); builder)
            {
                baseLoaders[LoaderKey(id)] = std::make_unique<LoaderBuilder<B>>
                (
                    [=]
                    {
                        auto loader = builder->Instantiate();
                        loader->SetResourceInstantiator(instantiator);
                        return loader;
                    }
                );
            }
        }
    }

    template<typename S, typename B, typename R>
    void ResourceLoaderFactory::Reuse()
    {
        Reuse<S, B, R>(StringHelper::GetTypeName<R>(false));
    }

    template<typename S, typename B, typename R, typename U>
    void ResourceLoaderFactory::Reuse(const type_identity_t<U>& id)
    {
        Reuse<S, B, R>(id, std::function{[] (const ResourceContext& _)
        {
            if constexpr (!std::is_default_constructible_v<R>)
            {
                if (m_context)
                    return m_context->Instantiate<R>();

                throw Exception(std::string(typeid(R).name()) + " is not constructible without application context");
            }
            else
                return std::make_unique<R>();
        }});
    }

    template<typename S, typename B, typename R, typename ... Args>
    void ResourceLoaderFactory::Reuse(const std::function<std::unique_ptr<R>(const ResourceContext&, Args...)>& instantiator)
    {
        Reuse<S, B, R>(StringHelper::GetTypeName<R>(false), instantiator);
    }

    template<typename S, typename B, typename R, typename U, typename ... Args>
    void ResourceLoaderFactory::Reuse(const type_identity_t<U>& id, const std::function<std::unique_ptr<R>(const ResourceContext&, Args...)>& instantiator)
    {
        static_assert(std::is_base_of_v<S, B>, "Parameter B must be a S");

        Reuse<B, R, U>(id, instantiator);

        auto& rootLoaders = m_loaders[typeid(S)];
        if (const auto it = rootLoaders.find(LoaderKey(StringHelper::GetTypeName<B>(false))); it != rootLoaders.end() && rootLoaders.find(LoaderKey(id)) == rootLoaders.end())
        {
            if (auto builder = dynamic_cast<LoaderBuilder<S>*>(it->second.get()); builder)
            {
                rootLoaders[LoaderKey(id)] = std::make_unique<LoaderBuilder<S>>
                (
                    [=]
                    {
                        auto loader = builder->Instantiate();
                        if (auto adaptor = dynamic_cast<AdaptorLoader<S, B>*>(loader.get()); adaptor)
                            adaptor->GetLoader()->SetResourceInstantiator(instantiator);

                        loader->SetResourceInstantiator(instantiator);
                        return loader;
                    }
                );
            }
        }
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

        auto loader = loaders.begin();
        if (loaders.size() > 1)
            loader = loaders.find(LoaderKey(StringHelper::GetTypeName<R>(false)));

        if (loader != loaders.end())
        {
            if (auto factory = dynamic_cast<LoaderBuilder<R>*>(loader->second.get());factory)
                return factory->Instantiate();
        }

        return nullptr;
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
