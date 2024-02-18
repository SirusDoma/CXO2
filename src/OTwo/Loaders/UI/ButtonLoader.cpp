#include <OTwo/Loaders/UI/ButtonLoader.hpp>
#include <OTwo/Loaders/SpriteLoader.hpp>

ButtonLoader::ButtonLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> ButtonLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    auto metadata = ButtonMetadata();

    metadata.ResourceType = json.at("type").get<std::string>();
    ParseReferences(json["require"], metadata);

    std::unordered_map<std::string, Gx::Button::State> stateMap = {
        { "normal", Gx::Button::State::Normal },
        { "hover", Gx::Button::State::Hover },
        { "click", Gx::Button::State::Active },
    };

    ParseButton(json["attributes"], stateMap, metadata);
    return std::make_unique<ButtonMetadata>(metadata);
}

Gx::ResourcePtr<Gx::Button> ButtonLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const ButtonMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto button = std::make_unique<Gx::Button>();
    if (context.Texture)
        button->SetTexture(*context.Texture);

    button->SetName(context.Name);
    button->SetOrigin(spec->Origin);
    button->SetPosition(spec->Position);
    button->SetScale(spec->Scale);
    button->SetRotation(spec->Rotation);

    auto loader = Gx::ResourceLoaderFactory::GetLoader<Gx::Sprite>();
    if (loader)
    {
        for (auto[state, meta] : spec->States)
            button->SetStateFrame(state, *loader->Load(meta, Gx::ResourceContext()));
    }

    return button;
}

void ButtonLoader::ParseButton(Json attributes, std::unordered_map<std::string, Gx::Button::State> stateMap, ButtonMetadata &metadata)
{
    SpriteLoader::ParseSprite(attributes, metadata);

    auto states = attributes.at("states");
    for (auto [name, state] : stateMap)
    {
        if (!states.contains(name))
            continue;

        SpriteMetadata stateMeta;
        SpriteLoader::ParseSprite(states.at(name), stateMeta);

        metadata.States[state] = stateMeta;
    }
}
