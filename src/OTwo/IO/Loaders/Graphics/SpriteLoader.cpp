#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>

#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>

#include <OTwo/IO/ResourceContextDecorator.hpp>
#include <OTwo/Metadata/Graphics/SpriteMetadata.hpp>

#include <magic_enum.hpp>

Gx::ResourcePtr<Gx::Sprite> SpriteLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    SpriteMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    if (!ParseMetadata(json.at("attributes"), metadata, context))
        return nullptr;

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Gx::Sprite> SpriteLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    auto ctx = ResourceContextDecorator::Decorate(context);
    auto metadata = dynamic_cast<const SpriteMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible.");

    auto sprite = std::make_unique<Gx::Sprite>();
    if (auto texture = ctx.Find<sf::Texture>(*metadata); texture)
        sprite->SetTexture(*texture);

    sprite->SetName(metadata->Name);
    sprite->SetBlendMode(metadata->BlendMode);
    sprite->SetTexCoords(metadata->TexCoords);
    sprite->SetColor(metadata->Color);
    sprite->SetOrigin(metadata->Origin);
    sprite->SetPosition(metadata->Position);
    sprite->SetScale(metadata->Scale);
    sprite->SetRotation(metadata->Rotation);

    return sprite;
}

bool SpriteLoader::ParseMetadata(Gx::Json attributes, SpriteMetadata &metadata, const Gx::ResourceContext &ctx)
{
    if (attributes.empty())
        return false;

    if (auto transform = attributes.find("transform"); transform != attributes.end())
        TransformLoader::ParseMetadata(transform.value(), metadata, ctx);

    metadata.BlendMode = Gx::BlendMode::Auto;
    if (auto mode = attributes.find("blend"); mode != attributes.end())
    {
        if (auto parsed = magic_enum::enum_cast<Gx::BlendMode>(mode->get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
            metadata.BlendMode = parsed.value();
    }

    auto color = attributes.find("color");
    if (color != attributes.end())
    {
        unsigned int a, r, g, b;
        color->at("a").get_to(a);
        color->at("r").get_to(r);
        color->at("g").get_to(g);
        color->at("b").get_to(b);
        metadata.Color = sf::Color(r, g, b, a);
    }
    else
        metadata.Color = sf::Color::White;

    auto texCoords  = attributes.find("texCoords");
    if (texCoords != attributes.end())
    {
        unsigned int x, y, w, h;
        texCoords->at("x").get_to(x);
        texCoords->at("y").get_to(y);
        texCoords->at("width").get_to(w);
        texCoords->at("height").get_to(h);
        metadata.TexCoords = sf::IntRect(sf::Vector2i(x, y), sf::Vector2i(w, h));
    }

    return true;
}
