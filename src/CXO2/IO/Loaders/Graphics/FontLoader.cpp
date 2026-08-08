#include <CXO2/IO/Loaders/Graphics/FontLoader.hpp>
#include <Genode/IO/FileSystem.hpp>
#include <Genode/IO/BufferedInputStream.hpp>

#include <Genode/Graphics/Font.hpp>

#include <cstddef>
#include <memory>
#include <vector>

namespace
{
    class Font : public Gx::Font
    {
    public:
        Font(const void* data, const std::size_t size) :
            m_buffer(data, size)
        {
            m_loaded = LoadFromStream(m_buffer);
        }

        explicit Font(std::vector<std::byte>&& data) :
            m_buffer(std::move(data))
        {
            m_loaded = LoadFromStream(m_buffer);
        }

        [[nodiscard]] bool IsLoaded() const
        {
            return m_loaded;
        }

    private:
        Gx::BufferedInputStream m_buffer;
        bool m_loaded = false;
    };

    std::vector<std::byte> Slurp(sf::InputStream& stream)
    {
        const auto size     = stream.getSize();
        const auto position = stream.tell();
        if (!size.has_value() || !position.has_value())
            return {};

        const auto remaining = size.value() - position.value();
        auto data = std::vector<std::byte>(remaining);
        if (const auto read = stream.read(data.data(), remaining); !read.has_value() || read.value() != remaining)
            return {};

        return data;
    }
}

namespace Cx
{
    bool FontLoader::IsStreaming() const
    {
        return false;
    }

    void FontLoader::UseSmooth(const bool smooth)
    {
        m_smooth = smooth;
    }

    Gx::ResourcePtr<Gx::Font> FontLoader::LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const
    {
        const auto stream = Gx::FileSystem::Open(fileName);
        if (!stream)
            throw Gx::ResourceLoadException(fileName.string());

        auto data = Slurp(*stream);
        if (data.empty())
            return nullptr;

        auto resource = std::make_unique<::Font>(std::move(data));
        if (!resource->IsLoaded())
            return nullptr;

        resource->SetSmooth(m_smooth);
        return std::unique_ptr<Gx::Font>(std::move(resource));
    }

    Gx::ResourcePtr<Gx::Font> FontLoader::LoadFromMemory(void* data, const std::size_t size, const Gx::ResourceContext& ctx) const
    {
        auto resource = std::make_unique<::Font>(data, size);
        if (!resource->IsLoaded())
            return nullptr;

        resource->SetSmooth(m_smooth);
        return std::unique_ptr<Gx::Font>(std::move(resource));
    }

    Gx::ResourcePtr<Gx::Font> FontLoader::LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const
    {
        auto data = Slurp(stream);
        if (data.empty())
            return nullptr;

        auto resource = std::make_unique<::Font>(std::move(data));
        if (!resource->IsLoaded())
            return nullptr;

        resource->SetSmooth(m_smooth);
        return std::unique_ptr<Gx::Font>(std::move(resource));
    }
}
