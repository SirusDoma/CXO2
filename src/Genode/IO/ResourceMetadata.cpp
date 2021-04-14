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
        References.push_back(ResourceReference{name, value, type});
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
}
