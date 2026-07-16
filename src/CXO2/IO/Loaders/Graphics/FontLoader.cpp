#include <CXO2/IO/Loaders/Graphics/FontLoader.hpp>
#include <Genode/IO/FileSystem/FileSystem.hpp>

#include <Genode/Graphics/Font.hpp>

#include <cstring>

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

    Gx::ResourcePtr<Gx::Font> FontLoader::LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const
    {
        auto stream = Gx::FileSystem::Open(fileName);
        if (!stream)
            throw Gx::ResourceLoadException(fileName.string());

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
        auto buffer = new std::uint8_t[size];
        std::memcpy(buffer, data, size);

        auto resource = Gx::ResourcePtr<Gx::Font>(new Gx::Font(), [buffer] (auto ptr)
        {
            delete ptr;
            delete[] buffer;
        });

        if (!resource->LoadFromMemory(buffer, size))
            return nullptr;

        resource->SetSmooth(m_smooth);
        return resource;
    }

    Gx::ResourcePtr<Gx::Font> FontLoader::LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const
    {
        const auto size = stream.getSize();
        const auto position = stream.tell();
        if (!size.has_value() || !position.has_value())
            return nullptr;

        const auto remaining = size.value() - position.value();
        auto buffer = new std::uint8_t[remaining];
        if (stream.read(buffer, remaining) != remaining)
        {
            delete[] buffer;
            return nullptr;
        }

        auto resource = Gx::ResourcePtr<Gx::Font>(new Gx::Font(), [buffer] (auto ptr)
        {
            delete ptr;
            delete[] buffer;
        });

        if (!resource->LoadFromMemory(buffer, remaining))
            return nullptr;

        resource->SetSmooth(m_smooth);
        return resource;
    }
}
