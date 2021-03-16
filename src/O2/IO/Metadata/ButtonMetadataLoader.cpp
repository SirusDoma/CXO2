#include <O2/IO/Metadata/ButtonMetadataLoader.hpp>

#include <Genode/IO/ResourceLoaderFactory.hpp>

#include <O2/IO/Metadata/SpriteMetadataLoader.hpp>

ButtonMetadataLoader::ButtonMetadataLoader()
{
}

Gx::ResourceMetadata* ButtonMetadataLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));
    ButtonMetadata metadata;

    json.at("type").get_to(metadata.Type);

    auto resources = json.at("resources");
    for (auto resource : resources.items())
        metadata.ResourceReferences[resource.key()] = resource.value();

    std::unordered_map<std::string, Gx::Button::State> stateMap = {
        { "normal", Gx::Button::State::Normal },
        { "hover", Gx::Button::State::Hover },
        { "click", Gx::Button::State::Active },
    };

    Parse(json.at("attributes"), stateMap, &metadata);
    return new ButtonMetadata(metadata);
}

void ButtonMetadataLoader::Parse(Json attributes, std::unordered_map<std::string, Gx::Button::State> stateMap, ButtonMetadata *metadata)
{
    SpriteMetadataLoader::Parse(attributes, metadata);

    auto states = attributes.at("states");
    metadata->States = std::unordered_map<Gx::Button::State, SpriteMetadata>();
    for (auto [name, state] : stateMap)
    {
        if (!states.contains(name))
            continue;

        SpriteMetadata stateMeta;
        SpriteMetadataLoader::Parse(states.at(name), &stateMeta);

        metadata->States[state] = stateMeta;
    }
}

Gx::Button* ButtonMetadataLoader::Create(Gx::ResourceMetadata* metadata, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<ButtonMetadata*>(metadata);
    if (!spec)
        return nullptr;

    auto button = new Gx::Button(context.Texture);
    button->SetName(spec->Name);
    button->SetOrigin(spec->Origin);
    button->SetPosition(spec->Position);
    button->SetScale(spec->Scale);
    button->SetRotation(spec->Rotation);

    auto loader = Gx::ResourceLoaderFactory::GetMetadataLoader<Gx::Sprite>();
    if (loader)
    {
        for (auto[state, meta] : spec->States)
            button->SetStateFrame(state, *loader->Create(&meta, Gx::ResourceContext()));
    }

    return button;
}