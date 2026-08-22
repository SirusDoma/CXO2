#include <CXO2/IO/Loaders/UI/ScrollBarLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/Metadata/UI/ScrollBarMetadata.hpp>
#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>

namespace Cx
{
    Gx::ResourcePtr<ScrollBar> ScrollBarLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        ScrollBarMetadata metadata;
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
                return Instantiate(context);

            if (const auto orientation = attributes.find("orientation"); orientation != attributes.end())
            {
                if (Gx::StringHelper::EqualsCaseInsensitive(orientation->get<std::string>(), "VERTICAL"))
                    metadata.Orientation = ScrollBar::ScrollOrientation::Vertical;
                else
                    metadata.Orientation = ScrollBar::ScrollOrientation::Horizontal;
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
                metadata.Bounds = sf::IntRect(sf::Vector2i(x, y), sf::Vector2i(w, h));
            }
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<ScrollBar> ScrollBarLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const ScrollBarMetadata*>(&meta);
        if (!metadata)
            return Instantiate(context);

        auto scrollBar = Instantiate(context);
        const auto ctx = ResourceContextDecorator::Decorate(context);
        if (const auto texture = ctx.Require<sf::Texture>(*metadata); texture)
        {
            scrollBar->SetTexture(*texture);
            scrollBar->SetTexCoords(metadata->TexCoords);
            scrollBar->SetPosition(metadata->Position.value_or(sf::Vector2f()));
            scrollBar->SetLocalBounds({
                {
                    static_cast<float>(metadata->Bounds.position.x),
                    static_cast<float>(metadata->Bounds.position.y),
                },
                {
                    static_cast<float>(metadata->Bounds.size.x),
                    static_cast<float>(metadata->Bounds.size.y),
                }
            });
        }
        else
        {
            if (const auto sheet = ctx.Require<SpriteSheet>(*metadata))
            {
                scrollBar->SetTexture(sheet->GetTexture());
                if (metadata->TexCoords != sf::IntRect())
                    scrollBar->SetTexCoords(metadata->TexCoords);
                else if (!sheet->TexCoords.empty())
                    scrollBar->SetTexCoords(sheet->TexCoords[0]);
            }

            const auto bound = ctx.Require<sf::IntRect>(*metadata);
            if (metadata->Position.has_value())
            {
                scrollBar->SetPosition(*metadata->Position);
            }
            else if (bound)
            {
                scrollBar->SetPosition({
                    static_cast<float>(bound->position.x),
                    static_cast<float>(bound->position.y),
                });
            }

            if (metadata->Bounds != sf::IntRect())
            {
                scrollBar->SetLocalBounds({
                        {
                            static_cast<float>(metadata->Bounds.position.x),
                            static_cast<float>(metadata->Bounds.position.y),
                        },
                    {
                        static_cast<float>(metadata->Bounds.size.x),
                        static_cast<float>(metadata->Bounds.size.y),
                    }
                });
            }
            else if (bound)
            {
                scrollBar->SetLocalBounds({
                    {},
                    {
                        static_cast<float>(bound->size.x),
                        static_cast<float>(bound->size.y),
                    }
                });
            }
        }

        scrollBar->SetName(metadata->Name);
        scrollBar->SetScrollOrientation(metadata->Orientation);
        scrollBar->SetMaximumValue(metadata->Maximum);
        scrollBar->SetStep(metadata->Step);
        scrollBar->SetColor(metadata->Color);
        scrollBar->SetOrigin(metadata->Origin);
        scrollBar->SetScale(metadata->Scale);
        scrollBar->SetRotation(metadata->Rotation);

        auto container = SceneComposer::Compose(*scrollBar);
        LoadChildren(container, meta, context);

        return scrollBar;
    }
}
