#include <O2/IO/Loaders/UI/RepeaterLoader.hpp>

#include <O2/IO/Loaders/TransformLoader.hpp>
#include <O2/IO/Metadata/UI/RepeaterMetadata.hpp>

RepeaterLoader::RepeaterLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> RepeaterLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    RepeaterMetadata metadata;

    metadata.SetType(json.at("type").get<std::string>());
    auto attributes = json.at("attributes");

    ParseReferences(json["require"], metadata);
    TransformLoader::ParseTransform(attributes["transform"], metadata);

    auto vertical = attributes.find("vertical");
    if (vertical != attributes.end())
    {
        metadata.SetVerticalCount(vertical->at("count").get<unsigned int>());
        metadata.SetVerticalSpacing(vertical->at("spacing").get<float>());
    }
    else
    {
        metadata.SetVerticalCount(1);
        metadata.SetVerticalSpacing(0.f);
    }

    auto horizontal = attributes.find("horizontal");
    if (horizontal != attributes.end())
    {
        metadata.SetHorizontalCount(horizontal->at("count").get<unsigned int>());
        metadata.SetHorizontalSpacing(horizontal->at("spacing").get<float>());
    }
    else
    {
        metadata.SetHorizontalCount(1);
        metadata.SetHorizontalSpacing(0.f);
    }

    return std::make_unique<RepeaterMetadata>(metadata);
}

Gx::ResourcePtr<Gx::Repeater> RepeaterLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const RepeaterMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto repeater = std::make_unique<Gx::Repeater>(
        spec->GetVerticalCount(),   spec->GetVerticalSpacing(),
        spec->GetHorizontalCount(), spec->GetHorizontalSpacing()
    );

    repeater->SetName(context.Name);
    repeater->SetOrigin(spec->GetOrigin());
    repeater->SetPosition(spec->GetPosition());
    repeater->SetScale(spec->GetScale());
    repeater->SetRotation(spec->GetRotation());

    return repeater;
}

