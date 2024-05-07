#include <OTwo/IO/Loaders/Chart/ChartMetadataLoader.hpp>

Gx::ResourcePtr <ChartMetadata> ChartMetadataLoader::LoadFromFile(const std::string &fileName, const Gx::ResourceContext &ctx) const
{
    const auto stream = Gx::FileSystem::Open(fileName);
    if (!stream)
        throw Gx::ResourceLoadException("Failed to open the file: " + fileName);

    auto& inputStream = *stream;
    auto metadata = LoadFromStream(inputStream, ctx);
    if (metadata)
        metadata->Source = fileName;

    return metadata;
}

Gx::ResourcePtr <ChartMetadata> ChartMetadataLoader::LoadFromMemory(void *data, std::size_t size, const Gx::ResourceContext &ctx) const
{
    auto stream = sf::MemoryInputStream();
    stream.open(data, size);

    return LoadFromStream(stream, ctx);
}

Gx::ResourcePtr <ChartMetadata> ChartMetadataLoader::LoadFromStream(sf::InputStream &stream, const Gx::ResourceContext &ctx) const
{
    auto metadata = std::make_unique<ChartMetadata>();
    if (stream.read(metadata.get(), ChartMetadata::Size) != ChartMetadata::Size)
        return nullptr;

    metadata->Source = ctx.GetID();
    return metadata;
}
