#pragma once
#include <Genode/IO/ResourceLoader.hpp>

namespace Cx
{
    struct ControlList;

    class ControlListLoader : public Gx::ResourceLoader<ControlList>
    {
    public:
        ControlListLoader() = default;
        explicit ControlListLoader(bool mapBounds);

        [[nodiscard]] Gx::ResourcePtr<ControlList> LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<ControlList> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<ControlList> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;

    private:
        static Gx::ResourcePtr<ControlList> Parse(std::istringstream source, bool mapBounds, const Gx::ResourceContext& ctx);

        bool m_mapBounds = true;
    };
}
