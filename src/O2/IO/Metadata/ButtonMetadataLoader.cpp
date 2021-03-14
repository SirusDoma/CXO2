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

    auto attributes = json.at("attributes");
    SpriteMetadataLoader::Parse(attributes, &metadata);

    auto states = attributes.at("states");
    std::unordered_map<std::string, Gx::Button::State> stateMap = {
        { "normal", Gx::Button::State::Normal },
        { "hover", Gx::Button::State::Hover },
        { "click", Gx::Button::State::Pressed },
    };

    metadata.States = std::unordered_map<Gx::Button::State, SpriteMetadata>();
    for (auto [name, state] : stateMap)
    {
        if (!states.contains(name))
            continue;

        SpriteMetadata stateMeta;
        SpriteMetadataLoader::Parse(states.at(name), &stateMeta);

        metadata.States[state] = stateMeta;
    }

    return new ButtonMetadata(metadata);
}

Gx::Button* ButtonMetadataLoader::Create(Gx::ResourceMetadata* metadata, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<ButtonMetadata*>(metadata);
    if (!spec)
        return nullptr;

    auto button = new Gx::Button(context.Texture);
    button->SetPosition(spec->Position);
    button->SetScale(spec->Scale);
    button->SetRotation(spec->Rotation);
    button->SetOrigin(spec->Origin);

    auto loader = Gx::ResourceLoaderFactory::GetMetadataLoader<Gx::Sprite>();
    if (loader)
    {
        for (auto[state, meta] : spec->States)
            button->SetStateFrame(state, *loader->Create(&meta, Gx::ResourceContext()));
    }

    return button;
}