#pragma once

#include <Genode/IO/Archive.hpp>
#include <Genode/IO/ResourceContainer.hpp>
#include <Genode/Utilities/Extensions.hpp>

#include <typeindex>
#include <memory>

namespace Gx
{
    class ResourceContext;
    class ResourceManager final
    {
    public:
        using ContextBuilder = std::function<std::unique_ptr<ResourceContext>(const std::string&, ResourceManager&, const CacheMode)>;

        ResourceManager();
        ~ResourceManager() = default;

        template<typename R>
        void Register();

        template<typename R>
        bool Release();

        template<typename R, typename U = std::string>
        ResourcePtr<R> Instantiate(const type_identity_t<U>& id);

        template<typename R, typename U = std::string>
        ResourcePtr<R> Instantiate(const type_identity_t<U>& id, const std::string& fileName);

        template<typename R, typename U = std::string>
        ResourcePtr<R> Instantiate(const type_identity_t<U>& id, void* data, std::size_t size);

        template<typename R, typename U = std::string>
        ResourcePtr<R> Instantiate(const type_identity_t<U>& id, sf::InputStream& stream);

        template<typename R, typename U = std::string>
        ResourcePtr<R> Instantiate(const type_identity_t<U>& id, std::function<ResourcePtr<R>()> deserializer);

        template<typename R, typename U = std::string>
        R& AddFromFile(const type_identity_t<U>& idOrFileName, CacheMode mode = CacheMode::Reuse);

        template<typename R, typename U = std::string>
        R& AddFromFile(const type_identity_t<U>& id, const std::string& fileName, CacheMode mode = CacheMode::Reuse);

        template<typename R, typename U = std::string>
        R& AddFromMemory(const type_identity_t<U>& id, void* data, std::size_t size, CacheMode mode = CacheMode::Update);

        template<typename R, typename U = std::string>
        R& AddFromStream(const type_identity_t<U>& id, sf::InputStream& stream, CacheMode mode = CacheMode::Update);

        template<typename R, typename U = std::string>
        R& AddFromDeserializer(const type_identity_t<U>& id, std::function<ResourcePtr<R>()> deserializer, CacheMode mode = CacheMode::Reuse);

        template<typename R, typename U = std::string, class... Args>
        R& Create(const type_identity_t<U>& id, Args&&... args);

        template<typename R, typename U = std::string>
        R& Store(const type_identity_t<U>& id, R& resource, CacheMode mode = CacheMode::Update);

        template<typename R, typename U = std::string>
        R& Store(const type_identity_t<U>& id, ResourcePtr<R> resource, CacheMode mode = CacheMode::Update);

        template<typename R, typename U = std::string>
        R* Find(const type_identity_t<U>& id) const;

        template<typename R, typename U = std::string>
        void Each(const std::function<void(const type_identity_t<U>&, R&)> &callback);

        template<typename R>
        [[nodiscard]] unsigned int Count() const;

        template<typename R, typename U = std::string>
        bool Destroy(const type_identity_t<U>& id);

        template<typename R>
        bool Destroy(const R& resource);

        void SetContextBuilder(const ContextBuilder& builder);

        void Clear();

    private:
        class ContainerBase
        {
        public:
            virtual ~ContainerBase() = default;
        };

        template<typename R, typename U = std::string>
        class ContainerWrapper final : public ContainerBase
        {
        public:
            explicit ContainerWrapper(std::unique_ptr<ResourceContainer<R, U>> container) : Container(std::move(container)) {};
            std::unique_ptr<ResourceContainer<R, U>> Container;
        };
        using ContainerMap = std::unordered_map<std::type_index, std::unique_ptr<ContainerBase>>;

        ContainerMap   m_containers{};
        ContextBuilder m_contextBuilder{};
    };
}

#include <Genode/IO/ResourceManager.inl>
