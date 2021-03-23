#include <O2/IO/Loaders/MusicLoader.hpp>
#include <Genode/IO/ResourceManager.hpp>

MusicLoader::MusicLoader()
{
}

Gx::ResourceMetadata* MusicLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));
    MusicMetadata definition;

    json.at("type").get_to(definition.Type);

    auto resources = json.at("resources");
    for (auto resource : resources.items())
        definition.ResourceReferences[resource.key()] = resource.value();

    auto attributes = json.at("attributes");
    attributes.at("loop").get_to(definition.Loop);

    return new MusicMetadata(definition);
}

sf::Music* MusicLoader::Create(Gx::ResourceMetadata* definition, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<MusicMetadata*>(definition);
    if (!spec)
        return nullptr;

    Gx::Uint8 *data;
    auto size = Gx::ResourceManager::Instance()->GetResourceData(definition->ResourceReferences["music"], &data);

    auto music = new sf::Music();
    if (!music->openFromMemory(data, size))
        return nullptr;

    music->setLoop(spec->Loop);
    return music;
}