#include <O2/IO/Loaders/UI/LabelLoader.hpp>

#include <O2/IO/Metadata/UI/LabelMetadata.hpp>
#include <O2/IO/Loaders/TransformLoader.hpp>

LabelLoader::LabelLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> LabelLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    LabelMetadata metadata;

    metadata.SetType(json.at("type").get<std::string>());

    auto attributes = json.at("attributes");
    ParseReferences(json["require"], metadata);
    ParseLabel(attributes, metadata);

    return std::make_unique<LabelMetadata>(metadata);
}

Gx::ResourcePtr<Gx::Label> LabelLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const LabelMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto label = std::make_unique<Gx::Label>();
    if (context.Font)
        label->SetFont(*context.Font);

    label->SetCharacterSize(spec->GetFontSize());
    label->SetString(spec->GetString());

    label->SetOrigin(spec->GetOrigin());
    label->SetPosition(spec->GetPosition());
    label->SetScale(spec->GetScale());
    label->SetRotation(spec->GetRotation());

    return label;
}

void LabelLoader::ParseLabel(Json attributes, LabelMetadata &metadata)
{
    TransformLoader::ParseTransform(attributes["transform"], metadata);

    auto fontSize = attributes.find("fontSize");
    if (fontSize != attributes.end())
        metadata.SetFontSize(fontSize->get<unsigned int>());
    else
        metadata.SetFontSize(30);

    auto string = attributes.find("string");
    if (string != attributes.end())
        metadata.SetString(string->get<std::string>());
}
