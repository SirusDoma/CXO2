#include <OTwo/Loaders/UI/ProgressBarLoader.hpp>

#include <OTwo/Loaders/SpriteLoader.hpp>
#include <OTwo/Metadata/UI/ProgressBarMetadata.hpp>

ProgressBarLoader::ProgressBarLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> ProgressBarLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    ProgressBarMetadata metadata;

    auto attributes = json.at("attributes");
    metadata.ResourceType = json.at("type").get<std::string>();

    ProgressBarLoader::ParseReferences(json["require"], metadata);
    SpriteLoader::ParseSprite(attributes, metadata);

    auto orientation = attributes.find("orientation");
    if (orientation != attributes.end())
    {
        if (orientation->get<std::string>() == "VERTICAL")
            metadata.Orientation = Gx::ProgressBar::Vertical;
        else
            metadata.Orientation = Gx::ProgressBar::Horizontal;
    }

    auto maximum = attributes.find("maximum");
    if (maximum != attributes.end())
        metadata.Maximum = maximum->get<float>();
    else
        metadata.Maximum = 100.0f;

    return std::make_unique<ProgressBarMetadata>(metadata);
}

Gx::ResourcePtr<Gx::ProgressBar> ProgressBarLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const ProgressBarMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto progressBar = std::make_unique<Gx::ProgressBar>();
    if (context.Texture)
        progressBar->SetTexture(*context.Texture);

    progressBar->SetTexCoords(spec->TexCoords);
    progressBar->SetOrientation(spec->Orientation);
    progressBar->SetMaximumValue(spec->Maximum);
    progressBar->SetColor(spec->Color);

    progressBar->SetOrigin(spec->Origin);
    progressBar->SetPosition(spec->Position);
    progressBar->SetScale(spec->Scale);
    progressBar->SetRotation(spec->Rotation);

    return progressBar;
}

