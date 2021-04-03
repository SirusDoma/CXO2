#include <O2/IO/Loaders/MusicLoader.hpp>
#include <O2/IO/Metadata/MusicMetadata.hpp>

MusicLoader::MusicLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> MusicLoader::LoadMetadata(const void* data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    MusicMetadata metadata;

    metadata.SetType(json.at("type").get<std::string>());
    for (auto resource : json["require"].items())
    {
        if (resource.key() == "music")
        {
            metadata.SetSource(resource.value());
            break;
        }
    }

    auto attributes = json.at("attributes");
    metadata.SetLoop(attributes.at("loop").get<bool>());

    return std::make_unique<MusicMetadata>(metadata);
}

Gx::ResourcePtr<sf::Music> MusicLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const MusicMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    Gx::Uint8 *data;
    if (auto size = context.Resources->GetResourceData(spec->GetSource(), &data))
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

        music->setLoop(spec->IsLoop());
        return music;
    }

    return nullptr;
}
