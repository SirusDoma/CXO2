#ifndef GENODE_IO_RESOURCE_METADATA_HPP
#define GENODE_IO_RESOURCE_METADATA_HPP

#include <string>
#include <map>
#include <functional>
#include <memory>

namespace Gx
{
    template<typename R>
    using ResourcePtr = std::unique_ptr<R, std::function<void(R*)>>;

    struct ResourceReference
    {
        enum ResourceType
        {
            Custom,
            Texture,
            Image,
            Font,
            SoundBuffer,
            Shader,
            Metadata
        };

        const std::string Name;
        const std::string Value;
        const ResourceType Type;
    };

    class ResourceMetadata
    {
    public:
        ResourceMetadata();
        virtual ~ResourceMetadata();

        std::string ResourceType;
        std::string Name;
        std::vector<ResourceReference> References;

        void Require(const std::string& name, const std::string& value, const ResourceReference::ResourceType& type);
        void RequireTexture(const std::string& name, const std::string& value);
        void RequireFont(const std::string& name, const std::string& value);
        void RequireSoundBuffer(const std::string& name, const std::string& value);
        void RequireMetadata(const std::string& name, const std::string& value);
    };
}

#endif