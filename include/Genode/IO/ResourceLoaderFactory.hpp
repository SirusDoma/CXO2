#pragma once

#include <typeindex>
#include <typeinfo>
#include <functional>
#include <unordered_map>
#include <memory>

namespace Gx
{
    template<typename T>
    class ResourceLoader;
    class ResourceContext;
    class Context;
    class ResourceLoaderFactory final
    {
    public:
        ResourceLoaderFactory() = delete;
        ~ResourceLoaderFactory() = delete;

        static const Context* GetApplicationContext();
        static void SetApplicationContext(const Context& context);

        template<typename R, typename L>
        static void Register();

        template<typename R>
        static void Register(std::function<std::unique_ptr<ResourceLoader<R>>()> builder);

        template<typename B, typename R>
        static void RegisterDerived();

        template<typename B, typename R, typename ... Args>
        static void RegisterDerived(const std::function<std::unique_ptr<R>(const ResourceContext&, Args...)>& creator);

        template<typename R>
        static bool Remove();

        template<typename R>
        static std::unique_ptr<ResourceLoader<R>> CreateLoader();

    private:
        static void EnsureDefaultLoadersRegistered();
        struct BaseLoaderFactory {};

        template<typename R>
        struct LoaderFactory : BaseLoaderFactory
        {
            std::function<std::unique_ptr<ResourceLoader<R>>()> Create;
        };
        using LoaderMap = std::unordered_map<std::type_index, std::unique_ptr<BaseLoaderFactory>>;

        inline static const Context* m_context;
        inline static LoaderMap      m_loaders;
    };
}

#include <Genode/IO/ResourceLoaderFactory.inl>
