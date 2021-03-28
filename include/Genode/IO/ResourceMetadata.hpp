#ifndef GENODE_RESOURCE_METADATA_HPP
#define GENODE_RESOURCE_METADATA_HPP

#include <string>
#include <map>
#include <functional>

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

        const std::string& GetType() const;
        void SetType(const std::string &type);

        const std::string& GetName() const;
        void SetName(const std::string &name);

        void Require(const std::string& name, const std::string& value, const ResourceReference::ResourceType& type);
        void RequireTexture(const std::string& name, const std::string& value);
        void RequireFont(const std::string& name, const std::string& value);
        void RequireSoundBuffer(const std::string& name, const std::string& value);
        void RequireMetadata(const std::string& name, const std::string& value);

        const ResourceReference* GetResourceReference(const ResourceReference::ResourceType& type) const;
        const ResourceReference* GetResourceReference(const std::string& name) const;

    private:
        std::string m_type;
        std::string m_name;
        std::map<ResourceReference::ResourceType, ResourceReference> m_references;
    };
}

#endif