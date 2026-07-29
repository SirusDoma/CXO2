#include <CXO2/IO/Loaders/Chart/O2JamMusicListLoader.hpp>

#include <Genode/IO/FileSystem.hpp>
#include <Genode/IO/IOException.hpp>

#include <unordered_map>

namespace Cx
{
    Gx::ResourcePtr<O2JamMusicList> O2JamMusicListLoader::LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const
    {
        const auto stream = Gx::FileSystem::Open(fileName);
        if (!stream)
            throw Gx::ResourceLoadException(fileName.string());

        auto& inputStream = *stream;
        auto list = LoadFromStream(inputStream, ctx);

        if (list)
            list->Source = fileName.string();

        return list;
    }

    Gx::ResourcePtr<O2JamMusicList> O2JamMusicListLoader::LoadFromMemory(void* data, const std::size_t size, const Gx::ResourceContext& ctx) const
    {
        auto stream = sf::MemoryInputStream(data, size);
        return LoadFromStream(stream, ctx);
    }

    Gx::ResourcePtr<O2JamMusicList> O2JamMusicListLoader::LoadFromStream(sf::InputStream& input, const Gx::ResourceContext& ctx) const
    {
        auto list = std::make_unique<O2JamMusicList>();
        list->Source = ctx.GetID();

        std::uint32_t count{};
        if (input.read(&count, sizeof(count)) != sizeof(count))
            return nullptr;

        for (std::uint32_t i = 0; i < count; i++)
        {
            auto chart = O2JamChartMetadata();
            if (input.read(static_cast<O2JamChartHeader*>(&chart), sizeof(O2JamChartHeader)) != sizeof(O2JamChartHeader))
                return nullptr;

            list->Charts.push_back(std::move(chart));
        }

        auto indices = std::unordered_map<std::uint32_t, std::size_t>();
        for (std::size_t i = 0; i < list->Charts.size(); i++)
            indices[list->Charts[i].ID] = i;

        std::uint32_t newCount{};
        if (input.read(&newCount, sizeof(newCount)) != sizeof(newCount))
            return list;

        for (std::uint32_t i = 0; i < newCount; i++)
        {
            std::uint32_t entry[4]{};
            if (input.read(entry, sizeof(entry)) != sizeof(entry))
                return list;

            if (const auto it = indices.find(entry[0]); it != indices.end())
                list->Charts[it->second].IsNew = entry[1] != 0;
        }

        std::uint32_t premiumCount{};
        if (input.read(&premiumCount, sizeof(premiumCount)) != sizeof(premiumCount))
            return list;

        for (std::uint32_t i = 0; i < premiumCount; i++)
        {
            std::uint32_t entry[4]{};
            if (input.read(entry, sizeof(entry)) != sizeof(entry))
                return list;

            if (const auto it = indices.find(entry[0]); it != indices.end() && entry[1] != 0)
                list->Charts[it->second].Prices[Currency::Gem] = entry[1];
        }

        return list;
    }
}
