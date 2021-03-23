#include <O2/IO/Loaders/UI/RepeaterLoader.hpp>

RepeaterLoader::RepeaterLoader()
{
}

Gx::ResourceMetadata* RepeaterLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));
    RepeaterMetadata metadata;

    json.at("type").get_to(metadata.Type);
    auto attributes = json.at("attributes");
    TransformLoader::Parse(attributes["transform"], &metadata);

    auto vertical = attributes.find("vertical");
    if (vertical != attributes.end())
    {
        vertical->at("count").get_to(metadata.VerticalCount);
        vertical->at("spacing").get_to(metadata.VerticalSpacing);
    }
    else
    {
        metadata.VerticalCount = 1;
        metadata.VerticalSpacing = 0;
    }

    auto horizontal = attributes.find("horizontal");
    if (horizontal != attributes.end())
    {
        horizontal->at("count").get_to(metadata.HorizontalCount);
        horizontal->at("spacing").get_to(metadata.HorizontalSpacing);
    }
    else
    {
        metadata.HorizontalCount = 1;
        metadata.HorizontalSpacing = 0;
    }

    return new RepeaterMetadata(metadata);
}


Gx::Repeater* RepeaterLoader::Create(Gx::ResourceMetadata* metadata, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<RepeaterMetadata*>(metadata);
    if (!spec)
        return nullptr;

    auto repeater = new Gx::Repeater(
        spec->VerticalCount,   spec->VerticalSpacing,
        spec->HorizontalCount, spec->HorizontalSpacing
    );

    repeater->SetOrigin(spec->Origin);
    repeater->SetPosition(spec->Position);
    repeater->SetScale(spec->Scale);
    repeater->SetRotation(spec->Rotation);

    return repeater;
}
