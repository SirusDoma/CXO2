#include <O2/IO/Metadata/UI/ProgressBarMetadataLoader.hpp>

#include <O2/IO/Metadata/SpriteMetadataLoader.hpp>

ProgressBarMetadataLoader::ProgressBarMetadataLoader()
{
}

Gx::ResourceMetadata* ProgressBarMetadataLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));
    ProgressBarMetadata metadata;

    json.at("type").get_to(metadata.Type);

    auto resources = json.at("resources");
    for (auto resource : resources.items())
        metadata.ResourceReferences[resource.key()] = resource.value();

    auto attributes = json.at("attributes");
    SpriteMetadataLoader::Parse(attributes, &metadata);

    auto orientation = attributes.find("orientation");
    if (orientation != attributes.end())
    {
        std::string result;
        orientation->get_to(result);
        if (result == "vertical")
            metadata.Orientation = Gx::ProgressBar::Vertical;
        else
            metadata.Orientation = Gx::ProgressBar::Horizontal;
    }

    auto maximum = attributes.find("maximum");
    if (maximum != attributes.end())
        maximum->get_to(metadata.Maxiumum);
    else
        metadata.Maxiumum = 100.0f;

    return new ProgressBarMetadata(metadata);
}

Gx::ProgressBar* ProgressBarMetadataLoader::Create(Gx::ResourceMetadata* metadata, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<ProgressBarMetadata*>(metadata);
    if (!spec)
        return nullptr;

    auto progressBar = new Gx::ProgressBar(context.Texture, spec->TexCoords);
    progressBar->SetOrientation(spec->Orientation);
    progressBar->SetMaximumValue(spec->Maxiumum);
    progressBar->SetColor(spec->Color);

    progressBar->SetOrigin(spec->Origin);
    progressBar->SetPosition(spec->Position);
    progressBar->SetScale(spec->Scale);
    progressBar->SetRotation(spec->Rotation);

    return progressBar;
}
