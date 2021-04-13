#include <Genode/IO/ResourceMetadata.hpp>

namespace Gx
{
    ResourceMetadata::ResourceMetadata() :
        m_name(),
        m_type(),
        m_references()
    {
    }

    ResourceMetadata::~ResourceMetadata()
    {
        m_references.clear();
    }

    const std::string &ResourceMetadata::GetResourceType() const
    {
        return m_type;
    }

    void ResourceMetadata::SetResourceType(const std::string &type)
    {
        m_type = type;
    }

    const std::string &ResourceMetadata::GetName() const
    {
        return m_name;
    }

    void ResourceMetadata::SetName(const std::string &name)
    {
        m_name = name;
    }

    void ResourceMetadata::Require(const std::string &name, const std::string &value, const ResourceReference::ResourceType &type)
    {
        m_references.insert(std::make_pair(type, ResourceReference{name, value, type}));
    }

    void ResourceMetadata::RequireTexture(const std::string &name, const std::string &value)
    {
        Require(name, value, ResourceReference::Texture);
    }

    void ResourceMetadata::RequireFont(const std::string &name, const std::string &value)
    {
        Require(name, value, ResourceReference::Font);
    }

    void ResourceMetadata::RequireSoundBuffer(const std::string &name, const std::string &value)
    {
        Require(name, value, ResourceReference::SoundBuffer);
    }

    void ResourceMetadata::RequireMetadata(const std::string &name, const std::string &value)
    {
        Require(name, value, ResourceReference::Metadata);
    }

    const ResourceReference* ResourceMetadata::GetResourceReference(const ResourceReference::ResourceType &type) const
    {
        auto it = m_references.find(type);
        if (it != m_references.end())
            return &it->second;

        return nullptr;
    }

    const ResourceReference *ResourceMetadata::GetResourceReference(const std::string &name) const
    {
        for (auto it = m_references.begin(); it != m_references.end(); ++it)
        {
            if (it->second.Name == name)
                return &it->second;
        }

        return nullptr;
    }
}
