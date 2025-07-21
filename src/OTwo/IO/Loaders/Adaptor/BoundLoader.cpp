#include <OTwo/IO/Loaders/Adaptor/BoundLoader.hpp>
#include <Genode/IO/FileSystem/FileSystem.hpp>

#include <SFML/System/MemoryInputStream.hpp>

Gx::ResourcePtr<std::vector<ControlList::Bound>> BoundLoader::LoadFromFile(const std::string& fileName, const Gx::ResourceContext& ctx) const
{
    const auto stream = Gx::FileSystem::Open(fileName);
    if (!stream)
        throw Gx::ResourceLoadException("Failed to open the file: " + fileName);

    auto& inputStream = *stream.get();
    return LoadFromStream(inputStream, ctx);
}

Gx::ResourcePtr<std::vector<ControlList::Bound>> BoundLoader::LoadFromMemory(void* data, std::size_t size, const Gx::ResourceContext& ctx) const
{
    auto stream = sf::MemoryInputStream(data, size);
    return LoadFromStream(stream, ctx);
}

Gx::ResourcePtr<std::vector<ControlList::Bound>> BoundLoader::LoadFromStream(sf::InputStream& stream, const Gx::ResourceContext& ctx) const
{
    static_assert(std::is_trivially_copyable_v<ControlList::Bound>, "Bound is not safe-parsable: not trivially copyable");

    auto bounds = std::vector<ControlList::Bound>();

    std::uint32_t signature;
    if (stream.read(&signature, sizeof(signature)) != sizeof(signature))
        throw Gx::ResourceLoadException("Failed to load bound file");

    std::uint16_t count;
    if (stream.read(&count, sizeof(count)) != sizeof(count))
        throw Gx::ResourceLoadException("Failed to load bound file");

    for (std::size_t i = 0; i < count; i++)
    {
        auto bound = ControlList::Bound();
        if (stream.read(&bound, sizeof(bound)) != sizeof(bound))
            throw Gx::ResourceLoadException("Failed to load bound file");

        bounds.push_back(bound);
    }

    return std::make_unique<std::vector<ControlList::Bound>>(bounds);
}
