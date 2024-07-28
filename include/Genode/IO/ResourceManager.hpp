#ifndef GENODE_IO_RESOURCEMANAGER_HPP
#define GENODE_IO_RESOURCEMANAGER_HPP

#include <Genode/IO/Archive.hpp>
#include <Genode/IO/ResourceContainer.hpp>

#include <Genode/System/Context.hpp>

#include <typeindex>
#include <memory>

namespace Gx
{
    class ResourceContext;
    class ResourceManager : NonCopyable, public Context
    {
    public:
        using ContextBuilder = std::function<std::unique_ptr<ResourceContext>(const std::string&, ResourceManager&, const CacheMode)>;

        ResourceManager();
        ResourceManager(ResourceManager &&other) noexcept;
        virtual ~ResourceManager() = default;

        ResourceManager& operator=(ResourceManager&& right) noexcept;

        template<typename R>
        void Register();

        template<typename R>
        bool Release();

        template<typename R>
        ResourcePtr<R> Instantiate(const std::string &id);

        template<typename R>
        ResourcePtr<R> Instantiate(const std::string &id, const std::string &fileName);

        template<typename R>
        ResourcePtr<R> Instantiate(const std::string &id, void *data, std::size_t size);

        template<typename R>
        ResourcePtr<R> Instantiate(const std::string &id, sf::InputStream &stream);

        template<typename R>
        ResourcePtr<R> Instantiate(const std::string &id, std::function<ResourcePtr<R>()> deserializer);

        template<typename R>
        R &AddFromFile(const std::string &idOrFileName, CacheMode mode = CacheMode::Reuse);

        template<typename R>
        R &AddFromFile(const std::string &id, const std::string &fileName, CacheMode mode = CacheMode::Reuse);

        template<typename R>
        R &AddFromMemory(const std::string &id, void *data, std::size_t size, CacheMode mode = CacheMode::Update);

        template<typename R>
        R &AddFromStream(const std::string &id, sf::InputStream &stream, CacheMode mode = CacheMode::Update);

        template<typename R>
        R &AddFromDeserializer(const std::string &id, std::function<ResourcePtr<R>()> deserializer, CacheMode mode = CacheMode::Reuse);

        template<typename R, class... Args>
        R &Create(const std::string &id, Args&&... args);

        template<typename R>
        R &Store(const std::string &id, R& resource, CacheMode mode = CacheMode::Update);

        template<typename R>
        R &Store(const std::string &id, ResourcePtr<R> resource, CacheMode mode = CacheMode::Update);

        template<typename R>
        R *Find(const std::string &id) const;

        template<typename R>
        unsigned int Count() const;

        template<typename R>
        bool Destroy(const std::string &id);

        template<typename R>
        bool Destroy(const R& resource);

        // TODO: This seems to be counter-intuitive because other modules (e.g Scene, Mixer) are using separate resource manager
        //       And therefore, they need to be configured separately as well.
        //       Consider turn this builder into static.

        void ConfigureContextBuilder(const ContextBuilder &builder);

        void Clear();

    private:
        class BaseContainer
        {
        public:
            virtual ~BaseContainer() = default;
        };

        template<typename R>
        class ManagedContainer final : public BaseContainer
        {
        public:
            explicit ManagedContainer(std::unique_ptr<ResourceContainer<R>> container) : Container(std::move(container)) {};
            std::unique_ptr<ResourceContainer<R>> Container;
        };
        using ContainerMap = std::unordered_map<std::type_index, std::unique_ptr<BaseContainer>>;

        ContainerMap   m_containers;
        ContextBuilder m_contextBuilder;
    };
}

#include <Genode/IO/ResourceManager.inl>
#endif
