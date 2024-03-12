#include <OTwo/IO/Loaders/UI/ProgressBarLoader.hpp>

#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/Metadata/UI/ProgressBarMetadata.hpp>
#include <OTwo/IO/ResourceContextDecorator.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

Gx::ResourcePtr<Gx::ProgressBar> ProgressBarLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    ProgressBarMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
        return nullptr;

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

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Gx::ProgressBar> ProgressBarLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    auto metadata = dynamic_cast<const ProgressBarMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible.");

    auto progressBar = std::make_unique<Gx::ProgressBar>();
    auto ctx = ResourceContextDecorator::Decorate(context);
    if (auto texture = ctx.Find<sf::Texture>(*metadata); texture)
        progressBar->SetTexture(*texture);
    
    progressBar->SetName(metadata->Name);
    progressBar->SetOrientation(metadata->Orientation);
    progressBar->SetMaximumValue(metadata->Maximum);
    progressBar->SetTexCoords(metadata->TexCoords);
    progressBar->SetColor(metadata->Color);
    progressBar->SetOrigin(metadata->Origin);
    progressBar->SetPosition(metadata->Position);
    progressBar->SetScale(metadata->Scale);
    progressBar->SetRotation(metadata->Rotation);

    auto populator = ObjectPopulator::Decorate(progressBar.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(name, context);

            ObjectLoader::Load(name, object, populator, objectCtx);
        }
    }

    return progressBar;
}
