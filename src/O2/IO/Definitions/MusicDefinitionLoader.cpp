#include <O2/IO/Definitions/MusicDefinitionLoader.hpp>
#include <Genode/IO/ResourceManager.hpp>

MusicDefinitionLoader::MusicDefinitionLoader()
{
}

Gx::ResourceDefinition* MusicDefinitionLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));
    MusicDefinition definition;

    json.at("type").get_to(definition.Type);

    auto resources = json.at("resources");
    for (auto resource : resources.items())
        definition.ResourceReferences[resource.key()] = resource.value();

    auto attributes = json.at("attributes");
    attributes.at("loop").get_to(definition.Loop);

    return new MusicDefinition(definition);
}

sf::Music* MusicDefinitionLoader::Create(Gx::ResourceDefinition* definition, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<MusicDefinition*>(definition);
    if (!spec)
        return nullptr;

    auto music = new sf::Music();

    Gx::Uint8 *data;
    auto size = Gx::ResourceManager::Instance()->GetResourceData(definition->ResourceReferences["music"], &data);

    if (!music->openFromMemory(data, size))
        return nullptr;

    music->setLoop(spec->Loop);
    return music;
}