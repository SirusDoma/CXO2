#pragma once

#include <Genode/IO/ResourceLoaderFactory.hpp>
#include <Genode/IO/ResourceLoader.hpp>

namespace Gx
{
    template<typename R>
    void ResourceManager::Register()
    {
        if (m_containers.find(typeid(R)) != m_containers.end())
            return;

        m_containers[typeid(R)] = std::make_unique<ContainerWrapper<R>>(std::make_unique<ResourceContainer<R>>());
    }

    template<typename R>
    bool ResourceManager::Release()
    {
        return m_containers.erase(typeid(R)) != 0;
    }

    template<typename R>
    ResourcePtr<R> ResourceManager::Instantiate(const std::string& id)
    {
        Register<R>();

        auto resource = Find<R>(id);
        if (resource)
            return std::make_unique<R>(*resource);

        return Instantiate<R>(id, id);
    }

    template<typename R>
    ResourcePtr<R> ResourceManager::Instantiate(const std::string& id, const std::string& fileName)
    {
        Register<R>();

        auto resource = &AddFromFile<R>(id, fileName, CacheMode::Reuse);
        return std::make_unique<R>(*resource);
    }

    template<typename R>
    ResourcePtr<R> ResourceManager::Instantiate(const std::string& id, void* data, std::size_t size)
    {
        Register<R>();

        auto resource = &AddFromMemory<R>(id, data, size, CacheMode::Reuse);
        return std::make_unique<R>(*resource);
    }

    template<typename R>
    ResourcePtr<R> ResourceManager::Instantiate(const std::string& id, sf::InputStream& stream)
    {
        Register<R>();

        auto resource = &AddFromStream<R>(id, stream, CacheMode::Reuse);
        return std::make_unique<R>(*resource);
    }

    template<typename R>
    ResourcePtr<R> ResourceManager::Instantiate(const std::string& id, std::function<ResourcePtr<R>()> deserializer)
    {
        Register<R>();

        auto resource = &AddFromDeserializer<R>(id, deserializer, CacheMode::Reuse);
        return std::make_unique<R>(*resource);
    }

    template<typename R>
    R& ResourceManager::AddFromFile(const std::string& idOrFileName, CacheMode mode)
    {
        Register<R>();

        auto loader = ResourceLoaderFactory::CreateLoader<R>();
        if (!loader)
            throw ResourceLoadException("There's no [ResourceLoader] for [" + std::string(typeid(R).name()) + "] type");

        auto managed = static_cast<ContainerWrapper<R>*>(m_containers[typeid(R)].get());
        auto deserializer = [&, this] () {
            auto ctx = std::move(m_contextBuilder(idOrFileName, *this, mode));
            return loader->LoadFromFile(idOrFileName, *ctx);
        };

        return managed->Container->Store(idOrFileName, deserializer, mode);
    }

    template<typename R>
    R& ResourceManager::AddFromFile(const std::string& id, const std::string& fileName, CacheMode mode)
    {
        Register<R>();

        auto loader = ResourceLoaderFactory::CreateLoader<R>();
        if (!loader)
            throw ResourceLoadException("There's no [ResourceLoader] for [" + std::string(typeid(R).name()) + "] type");

        auto managed = static_cast<ContainerWrapper<R>*>(m_containers[typeid(R)].get());
        auto deserializer = [&, this] () {
            auto ctx = std::move(m_contextBuilder(id, *this, mode));
            return loader->LoadFromFile(fileName, *ctx);
        };

        return managed->Container->Store(id, deserializer, mode);
    }

    template<typename R>
    R& ResourceManager::AddFromMemory(const std::string& id, void* data, std::size_t size, CacheMode mode)
    {
        Register<R>();

        auto loader = ResourceLoaderFactory::CreateLoader<R>();
        if (!loader)
            throw ResourceLoadException("There's no [ResourceLoader] for [" + std::string(typeid(R).name()) + "] type");

        auto managed = static_cast<ContainerWrapper<R>*>(m_containers[typeid(R)].get());
        auto deserializer = [&, this] () {
            auto ctx = std::move(m_contextBuilder(id, *this, mode));
            return loader->LoadFromMemory(data, size, *ctx);
        };

        return managed->Container->Store(id, deserializer, mode);
    }

    template<typename R>
    R& ResourceManager::AddFromStream(const std::string& id, sf::InputStream& stream, CacheMode mode)
    {
        Register<R>();

        auto loader = ResourceLoaderFactory::CreateLoader<R>();
        if (!loader)
            throw ResourceLoadException("There's no [ResourceLoader] for [" + std::string(typeid(R).name()) + "] type.");

        auto managed = static_cast<ContainerWrapper<R>*>(m_containers[typeid(R)].get());
        auto deserializer = [&, this] () {
            auto ctx = std::move(m_contextBuilder(id, *this, mode));
            return loader->LoadFromStream(stream, *ctx);
        };

        return managed->Container->Store(id, deserializer, mode);
    }

    template<typename R>
    R& ResourceManager::AddFromDeserializer(const std::string& id, std::function<ResourcePtr<R>()> deserializer, CacheMode mode)
    {
        Register<R>();

        auto managed = static_cast<ContainerWrapper<R>*>(m_containers[typeid(R)].get());
        auto& result = managed->Container->Store(id, deserializer, mode);

        return result;
    }

    template<typename R, class... Args>
    R& ResourceManager::Create(const std::string& id, Args&&... args)
    {
        Register<R>();

        ResourcePtr<R> resource = std::make_unique<R>(std::forward<Args>(args)...);

        auto managed = static_cast<ContainerWrapper<R>*>(m_containers[typeid(R)].get());
        return managed->Container->Store(id, std::move(resource), CacheMode::None);
    }

    template<typename R>
    R& ResourceManager::Store(const std::string& id, R& resource, CacheMode mode)
    {
        Register<R>();

        auto managed = static_cast<ContainerWrapper<R>*>(m_containers[typeid(R)].get());
        return managed->Container->Store(id, std::make_unique<R>(resource), mode);
    }

    template<typename R>
    R& ResourceManager::Store(const std::string& id, ResourcePtr<R> resource, CacheMode mode)
    {
        if (!resource)
            throw ResourceStoreException("[" + id + "]\nCannot store empty resource.");

        Register<R>();

        auto managed = static_cast<ContainerWrapper<R>*>(m_containers[typeid(R)].get());
        return managed->Container->Store(id, std::move(resource), mode);
    }

    template<typename R>
    R* ResourceManager::Find(const std::string& id) const
    {
        const auto it = m_containers.find(typeid(R));
        if (it == m_containers.end())
            return nullptr;

        auto managed = dynamic_cast<ContainerWrapper<R>*>(it->second.get());
        if (!managed)
            return nullptr;

        return managed->Container->Find(id);
    }

    template<typename R>
    void ResourceManager::Each(const std::function<void(const std::string& , R&)> &callback)
    {
        const auto it = m_containers.find(typeid(R));
        if (it == m_containers.end())
            return;

        auto managed = dynamic_cast<ContainerWrapper<R>*>(it->second.get());
        if (!managed)
            return;

        managed->Container->Each(callback);
    }

    template<typename R>
    bool ResourceManager::Destroy(const R& resource)
    {
        const auto it = m_containers.find(typeid(R));
        if (it == m_containers.end())
            return false;

        auto managed = static_cast<ContainerWrapper<R>*>(it->second.get());
        return managed->Container->Destroy(resource);
    }

    template<typename R>
    bool ResourceManager::Destroy(const std::string& id)
    {
        Register<R>();

        auto managed = static_cast<ContainerWrapper<R>*>(m_containers[typeid(R)].get());
        return managed->Container->Destroy(id);
    }

    template<typename R>
    unsigned int ResourceManager::Count() const
    {
        if (auto it = m_containers.find(typeid(R)); it != m_containers.end())
        {
            auto managed = dynamic_cast<const ContainerWrapper<R>*>(it->second.get());
            return managed->Container->Count();
        }

        return 0;
    }
}