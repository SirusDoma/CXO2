#ifndef GENODE_IO_DESERIALIZER_FACRORY_HPP
#define GENODE_IO_DESERIALIZER_FACRORY_HPP

#include <typeindex>
#include <typeinfo>
#include <functional>
#include <unordered_map>
#include <memory>

namespace Gx
{
    template<typename T>
    class ResourceLoader;
    class ResourceLoaderFactory
    {
    public:
        template<typename R, typename L>
        static void Register();

        template<typename R>
        static void Register(std::function<std::unique_ptr<ResourceLoader<R>>()> factory);

        template<typename R>
        static bool Remove();

        template<typename R>
        static std::unique_ptr<ResourceLoader<R>> GetLoader();

    private:
        static void EnsureDefaultLoadersRegistered();
        struct BaseLoaderFactory {};

        template<typename R>
        struct LoaderFactory : BaseLoaderFactory
        {
            std::function<std::unique_ptr<ResourceLoader<R>>()> Create;
        };
        using LoaderMap = std::unordered_map<std::type_index, std::unique_ptr<BaseLoaderFactory>>;

        inline static LoaderMap m_loaders;
    };
}

#include <Genode/IO/ResourceLoaderFactory.inl>
#endif
