#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/Metadata/Chart/O2JamMusicList.hpp>

namespace Cx
{
    class O2JamMusicListLoader : public Gx::ResourceLoader<O2JamMusicList>
    {
    public:
        [[nodiscard]] Gx::ResourcePtr<O2JamMusicList> LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<O2JamMusicList> LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const override;
        [[nodiscard]] Gx::ResourcePtr<O2JamMusicList> LoadFromStream(sf::InputStream& input, const Gx::ResourceContext& ctx) const override;
    };
}
