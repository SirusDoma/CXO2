#pragma once

#include <Genode/IO/Resource.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <SFML/System/InputStream.hpp>

#include <typeindex>

namespace Gx
{
    template<typename T>
    class ResourceLoader
    {
    public:
        template <typename R>
        using ResourceInstantiator = std::function<std::unique_ptr<R>(const ResourceContext&)>;

        ResourceLoader() = default;
        virtual ~ResourceLoader() = default;

        virtual bool IsStreaming() const { return false; }

        std::type_index GetResourceType() const { return m_type; }

        virtual ResourcePtr<T> LoadFromFile(const std::string& fileName, const ResourceContext& ctx) const = 0;
        virtual ResourcePtr<T> LoadFromMemory(void* data, std::size_t size, const ResourceContext& ctx) const = 0;
        virtual ResourcePtr<T> LoadFromStream(sf::InputStream& stream, const ResourceContext& ctx) const = 0;

    protected:
        std::unique_ptr<T> Instantiate(const ResourceContext& context) const
        {
            if (m_instantiator)
                return m_instantiator(context);

            if constexpr (std::is_default_constructible_v<T>)
                return std::make_unique<T>();

            throw ResourceLoadException("Resource cannot be constructed without instantiator");
        }

        void SetResourceInstantiator(const ResourceInstantiator<T>& instantiator) { m_instantiator = instantiator; }

    private:
        friend class ResourceLoaderFactory;

        std::type_index m_type = typeid(T);
        ResourceInstantiator<T> m_instantiator{};
    };

}
