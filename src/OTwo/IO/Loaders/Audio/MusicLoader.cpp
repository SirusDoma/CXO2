#include <OTwo/IO/Loaders/Audio/MusicLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/Metadata/Audio/MusicMetadata.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/IO/FileSystem/FileSystem.hpp>

Gx::ResourcePtr<sf::Music> MusicLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    MusicMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto it = metadata.Require.find("music");;
    if (it != metadata.Require.end())
    {
        auto source = std::any_cast<Gx::Json>(it->second);
        metadata.Source = source.get<std::string>();
    }

    auto attributes = json.at("attributes");
    metadata.IsLoop = attributes["loop"].get<bool>();

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<sf::Music> MusicLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    auto metadata = dynamic_cast<const MusicMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible.");
    
    auto size = Gx::FileSystem::GetFileSize(metadata->Source);
    auto data = new Gx::Uint8[size];
    if (Gx::FileSystem::ReadFile(metadata->Source, data, size))
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

        music->setLoop(metadata->IsLoop);
        return music;
    }

    return nullptr;
}

bool MusicLoader::IsStreaming() const
{
    return true;
}
