#include <Genode/IO/ResourceManager.hpp>

namespace Gx
{
    ResourceManager::ResourceManager() :
        m_archives(),
        m_entries(),
        m_containers()
    {
        Register<sf::Texture>();
        Register<sf::Font>();
        Register<sf::SoundBuffer>();
        Register<ResourceMetadata>();
    }

    ResourceManager::~ResourceManager()
    {
    }

    ResourceContext ResourceManager::ResolveContext(const ResourceMetadata &metadata)
    {
        auto context = ResourceContext();
        context.Resources = this;
        for (auto type : {ResourceReference::Texture, ResourceReference::Font, ResourceReference::SoundBuffer })
        {
            auto ref = metadata.GetResourceReference(type);
            if (!ref)
                continue;

            if (ref->Type == ResourceReference::Texture)
                context.Texture = Load<sf::Texture>(ref->Value);
            else if (ref->Type == ResourceReference::Font)
                context.Font = Load<sf::Font>(ref->Value);
            else if (ref->Type == ResourceReference::SoundBuffer)
                context.SoundBuffer = Load<sf::SoundBuffer>(ref->Value);
        }

        return context;
    }

    Uint64 ResourceManager::GetResourceData(const std::string &name, Gx::Uint8 **data) const
    {
        auto iterator = m_entries.find(name);
        if (iterator != m_entries.end())
        {
            auto entry = iterator->second;
            return entry.GetContent(data);
        }
        else if (FileHelper::Exists(name, true))
            return FileHelper::ReadFile(name, data);
        else
            return 0;
    }
}
