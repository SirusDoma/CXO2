#include <OTwo/Loaders/SoundLoader.hpp>
#include <OTwo/Metadata/SoundMetadata.hpp>

SoundLoader::SoundLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> SoundLoader::LoadMetadata(const void* data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    SoundMetadata metadata;

    metadata.ResourceType = json.at("type").get<std::string>();
    SoundLoader::ParseReferences(json["require"], metadata);

    return std::make_unique<SoundMetadata>(metadata);
}

Gx::ResourcePtr<sf::Sound> SoundLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    if (!context.SoundBuffer)
        return nullptr;

    return std::make_unique<sf::Sound>(*context.SoundBuffer);
}
