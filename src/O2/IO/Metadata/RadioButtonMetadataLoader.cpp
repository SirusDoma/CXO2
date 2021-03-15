#include <O2/IO/Metadata/RadioButtonMetadataLoader.hpp>

#include <Genode/IO/ResourceLoaderFactory.hpp>

#include <O2/IO/Metadata/ButtonMetadataLoader.hpp>

RadioButtonMetadataLoader::RadioButtonMetadataLoader()
{
}

Gx::ResourceMetadata* RadioButtonMetadataLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));
    RadioButtonMetadata metadata;

    json.at("type").get_to(metadata.Type);

    auto resources = json.at("resources");
    for (auto resource : resources.items())
        metadata.ResourceReferences[resource.key()] = resource.value();

    std::unordered_map<std::string, Gx::Button::State> stateMap = {
        { "normal", Gx::Button::State::Normal },
        { "hover", Gx::Button::State::Hover },
        { "check", Gx::Button::State::Active },
    };

    ButtonMetadataLoader::Parse(json.at("attributes"), stateMap, &metadata);
    return new RadioButtonMetadata(metadata);
}

Gx::RadioButton* RadioButtonMetadataLoader::Create(Gx::ResourceMetadata* metadata, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<RadioButtonMetadata*>(metadata);
    if (!spec)
        return nullptr;

    auto radio = new Gx::RadioButton(context.Texture);
    radio->SetOrigin(spec->Origin);
    radio->SetPosition(spec->Position);
    radio->SetScale(spec->Scale);
    radio->SetRotation(spec->Rotation);

    auto loader = Gx::ResourceLoaderFactory::GetMetadataLoader<Gx::Sprite>();
    if (loader)
    {
        for (auto[state, meta] : spec->States)
            radio->SetStateFrame(state, *loader->Create(&meta, Gx::ResourceContext()));
    }

    return radio;
}

