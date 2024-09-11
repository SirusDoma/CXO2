#include <OTwo/IO/Loaders/Audio/MusicLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/Metadata/Audio/MusicMetadata.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/IO/FileSystem/FileSystem.hpp>
#include <Genode/Utilities/StringHelper.hpp>

Gx::ResourcePtr<sf::Music> MusicLoader::LoadFromFile(const std::string &fileName, const Gx::ResourceContext &ctx) const
{
    if (Gx::StringHelper::IsGlobMatch(fileName, "*.json"))
        return ResourceLoader::LoadFromFile(fileName, ctx);

    auto metadata = MusicMetadata();
    metadata.Source = fileName;

    return LoadFromMetadata(metadata, ctx);
}

Gx::ResourcePtr<sf::Music> MusicLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    MusicMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    if (const auto it = metadata.Require.find("music"); it != metadata.Require.end())
    {
        const auto source = std::any_cast<Gx::Json>(it->second);
        metadata.Source = source.get<std::string>();
    }

    auto attributes = json.at("attributes");
    metadata.IsLoop = attributes["loop"].get<bool>();

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<sf::Music> MusicLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    const auto metadata = dynamic_cast<const MusicMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    // "data" must be a pointer (or reference to somewhere else) because it need to be alive outside this method
    const auto size = Gx::FileSystem::GetFileSize(metadata->Source);
    if (auto data = new Gx::Uint8[size]; Gx::FileSystem::ReadFile(metadata->Source, data, size))
    {
        auto music = Gx::ResourcePtr<sf::Music>(new sf::Music(), [data] (auto music) {
            delete music;
            delete[] data;
        });

        if (!music->openFromMemory(data, size))
        {
            delete[] data;
            return nullptr;
        }

        music->setLooping(metadata->IsLoop);
        return music;
    }

    return nullptr;
}

bool MusicLoader::IsStreaming() const
{
    return true;
}
