#include <O2/Loaders/MusicLoader.hpp>
#include <O2/Metadata/MusicMetadata.hpp>

#include <Genode/IO/ResourceManager.hpp>

MusicLoader::MusicLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> MusicLoader::LoadMetadata(const void* data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    MusicMetadata metadata;

    metadata.ResourceType = json.at("type").get<std::string>();
    for (auto resource : json["require"].items())
    {
        if (resource.key() == "music")
        {
            metadata.Source = resource.value();
            break;
        }
    }

    auto attributes = json.at("attributes");
    metadata.IsLoop = attributes["loop"].get<bool>();

    return std::make_unique<MusicMetadata>(metadata);
}

Gx::ResourcePtr<sf::Music> MusicLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const MusicMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    Gx::Uint8 *data;
    if (auto size = context.Resources->GetResourceData(spec->Source, &data))
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

        music->setLoop(spec->IsLoop);
        return music;
    }

    return nullptr;
}
