#include <OTwo/Loaders/UI/ImageLoader.hpp>

#include <OTwo/Loaders/SpriteLoader.hpp>
#include <OTwo/Metadata/UI/ImageMetadata.hpp>

ImageLoader::ImageLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> ImageLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    ImageMetadata metadata;

    metadata.ResourceType = json.at("type").get<std::string>();
    auto attributes = json.at("attributes");

    ParseReferences(json["require"], metadata);
    ParseImage(attributes, metadata);

    return std::make_unique<ImageMetadata>(metadata);
}

Gx::ResourcePtr<Gx::Image> ImageLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const ImageMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto image = std::make_unique<Gx::Image>();
    image->SetName(context.Name);
    image->SetColor(spec->Color);

    if (spec->Frames.size() > 0)
    {
        for (auto frame : spec->Frames)
            image->AddFrame(frame.first, frame.second);
    }
    else
        image->SetTexCoords(spec->TexCoords);

    if (context.Texture)
        image->SetTexture(*context.Texture);

    image->SetOrigin(spec->Origin);
    image->SetPosition(spec->Position);
    image->SetScale(spec->Scale);
    image->SetRotation(spec->Rotation);

    return image;
}

void ImageLoader::ParseImage(Json attributes, ImageMetadata &metadata)
{
    SpriteLoader::ParseSprite(attributes, metadata);
    if (attributes.contains("frames"))
    {
        auto frames = attributes.at("frames");
        for (auto [frameName, frameAttr] : frames.items())
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

                texCoords = sf::IntRect(x, y, w, h);
            }
            frame.TexCoords = texCoords;

            metadata.Frames[frameName] = frame;
        }
    }
    else
    {
        metadata.Frames["default"] = Gx::Image::Frame
        {
            metadata.TexCoords,
            metadata.Origin,
            metadata.Position,
            metadata.Rotation, metadata.Scale
        };
    }
}
