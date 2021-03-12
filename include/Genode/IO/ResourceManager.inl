
#include "ResourceManager.hpp"

namespace Gx
{
    template<typename T>
    inline T* ResourceManager::AddArchive(const std::string& fileName)
    {
        static_assert(std::is_base_of<Archive, T>::value, "Parameter must be a Gx::Archive");

        auto name = FileHelper::GetFileName(fileName);
        if (auto archive = GetArchive<T>(name); archive != nullptr)
            return archive;

        m_archives[name] = new T();
        if (!m_archives[name]->Open(FileHelper::GetFullName(fileName)))
        {
            m_archives.erase(m_archives.find(name));
            return nullptr;
        }

        for (auto entry : m_archives[name]->GetFileEntries())
        {
            m_entries[entry.Name] = entry;
            m_entries[entry.Name].Parent = m_archives[name];
        }

        return static_cast<T*>(m_archives[name]);
    }

    template<typename T>
    inline T* ResourceManager::GetArchive(const std::string& filename) const
    {
        static_assert(std::is_base_of<Archive, T>::value, "Parameter must be a Gx::Archive");

        auto name     = FileHelper::GetFileName(filename);
        auto iterator = m_archives.find(name);
        if (iterator != m_archives.end())
            return static_cast<T*>(iterator->second);

        return nullptr;
    }

    template<typename T>
    inline std::shared_ptr<T> ResourceManager::Resolve(const std::string name)
    {
        // Load resources either from cache or file system (archive / physical file)
        if (!m_cache->Contains(name))
        {
            Uint8* data;
            Uint64 size = GetResourceData(name, &data);

            return m_cache->Add<T>(name, data, size, false);
        }

        return m_cache->Get<T>(name);
    }

    template<typename T>
    inline ResourceDefinition* ResourceManager::GetDefinition(const std::string& name, bool cache)
    {
        // Definition of target resource
        std::shared_ptr<ResourceDefinition> definition;

        // Load definition and create context for resource dependencies
        auto context = ResourceContext();
        if (!m_cache->Contains(name))
        {
            Uint8* data;
            Uint64 size = GetResourceData(name, &data);

            // Find capable loader
            auto loader = ResourceLoaderFactory::GetDefinitionLoader<T>();
            if (!loader)
                return nullptr;

            // Load it with loader
            definition = m_cache->Add(name, loader->Load(data, size), cache);
        }
        else
            definition = m_cache->Get<ResourceDefinition>(name);

        // No definition found, cannot proceed
        if (!definition)
            return nullptr;

        return definition.get();
    }
    
    template<typename T>
    inline T* ResourceManager::Create(const std::string& name, bool cache)
    {
        // Definition of target resource
        auto definition = GetDefinition<T>(name, cache);
        if (!definition)
            return nullptr;

        // Find capable loader
        auto loader = ResourceLoaderFactory::GetDefinitionLoader<T>();
        if (!loader)
            return nullptr;

        // Load required resources to build resource from definition
        auto context = ResourceContext();
        for (auto resource : definition->ResourceReferences)
        {
            if (resource.first == "texture")
                context.Texture = Resolve<sf::Texture>(resource.second);
            else if (resource.first == "font")
                context.Font    = Resolve<sf::Font>(resource.second);
        }

        return loader->Create(definition, context);
    }
}