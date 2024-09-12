#include <Genode/IO/Loaders/FontLoader.hpp>
#include <Genode/IO/FileSystem/LocalFileSystem.hpp>

#include <Genode/Graphics/Font.hpp>

namespace Gx
{
    bool FontLoader::IsStreaming() const
    {
        return true;
    }

    void FontLoader::UseSmooth(const bool smooth)
    {
        m_smooth = smooth;
    }

    ResourcePtr<Font> FontLoader::LoadFromFile(const std::string& fileName, const ResourceContext& ctx) const
    {
        auto resource = std::make_unique<Font>();
        if (!resource->LoadFromFile(LocalFileSystem::Instance().GetFullName(fileName)))
            return nullptr;

        resource->SetSmooth(m_smooth);
        return resource;
    }

    ResourcePtr<Font> FontLoader::LoadFromMemory(void* data, const std::size_t size, const ResourceContext& ctx) const
    {
        auto resource = std::make_unique<Font>();
        if (!resource->LoadFromMemory(data, size))
            return nullptr;

        resource->SetSmooth(m_smooth);
        return resource;
    }

    ResourcePtr<Font> FontLoader::LoadFromStream(sf::InputStream& stream, const ResourceContext& ctx) const
    {
        auto resource = std::make_unique<Font>();
        if (!resource->LoadFromStream(stream))
            return nullptr;

        resource->SetSmooth(m_smooth);
        return resource;
    }
}
