#ifndef GENODE_IO_DESERIALIZER_FACRORY_HPP
#define GENODE_IO_DESERIALIZER_FACRORY_HPP

#include <Genode/IO/ResourceLoader.hpp>

#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <functional>

namespace Gx
{
    using LoaderMap = std::unordered_map<std::type_index, std::function<priv::BaseLoader*()>>;
    class ResourceLoaderFactory
    {
    public:
        template<typename R, typename D>
        static void Register();

        template<typename R>
        static bool Remove();

        template<typename R>
        static ResourceLoader<R>* GetLoader();

    private:
        inline static LoaderMap m_loaders;
    };
}

#include <Genode/IO/ResourceLoaderFactory.inl>
#endif
