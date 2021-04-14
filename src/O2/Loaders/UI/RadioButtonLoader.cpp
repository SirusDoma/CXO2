#include <O2/Loaders/UI/RadioButtonLoader.hpp>

#include <O2/Loaders/UI/ButtonLoader.hpp>
#include <O2/Metadata/UI/RadioButtonMetadata.hpp>

RadioButtonLoader::RadioButtonLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> RadioButtonLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    auto metadata = RadioButtonMetadata();

    metadata.ResourceType = json.at("type").get<std::string>();
    ParseReferences(json["require"], metadata);

    std::unordered_map<std::string, Gx::Button::State> stateMap = {
        { "normal", Gx::Button::State::Normal },
        { "hover", Gx::Button::State::Hover },
        { "check", Gx::Button::State::Active },
    };

    ButtonLoader::ParseButton(json["attributes"], stateMap, metadata);
    return std::make_unique<RadioButtonMetadata>(metadata);
}

Gx::ResourcePtr<Gx::RadioButton> RadioButtonLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const RadioButtonMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto radio = std::make_unique<Gx::RadioButton>();
    if (context.Texture)
        radio->SetTexture(*context.Texture);

    radio->SetName(context.Name);
    radio->SetOrigin(spec->Origin);
    radio->SetPosition(spec->Position);
    radio->SetScale(spec->Scale);
    radio->SetRotation(spec->Rotation);

    auto loader = Gx::ResourceLoaderFactory::GetLoader<Gx::Sprite>();
    if (loader)
    {
        for (auto[state, meta] : spec->States)
            radio->SetStateFrame(state, *loader->Load(meta, Gx::ResourceContext()));
    }

    return radio;
}


