#include <CXO2/IO/Loaders/UI/Components/Common/MarqueeLoader.hpp>
#include <CXO2/IO/Loaders/UI/LabelLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/Metadata/UI/Components/Common/MarqueeMetadata.hpp>
#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>

namespace Cx
{
    Gx::ResourcePtr<Marquee> MarqueeLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        MarqueeMetadata metadata;
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (!LabelLoader::ParseMetadata(attributes, metadata, context))
                return Instantiate(context);

            if (const auto speed = attributes.find("speed"); speed != attributes.end())
                metadata.Speed = speed->get<double>();
            else
                metadata.Speed = 30.f;

            if (const auto bounds = attributes.find("bounds"); bounds != attributes.end())
            {
                unsigned int x, y, w, h;
                bounds->at("x").get_to(x);
                bounds->at("y").get_to(y);
                bounds->at("width").get_to(w);
                bounds->at("height").get_to(h);
                metadata.Bounds = sf::IntRect(sf::Vector2i(x, y), sf::Vector2i(w, h));
            }
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<Marquee> MarqueeLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const MarqueeMetadata*>(&meta);
        if (!metadata)
            throw Gx::ResourceLoadException(context.GetID(), "The specified metadata is incompatible");

        auto marquee = Instantiate(context);
        const auto ctx = ResourceContextDecorator::Decorate(context);
        if (const auto font = ctx.Require<Gx::Font>(*metadata); font)
            marquee->SetFont(*font);

        if (metadata->Bounds != sf::IntRect())
        {
            marquee->SetLocalBounds({
                {
                    static_cast<float>(metadata->Bounds.position.x),
                    static_cast<float>(metadata->Bounds.position.y)
                },
                {
                    static_cast<float>(metadata->Bounds.size.x),
                    static_cast<float>(metadata->Bounds.size.y),
                }
            });

            marquee->SetPosition(metadata->Position);
        }
        else if (const auto bound = ctx.Require<sf::IntRect>(*metadata))
        {
            marquee->SetLocalBounds({
                {
                    static_cast<float>(bound->position.x),
                    static_cast<float>(bound->position.y),
                },
                {
                    static_cast<float>(bound->size.x),
                    static_cast<float>(bound->size.y),
                }
            });
        }

        marquee->SetName(metadata->Name);
        marquee->SetSpeed(metadata->Speed);
        marquee->SetCharacterSize(metadata->FontSize);
        marquee->SetColor(metadata->Color);
        marquee->SetOutlineThickness(metadata->OutlineThickness);
        marquee->SetOutlineColor(metadata->OutlineColor);
        marquee->SetString(metadata->String);

        marquee->SetOrigin(metadata->Origin);
        marquee->SetScale(metadata->Scale);
        marquee->SetRotation(metadata->Rotation);

        return marquee;
    }
}
