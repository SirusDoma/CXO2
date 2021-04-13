#include <O2/Loaders/UI/ProgressBarLoader.hpp>

#include <O2/Loaders/SpriteLoader.hpp>
#include <O2/Metadata/UI/ProgressBarMetadata.hpp>

ProgressBarLoader::ProgressBarLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> ProgressBarLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    ProgressBarMetadata metadata;

    auto attributes = json.at("attributes");
    metadata.SetResourceType(json.at("type").get<std::string>());

    ProgressBarLoader::ParseReferences(json["require"], metadata);
    SpriteLoader::ParseSprite(attributes, metadata);

    auto orientation = attributes.find("orientation");
    if (orientation != attributes.end())
    {
        if (orientation->get<std::string>() == "VERTICAL")
            metadata.SetOrientation(Gx::ProgressBar::Vertical);
        else
            metadata.SetOrientation(Gx::ProgressBar::Horizontal);
    }

    auto maximum = attributes.find("maximum");
    if (maximum != attributes.end())
        metadata.SetMaximum(maximum->get<float>());
    else
        metadata.SetMaximum(100.0f);

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

    progressBar->SetTexCoords(spec->GetTexCoords());
    progressBar->SetOrientation(spec->GetOrientation());
    progressBar->SetMaximumValue(spec->GetMaximum());
    progressBar->SetColor(spec->GetColor());

    progressBar->SetOrigin(spec->GetOrigin());
    progressBar->SetPosition(spec->GetPosition());
    progressBar->SetScale(spec->GetScale());
    progressBar->SetRotation(spec->GetRotation());

    return progressBar;
}

