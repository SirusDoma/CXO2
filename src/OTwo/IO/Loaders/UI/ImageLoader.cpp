#include <OTwo/IO/Loaders/UI/ImageLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>

#include <OTwo/Metadata/UI/ImageMetadata.hpp>
#include <OTwo/IO/ResourceContextDecorator.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

Gx::ResourcePtr<Gx::Image> ImageLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    ImageMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    const auto attributes = json.at("attributes");
    if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
        return nullptr;

    if (!ParseMetadata(attributes, metadata, context))
        return nullptr;

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Gx::Image> ImageLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    const auto metadata = dynamic_cast<const ImageMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible.");
    
    auto image = std::make_unique<Gx::Image>();
    const auto ctx = ResourceContextDecorator::Decorate(context);

    if (!metadata->Frames.empty())
    {
        for (const auto& frame : metadata->Frames)
            image->AddFrame(frame.first, frame.second);
    }
    else
        image->SetTexCoords(metadata->TexCoords);

    if (const auto texture = ctx.Find<sf::Texture>(*metadata); texture)
        image->SetTexture(*texture);

    image->SetName(metadata->Name);
    image->SetColor(metadata->Color);
    image->SetOrigin(metadata->Origin);
    image->SetPosition(metadata->Position);
    image->SetScale(metadata->Scale);
    image->SetRotation(metadata->Rotation);

    auto populator = ObjectPopulator::Decorate(image.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(name, context);

            ObjectLoader::Load(name, object, populator, objectCtx);
        }
    }

    return image;
}

bool ImageLoader::ParseMetadata(const Gx::Json &attributes, ImageMetadata &metadata, const Gx::ResourceContext &context)
{
    if (attributes.empty())
        return false;

    auto frames = attributes.find("frames");
    if (frames == attributes.end())
    {
        metadata.Frames.push_back({ "default", Gx::Image::Frame
            {
                metadata.TexCoords,
                metadata.Origin,
                metadata.Position,
                metadata.Rotation,
                metadata.Scale
            }
        });

        return true;
    }

    for (auto [frameName, frameAttr] : frames->items())
    {
        auto frame = Gx::Image::Frame();

        auto p = frameAttr.find("position");
        auto position = metadata.Position;
        if (p != frameAttr.end())
        {
            position = sf::Vector2f();
            p->at("x").get_to(position.x);
            p->at("y").get_to(position.y);
        }
        frame.Position = position;

        auto s = frameAttr.find("scale");
        auto scale = metadata.Scale;
        if (s != frameAttr.end())
        {
            scale = sf::Vector2f();
            s->at("scale").at("x").get_to(scale.x);
            s->at("scale").at("y").get_to(scale.y);
        }
        frame.Scale = scale;

        auto r = frameAttr.find("rotation");
        float rotation = metadata.Rotation;
        if (r != frameAttr.end())
            r->get_to(rotation);
        frame.Rotation = rotation;

        auto o = frameAttr.find("origin");
        auto origin = metadata.Origin;
        if (o != frameAttr.end())
        {
            o->at("x").get_to(origin.x);
            o->at("y").get_to(origin.y);
        }
        frame.Origin = origin;

        auto t = frameAttr.find("texCoords");
        auto texCoords = metadata.TexCoords;
        if (t != frameAttr.end())
        {
            unsigned int x, y, w, h;
            t->at("x").get_to(x);
            t->at("y").get_to(y);
            t->at("width").get_to(w);
            t->at("height").get_to(h);

            texCoords = sf::IntRect(sf::Vector2i(x, y), sf::Vector2i(w, h));
        }
        frame.TexCoords = texCoords;
        metadata.Frames.push_back({frameName, frame});
    }

    return true;
}
