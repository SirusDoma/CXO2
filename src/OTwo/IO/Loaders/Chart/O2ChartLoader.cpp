#include <OTwo/IO/Loaders/Chart/O2ChartLoader.hpp>

Gx::ResourcePtr<O2Chart> O2ChartLoader::LoadFromMetadata(const O2ChartMetadata &meta, const Gx::ResourceContext &ctx) const
{
    return LoadFromFile(meta.Source, ctx);
}

Gx::ResourcePtr<O2Chart> O2ChartLoader::LoadFromFile(const std::string &fileName, const Gx::ResourceContext &ctx) const
{
    return Gx::ResourcePtr<O2Chart>();
}

Gx::ResourcePtr<O2Chart> O2ChartLoader::LoadFromMemory(void *data, std::size_t size, const Gx::ResourceContext &ctx) const
{
    return Gx::ResourcePtr<O2Chart>();
}

Gx::ResourcePtr<O2Chart> O2ChartLoader::LoadFromStream(sf::InputStream &stream, const Gx::ResourceContext &ctx) const
{
    return Gx::ResourcePtr<O2Chart>();
}

Gx::ResourcePtr<sf::Texture> O2ChartLoader::LoadThumbnail(O2ChartMetadata &metadata, const Gx::ResourceContext &ctx)
{
    auto fs = Gx::FileSystem::Open(metadata.Source);
    return LoadThumbnail(*fs, metadata, ctx);
}

Gx::ResourcePtr<sf::Texture> O2ChartLoader::LoadCoverArt(O2ChartMetadata &metadata, const Gx::ResourceContext &ctx)
{
    auto fs = Gx::FileSystem::Open(metadata.Source);
    return LoadCoverArt(*fs, metadata, ctx);
}

Gx::ResourcePtr<sf::Texture> O2ChartLoader::LoadThumbnail(sf::InputStream &stream, O2ChartMetadata &metadata, const Gx::ResourceContext &ctx)
{
    if (stream.seek(metadata.CoverOffset + metadata.CoverSize) == -1)
        return nullptr;

    char *data = new char[metadata.ThumbnailSize];
    if (stream.read(data, metadata.ThumbnailSize) != metadata.ThumbnailSize)
    {
        delete[] data;
        return nullptr;
    }

    auto loader = Gx::ResourceLoaderFactory::GetLoader<sf::Texture>();
    if (!loader)
    {
        delete[] data;
        return nullptr;
    }

    auto texture = loader->LoadFromMemory(data, metadata.ThumbnailSize, ctx);
    delete[] data;

    return texture;
}

Gx::ResourcePtr<sf::Texture> O2ChartLoader::LoadCoverArt(sf::InputStream &stream, O2ChartMetadata &metadata, const Gx::ResourceContext &ctx)
{
    if (stream.seek(metadata.CoverOffset) == -1)
        return nullptr;

    char *data = new char[metadata.CoverSize];
    if (stream.read(data, metadata.CoverSize) != metadata.CoverSize)
    {
        delete[] data;
        return nullptr;
    }

    auto loader = Gx::ResourceLoaderFactory::GetLoader<sf::Texture>();
    if (!loader)
    {
        delete[] data;
        return nullptr;
    }

    auto texture = loader->LoadFromMemory(data, metadata.CoverSize, ctx);
    delete[] data;

    return texture;
}
