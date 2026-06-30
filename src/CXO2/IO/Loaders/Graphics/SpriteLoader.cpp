#include <CXO2/IO/Loaders/Graphics/SpriteLoader.hpp>

#include <CXO2/IO/Loaders/Graphics/TransformLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>

#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/Metadata/Graphics/SpriteMetadata.hpp>

#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    Gx::ResourcePtr<Gx::Sprite> SpriteLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        SpriteMetadata metadata;
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto attributes = json.find("attributes"); attributes != json.end())
        {
            if (!ParseMetadata(attributes.value(), metadata, context))
                return Instantiate(context);
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<Gx::Sprite> SpriteLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto ctx = ResourceContextDecorator::Decorate(context);
        const auto metadata = dynamic_cast<const SpriteMetadata*>(&meta);
        if (!metadata)
            throw Gx::ResourceLoadException("The specified metadata is incompatible");

        auto sprite = Instantiate(context);
        if (const auto texture = ctx.Require<sf::Texture>(*metadata); texture)
        {
            sprite->SetTexture(*texture);
            sprite->SetTexCoords(metadata->TexCoords);
            sprite->SetPosition(metadata->Position);
        }
        else
        {
            if (metadata->Position != sf::Vector2f())
            {
                sprite->SetPosition(metadata->Position);
            }
            else if (const auto bound = ctx.Require<sf::IntRect>(*metadata))
            {
                sprite->SetPosition({
                    static_cast<float>(bound->position.x),
                    static_cast<float>(bound->position.y),
                });
            }

            if (const auto sheet = ctx.Require<SpriteSheet>(*metadata))
            {
                sprite->SetTexture(sheet->GetTexture());
                if (metadata->TexCoords != sf::IntRect())
                    sprite->SetTexCoords(metadata->TexCoords);
                else if (metadata->FrameID.has_value())
                    sprite->SetTexCoords(sheet->TexCoords[metadata->FrameID.value()]);
                else
                    sprite->SetTexCoords(sheet->TexCoords[0]);

                if (metadata->FrameID.has_value() && sprite->GetPosition() == sf::Vector2f())
                {
                    sprite->SetPosition({
                        static_cast<float>(sheet->Frames[metadata->FrameID.value()].position.x),
                        static_cast<float>(sheet->Frames[metadata->FrameID.value()].position.y),
                    });
                }
            }
        }

        sprite->SetName(metadata->Name);
        sprite->SetBlendMode(metadata->BlendMode);
        sprite->SetColor(metadata->Color);
        sprite->SetOrigin(metadata->Origin);
        sprite->SetScale(metadata->Scale);
        sprite->SetRotation(metadata->Rotation);

        return sprite;
    }

    bool SpriteLoader::ParseMetadata(const Gx::Json& attributes, SpriteMetadata& metadata, const Gx::ResourceContext& ctx)
    {
        if (attributes.empty())
            return false;

        if (const auto transform = attributes.find("transform"); transform != attributes.end())
            TransformLoader::ParseMetadata(transform.value(), metadata, ctx);

        if (const auto id = attributes.find("id"); id != attributes.end())
            metadata.FrameID = id->get<std::uint32_t>();

        metadata.BlendMode = Gx::BlendMode::Auto;
        if (const auto mode = attributes.find("blend"); mode != attributes.end())
        {
            if (const auto parsed = magic_enum::enum_cast<Gx::BlendMode>(mode->get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
                metadata.BlendMode = parsed.value();
        }

        const auto color = attributes.find("color");
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

        const auto texCoords  = attributes.find("texCoords");
        if (texCoords != attributes.end())
        {
            unsigned int x, y, w, h;
            texCoords->at("x").get_to(x);
            texCoords->at("y").get_to(y);
            texCoords->at("width").get_to(w);
            texCoords->at("height").get_to(h);
            metadata.TexCoords = sf::IntRect(sf::Vector2i(x, y), sf::Vector2i(w, h));
        }
        else
            metadata.TexCoords = sf::IntRect();

        return true;
    }
}
