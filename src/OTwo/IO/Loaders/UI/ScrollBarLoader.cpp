#include <OTwo/IO/Loaders/UI/ScrollBarLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/Metadata/UI/ScrollBarMetadata.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

Gx::ResourcePtr<Gx::ScrollBar> ScrollBarLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    ScrollBarMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
        return nullptr;

    if (const auto orientation = attributes.find("orientation"); orientation != attributes.end())
    {
        if (orientation->get<std::string>() == "VERTICAL")
            metadata.Orientation = Gx::ScrollBar::ScrollOrientation::Vertical;
        else
            metadata.Orientation = Gx::ScrollBar::ScrollOrientation::Horizontal;
    }

    if (const auto maximum = attributes.find("maximum"); maximum != attributes.end())
        metadata.Maximum = maximum->get<float>();
    else
        metadata.Maximum = 100.0f;

    if (const auto step = attributes.find("step"); step != attributes.end())
        metadata.Step = step->get<float>();
    else
        metadata.Step = 1.0f;

    if (const auto bounds = attributes.find("bounds"); bounds != attributes.end())
    {
        unsigned int x, y, w, h;
        bounds->at("x").get_to(x);
        bounds->at("y").get_to(y);
        bounds->at("width").get_to(w);
        bounds->at("height").get_to(h);
        metadata.Bounds = sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(w, h));
    }

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Gx::ScrollBar> ScrollBarLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    const auto metadata = dynamic_cast<const ScrollBarMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible.");
    
    auto scrollBar = std::make_unique<Gx::ScrollBar>();
    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto texture = ctx.Find<sf::Texture>(*metadata); texture)
        scrollBar->SetTexture(*texture);
    
    scrollBar->SetName(metadata->Name);
    scrollBar->SetScrollOrientation(metadata->Orientation);
    scrollBar->SetMaximumValue(metadata->Maximum);
    scrollBar->SetStep(metadata->Step);
    scrollBar->SetLocalBounds(metadata->Bounds);
    scrollBar->SetTexCoords(metadata->TexCoords);
    scrollBar->SetColor(metadata->Color);
    scrollBar->SetOrigin(metadata->Origin);
    scrollBar->SetPosition(metadata->Position);
    scrollBar->SetScale(metadata->Scale);
    scrollBar->SetRotation(metadata->Rotation);

    auto populator = ObjectPopulator::Decorate(scrollBar.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(name, context);

            ObjectLoader::Load(name, object, populator, objectCtx);
        }
    }

    return scrollBar;
}
