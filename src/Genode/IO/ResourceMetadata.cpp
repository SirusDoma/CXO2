#include <Genode/IO/ResourceMetadata.hpp>

namespace Gx
{
    ResourceMetadata::ResourceMetadata() :
        Name(),
        ResourceType(),
        References()
    {
    }

    ResourceMetadata::~ResourceMetadata()
    {
        References.clear();
    }

    void ResourceMetadata::Require(const std::string &name, const std::string &value, const ResourceReference::ResourceType &type)
    {
        References.insert(std::make_pair(type, ResourceReference{name, value, type}));
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
        auto it = References.find(type);
        if (it != References.end())
            return &it->second;

        return nullptr;
    }

    const ResourceReference *ResourceMetadata::GetResourceReference(const std::string &name) const
    {
        for (auto it = References.begin(); it != References.end(); ++it)
        {
            if (it->second.Name == name)
                return &it->second;
        }

        return nullptr;
    }
}
