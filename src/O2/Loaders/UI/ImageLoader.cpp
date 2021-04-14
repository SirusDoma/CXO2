#include <O2/Loaders/UI/ImageLoader.hpp>

#include <O2/Loaders/SpriteLoader.hpp>
#include <O2/Metadata/UI/ImageMetadata.hpp>

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
    SpriteLoader::ParseSprite(attributes, metadata);

    if (attributes.contains("frames"))
    {
        auto frames = attributes.at("frames");
        for (auto [frameName, frame] : frames.items())
        {
            unsigned int x, y, w, h;
            frame.at("x").get_to(x);
            frame.at("y").get_to(y);
            frame.at("width").get_to(w);
            frame.at("height").get_to(h);
            metadata.Frames[frameName] = sf::IntRect(x, y, w, h);
        }
    }
    else
        metadata.Frames["default"] = metadata.TexCoords;

    return std::make_unique<ImageMetadata>(metadata);
}

Gx::ResourcePtr<Gx::Image> ImageLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const ImageMetadata*>(&metadata);
    if (!spec)
        return nullptr;

    auto image = std::make_unique<Gx::Image>();
    image->SetName(context.Name);
    image->SetTexCoords(spec->TexCoords);
    image->SetColor(spec->Color);

    if (context.Texture)
        image->SetTexture(*context.Texture);

    for (auto frame : spec->Frames)
        image->AddFrame(frame.first, frame.second);

    image->SetOrigin(spec->Origin);
    image->SetPosition(spec->Position);
    image->SetScale(spec->Scale);
    image->SetRotation(spec->Rotation);

    return image;
}
