#ifndef GENODE_IO_RESOURCE_LOADER_HPP
#define GENODE_IO_RESOURCE_LOADER_HPP

#include <Genode/IO/Resource.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <SFML/System/InputStream.hpp>

namespace Gx
{
    template<typename T>
    class ResourceLoader
    {
    public:
        template <typename R, typename... Args>
        using ResourceCreator = std::function<std::unique_ptr<R>(Args...)>;

        ResourceLoader() = default;
        ResourceLoader(ResourceLoader& copy) :
            m_type(copy.m_type),
            m_creator(copy.m_creator ? copy.m_creator->Clone() : nullptr) {}

        virtual ~ResourceLoader() = default;

        virtual bool IsStreaming() const { return false; }

        std::type_index GetResourceType() const { return m_type; }

        virtual ResourcePtr<T> LoadFromFile(const std::string& fileName, const ResourceContext& ctx) const = 0;
        virtual ResourcePtr<T> LoadFromMemory(void* data, std::size_t size, const ResourceContext& ctx) const = 0;
        virtual ResourcePtr<T> LoadFromStream(sf::InputStream& stream, const ResourceContext& ctx) const = 0;

    protected:
        template<class... Args>
        std::unique_ptr<T> Create(Args&&... args) const;

        template<typename R, typename... Args>
        void SetResourceCreator(const ResourceCreator<R, Args...>& builder);

    private:
        friend class ResourceLoaderFactory;
        struct ResourceBuilderBase
        {
            std::type_index Type;

            explicit ResourceBuilderBase(const std::type_index& type) : Type(type) {};
            virtual ~ResourceBuilderBase() = default;

            template<typename... Args>
            std::unique_ptr<T> Build(Args&&... args) const;

            virtual std::unique_ptr<ResourceBuilderBase> Clone() = 0;
        };

        template <typename... Args>
        struct ResourceBuilder : ResourceBuilderBase
        {
            ResourceCreator<T, Args...> Create;

            explicit ResourceBuilder(const std::type_index& type, ResourceCreator<T, Args...> builder) :
                ResourceBuilderBase(type), Create(builder) { };

            std::unique_ptr<ResourceBuilderBase> Clone() override { return std::make_unique<ResourceBuilder>(*this); }
        };

        std::type_index m_type = typeid(T);
        std::unique_ptr<ResourceBuilderBase> m_creator;
    };
}

#include <Genode/IO/ResourceLoader.inl>
#endif