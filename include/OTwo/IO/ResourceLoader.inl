#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/IO/ResourceContextDecorator.hpp>

#include <Genode/IO/FileSystem/FileSystem.hpp>

#include <SFML/System/MemoryInputStream.hpp>

#include <string>

template<typename R>
Gx::ResourcePtr<R> ResourceLoader<R>::LoadFromFile(const std::string &fileName, const Gx::ResourceContext &ctx) const
{
    if (ctx.GetCacheMode() == Gx::CacheMode::Reuse)
    {
        if (auto metadata = ctx.Find<ResourceMetadata>(ctx.GetID()); metadata)
            return LoadFromMetadata(*metadata, ctx);
    }

    auto stream = Gx::FileSystem::Open(fileName);
    if (!stream)
        throw Gx::ResourceLoadException("Failed to open the file: " + fileName);

    auto& inputStream = *stream.get();
    return LoadFromStream(inputStream, ctx);
}

template<typename R>
Gx::ResourcePtr<R> ResourceLoader<R>::LoadFromMemory(void *data, std::size_t size, const Gx::ResourceContext &ctx) const
{
    if (ctx.GetCacheMode() == Gx::CacheMode::Reuse)
    {
        if (auto metadata = ctx.Find<ResourceMetadata>(ctx.GetID()); metadata)
            return LoadFromMetadata(*metadata, ctx);
    }

    auto stream = sf::MemoryInputStream();
    stream.open(data, size);

    return LoadFromStream(stream, ctx);
}

template<typename R>
Gx::ResourcePtr<R> ResourceLoader<R>::LoadFromStream(sf::InputStream &stream, const Gx::ResourceContext &ctx) const
{
    if (ctx.GetCacheMode() == Gx::CacheMode::Reuse)
    {
        if (auto metadata = ctx.Find<ResourceMetadata>(ctx.GetID()); metadata)
            return LoadFromMetadata(*metadata, ctx);
    }

    auto size = stream.getSize() - stream.tell();
    auto data = new Gx::Uint8[size];
    if (stream.read(data, size) == -1)
    {
        delete[] data;
        throw Gx::ResourceLoadException("Failed to load the resource.");
    }

    auto json = Gx::Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    delete[] data;

    return LoadFromJson(json, ctx);
}
