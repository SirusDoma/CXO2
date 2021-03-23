#include <O2/IO/Loaders/SoundLoader.hpp>
#include <Genode/IO/ResourceManager.hpp>

SoundLoader::SoundLoader()
{
}

Gx::ResourceMetadata* SoundLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));
    SoundMetadata definition;

    json.at("type").get_to(definition.Type);

    auto resources = json.at("resources");
    for (auto resource : resources.items())
        definition.ResourceReferences[resource.key()] = resource.value();

    return new SoundMetadata(definition);
}

sf::Sound* SoundLoader::Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<SoundMetadata*>(definition);
    if (!spec)
        return nullptr;

    return new sf::Sound(*context.SoundBuffer);
}