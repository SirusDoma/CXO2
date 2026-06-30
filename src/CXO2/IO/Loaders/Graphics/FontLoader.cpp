#include <CXO2/IO/Loaders/Graphics/FontLoader.hpp>
#include <Genode/IO/FileSystem/FileSystem.hpp>

#include <Genode/Graphics/Font.hpp>

namespace Cx
{
    bool FontLoader::IsStreaming() const
    {
        return true;
    }

    void FontLoader::UseSmooth(const bool smooth)
    {
        m_smooth = smooth;
    }

    Gx::ResourcePtr<Gx::Font> FontLoader::LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const
    {
        auto stream = Gx::FileSystem::Open(fileName);
        if (!stream)
            throw Gx::ResourceLoadException("Failed to open the file: " + fileName);

        const auto inputStream = stream.release();
        auto resource = Gx::ResourcePtr<Gx::Font>(new Gx::Font(), [inputStream] (auto ptr)
        {
            delete ptr;
            delete inputStream;
        });

        if (!resource->LoadFromStream(*inputStream))
            return nullptr;

        resource->SetSmooth(m_smooth);
        return resource;
    }

    Gx::ResourcePtr<Gx::Font> FontLoader::LoadFromMemory(void* data, const std::size_t size, const Gx::ResourceContext& ctx) const
    {
        auto resource = std::make_unique<Gx::Font>();
        if (!resource->LoadFromMemory(data, size))
            return nullptr;

        resource->SetSmooth(m_smooth);
        return resource;
    }

    Gx::ResourcePtr<Gx::Font> FontLoader::LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const
    {
        auto resource = std::make_unique<Gx::Font>();
        if (!resource->LoadFromStream(stream))
            return nullptr;

        resource->SetSmooth(m_smooth);
        return resource;
    }
}
