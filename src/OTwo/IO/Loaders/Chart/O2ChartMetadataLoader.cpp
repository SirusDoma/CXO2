#include <OTwo/IO/Loaders/Chart/O2ChartMetadataLoader.hpp>

Gx::ResourcePtr <O2ChartMetadata> O2ChartMetadataLoader::LoadFromFile(const std::string &fileName, const Gx::ResourceContext &ctx) const
{
    auto stream = Gx::FileSystem::Open(fileName);
    if (!stream)
        throw Gx::ResourceLoadException("Failed to open the file: " + fileName);

    auto& inputStream = *stream;
    auto metadata = LoadFromStream(inputStream, ctx);
    if (metadata)
        metadata->Source = fileName;

    return metadata;
}

Gx::ResourcePtr <O2ChartMetadata> O2ChartMetadataLoader::LoadFromMemory(void *data, std::size_t size, const Gx::ResourceContext &ctx) const
{
    auto stream = sf::MemoryInputStream();
    stream.open(data, size);

    return LoadFromStream(stream, ctx);
}

Gx::ResourcePtr <O2ChartMetadata> O2ChartMetadataLoader::LoadFromStream(sf::InputStream &stream, const Gx::ResourceContext &ctx) const
{
    auto metadata = std::make_unique<O2ChartMetadata>();
    static Gx::Int64 size = sizeof(O2ChartMetadata)
                          - sizeof(std::string); // exclude .Source field

    if (stream.read(metadata.get(), size) != size)
        return nullptr;

    metadata->Source = ctx.GetID();
    return metadata;
}
