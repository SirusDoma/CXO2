#include <O2/IO/Metadata/UI/ImageMetadataLoader.hpp>
#include <O2/IO/Metadata/SpriteMetadataLoader.hpp>

ImageMetadataLoader::ImageMetadataLoader()
{
}

Gx::ResourceMetadata* ImageMetadataLoader::Load(Gx::Uint8* data, Gx::Uint64 size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<char*>(data), size));
    ImageMetadata metadata;

    json.at("type").get_to(metadata.Type);

    auto resources = json.at("resources");
    for (auto resource : resources.items())
        metadata.ResourceReferences[resource.key()] = resource.value();

    auto attributes = json.at("attributes");
    SpriteMetadataLoader::Parse(attributes, &metadata);

    metadata.Frames = std::unordered_map<std::string, sf::IntRect>();
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

    return new ImageMetadata(metadata);
}

Gx::Image* ImageMetadataLoader::Create(Gx::ResourceMetadata* metadata, Gx::ResourceContext context) const
{
    auto spec = dynamic_cast<ImageMetadata*>(metadata);
    if (!spec)
        return nullptr;

    auto image = new Gx::Image();
    image->SetName(spec->Name);
    image->SetTexCoords(spec->TexCoords);
    image->SetColor(spec->Color);

    if (context.Texture)
        image->SetTexture(context.Texture);

    for (auto frame : spec->Frames)
        image->AddFrame(frame.first, frame.second);

    image->SetOrigin(spec->Origin);
    image->SetPosition(spec->Position);
    image->SetScale(spec->Scale);
    image->SetRotation(spec->Rotation);

    return image;
}
