#include <Genode/IO/ResourceManager.hpp>

namespace Gx
{
    ResourceManager::ResourceManager() :
        m_archives(),
        m_entries(),
        m_containers()
    {
        Register<sf::Texture>();
        Register<sf::Image>();
        Register<sf::Font>();
        Register<sf::SoundBuffer>();
        Register<sf::Shader>();
        Register<ResourceMetadata>();
    }

    ResourceManager::~ResourceManager()
    {
    }

    ResourceContext ResourceManager::ResolveContext(const ResourceMetadata &metadata)
    {
        auto context = ResourceContext();
        context.Name      = metadata.Name;
        context.Resources = this;
        for (auto ref : metadata.References)
        {
            switch (ref.Type)
            {
                case ResourceReference::Texture:     context.Texture     = Load<sf::Texture>(ref.Value);     break;
                case ResourceReference::Image:       context.Image       = Load<sf::Image>(ref.Value);       break;
                case ResourceReference::Font:        context.Font        = Load<sf::Font>(ref.Value);        break;
                case ResourceReference::SoundBuffer: context.SoundBuffer = Load<sf::SoundBuffer>(ref.Value); break;
                case ResourceReference::Shader:      context.Shader      = Load<sf::Shader>(ref.Value);      break;
                case ResourceReference::Metadata:    context.Metadata    = LoadMetadata<ResourceMetadata>(ref.Value); break;
                default: break;
            }
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
