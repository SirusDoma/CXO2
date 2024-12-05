#include <Genode/IO/ResourceManager.hpp>
#include <Genode/IO/ResourceContext.hpp>

namespace Gx
{
    ResourceManager::ResourceManager()
    {
        m_contextBuilder = [] (const std::string& id, ResourceManager& manager, const CacheMode mode) {
            return std::make_unique<ResourceContext>(id, manager);
        };
    }

    void ResourceManager::SetContextBuilder(const ContextBuilder& builder)
    {
        m_contextBuilder = builder;
    }

    void ResourceManager::Clear()
    {
        m_containers.clear();
    }
}