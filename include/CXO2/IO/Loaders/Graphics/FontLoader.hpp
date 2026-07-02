#pragma once

#include <Genode/IO/ResourceLoader.hpp>
#include <string>

namespace Gx
{
    class Font;
}

namespace Cx
{

    class FontLoader final : public Gx::ResourceLoader<Gx::Font>
    {
    public:
        FontLoader() = default;

        bool IsStreaming() const override;
        void UseSmooth(bool smooth);

        [[nodiscard]] Gx::ResourcePtr<Gx::Font> LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<Gx::Font> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<Gx::Font> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;

    private:
        bool m_smooth = true;
    };
}
