#pragma once
#include <Genode/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/Legacy/ControlList.hpp>

namespace Cx
{
    class BoundLoader : public Gx::ResourceLoader<std::vector<ControlList::Bound>>
    {
    public:
        [[nodiscard]] Gx::ResourcePtr<std::vector<ControlList::Bound>> LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<std::vector<ControlList::Bound>> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<std::vector<ControlList::Bound>> LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const override;
    };


}
