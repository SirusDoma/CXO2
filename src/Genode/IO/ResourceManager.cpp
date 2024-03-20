#include <Genode/IO/ResourceManager.hpp>
#include <Genode/IO/ResourceContext.hpp>

namespace Gx
{
    ResourceManager::ResourceManager() :
        m_containers(),
        m_contextBuilder()
    {
        m_contextBuilder = [] (const std::string &id, ResourceManager &manager, const CacheMode mode) {
            return std::make_unique<ResourceContext>(id, manager);
        };
    }

    ResourceManager::ResourceManager(ResourceManager &&other) noexcept :
        m_containers(std::move(other.m_containers)),
        m_contextBuilder(std::move(other.m_contextBuilder))
    {
    }

    ResourceManager &ResourceManager::operator=(ResourceManager &&right) noexcept
    {
        m_containers     = std::move(right.m_containers);
        m_contextBuilder = std::move(right.m_contextBuilder);

        return *this;
    }

    void ResourceManager::ConfigureContextBuilder(const ContextBuilder &builder)
    {
        m_contextBuilder = builder;
    }

    void ResourceManager::Clear()
    {
        m_containers.clear();
    }
}