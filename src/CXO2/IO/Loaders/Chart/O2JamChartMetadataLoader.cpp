#include <CXO2/IO/Loaders/Chart/O2JamChartMetadataLoader.hpp>
#include <CXO2/IO/Loaders/Chart/O2JamChartLoader.hpp>

namespace Cx
{
    Gx::ResourcePtr<O2JamChartMetadata> O2JamChartMetadataLoader::LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const
    {
        const auto stream = Gx::FileSystem::Open(fileName);
        if (!stream)
            throw Gx::ResourceLoadException(fileName.string());

        auto& inputStream = *stream;
        auto metadata = LoadFromStream(inputStream, ctx);

        if (metadata)
            metadata->Source = fileName.string();

        return metadata;
    }

    Gx::ResourcePtr<O2JamChartMetadata> O2JamChartMetadataLoader::LoadFromMemory(void* data, const std::size_t size, const Gx::ResourceContext& ctx) const
    {
        auto stream = sf::MemoryInputStream(data, size);
        return LoadFromStream(stream, ctx);
    }

    Gx::ResourcePtr<O2JamChartMetadata> O2JamChartMetadataLoader::LoadFromStream(sf::InputStream& input, const Gx::ResourceContext& ctx) const
    {
        const auto decryptedStream = O2JamChartLoader::Decrypt(Gx::ResourcePtr<sf::InputStream>(&input, [] (auto) {}));
        auto& stream = *decryptedStream.get();

        auto metadata = std::make_unique<O2JamChartMetadata>();
        if (stream.read(static_cast<O2JamChartHeader*>(metadata.get()), sizeof(O2JamChartHeader)) != sizeof(O2JamChartHeader))
            return nullptr;

        metadata->Source = ctx.GetID();
        return metadata;
    }
}
