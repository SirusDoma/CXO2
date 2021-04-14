#include <O2/Loaders/UI/CheckBoxLoader.hpp>

#include <O2/Loaders/UI/ButtonLoader.hpp>
#include <O2/Metadata/UI/CheckBoxMetadata.hpp>

CheckBoxLoader::CheckBoxLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> CheckBoxLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    auto metadata = CheckBoxMetadata();

    metadata.ResourceType = json.at("type").get<std::string>();
    ParseReferences(json["require"], metadata);

    std::unordered_map<std::string, Gx::Button::State> stateMap = {
        { "normal", Gx::Button::State::Normal },
        { "hover", Gx::Button::State::Hover },
        { "check", Gx::Button::State::Active },
    };

    ButtonLoader::ParseButton(json["attributes"], stateMap, metadata);
    return std::make_unique<CheckBoxMetadata>(metadata);
}

Gx::ResourcePtr<Gx::CheckBox> CheckBoxLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const CheckBoxMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto checkBox = std::make_unique<Gx::CheckBox>();
    if (context.Texture)
        checkBox->SetTexture(*context.Texture);

    checkBox->SetName(context.Name);
    checkBox->SetOrigin(spec->Origin);
    checkBox->SetPosition(spec->Position);
    checkBox->SetScale(spec->Scale);
    checkBox->SetRotation(spec->Rotation);

    auto loader = Gx::ResourceLoaderFactory::GetLoader<Gx::Sprite>();
    if (loader)
    {
        for (auto[state, meta] : spec->States)
            checkBox->SetStateFrame(state, *loader->Load(meta, Gx::ResourceContext()));
    }

    return checkBox;
}


