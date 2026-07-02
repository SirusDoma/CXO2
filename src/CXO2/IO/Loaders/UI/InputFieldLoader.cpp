#include <CXO2/IO/Loaders/UI/InputFieldLoader.hpp>
#include <CXO2/IO/Loaders/UI/LabelLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/Metadata/UI/InputFieldMetadata.hpp>
#include <CXO2/IO/Loaders/SceneGraph/ObjectLoader.hpp>

namespace Cx
{
    Gx::ResourcePtr<Gx::InputField> InputFieldLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        InputFieldMetadata metadata;
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (!LabelLoader::ParseMetadata(attributes, metadata, context))
                return Instantiate(context);

            if (const auto bounds = attributes.find("bounds"); bounds != attributes.end())
            {
                unsigned int x, y, w, h;
                bounds->at("x").get_to(x);
                bounds->at("y").get_to(y);
                bounds->at("width").get_to(w);
                bounds->at("height").get_to(h);

                metadata.Bounds = sf::IntRect(sf::Vector2i(x, y), sf::Vector2i(w, h));
            }

            if (const auto maxLength = attributes.find("maximumLength"); maxLength != attributes.end())
                metadata.MaximumLength = maxLength->get<unsigned int>();
            else
                metadata.MaximumLength = 0;

            if (const auto highlightTextColor = attributes.find("highlightTextColor"); highlightTextColor != attributes.end())
            {
                unsigned int a, r, g, b;
                highlightTextColor->at("a").get_to(a);
                highlightTextColor->at("r").get_to(r);
                highlightTextColor->at("g").get_to(g);
                highlightTextColor->at("b").get_to(b);
                metadata.HighlightTextColor = sf::Color(r, g, b, a);
            }
            else
                metadata.HighlightTextColor = sf::Color::Black;

            if (const auto highlightBackColor = attributes.find("highlightBackColor"); highlightBackColor != attributes.end())
            {
                unsigned int a, r, g, b;
                highlightBackColor->at("a").get_to(a);
                highlightBackColor->at("r").get_to(r);
                highlightBackColor->at("g").get_to(g);
                highlightBackColor->at("b").get_to(b);
                metadata.HighlightBackColor = sf::Color(r, g, b, a);
            }
            else
                metadata.HighlightBackColor = sf::Color::White;
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<Gx::InputField> InputFieldLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const InputFieldMetadata*>(&meta);
        if (!metadata)
            throw Gx::ResourceLoadException(context.GetID(), "The specified metadata is incompatible");
    
        auto input = Instantiate(context);
        const auto ctx = ResourceContextDecorator::Decorate(context);
        if (const auto font = ctx.Require<Gx::Font>(*metadata); font)
            input->SetFont(*font);

        if (const auto ffIt = metadata->Require.find("fallbackFonts"); ffIt != metadata->Require.end() && ffIt->second.has_value())
        {
            auto ffResource = ffIt->second;
            auto fallbackFonts = std::vector<std::string>();

            if (ffResource.type() == typeid(Gx::Json))
                fallbackFonts = std::any_cast<Gx::Json>(ffResource);
            else if (ffResource.type() == typeid(std::vector<std::string>()))
                fallbackFonts = std::any_cast<std::vector<std::string>>(ffResource);

            for (const auto& fontPath : fallbackFonts)
            {
                if (const auto font = ctx.Find<Gx::Font>(fontPath))
                    input->AddFallbackFont(*font);
                else if (Gx::FileSystem::Contains(fontPath))
                    input->AddFallbackFont(ctx.Acquire<Gx::Font>(fontPath));
            }
        }

        if (metadata->Bounds != sf::IntRect())
        {
            input->SetLocalBounds({
                {
                    static_cast<float>(metadata->Bounds.position.x),
                    static_cast<float>(metadata->Bounds.position.y)
                },
                {
                    static_cast<float>(metadata->Bounds.size.x),
                    static_cast<float>(metadata->Bounds.size.y),
                }
            });

            input->SetPosition(metadata->Position);
        }
        else if (const auto bound = ctx.Require<sf::IntRect>(*metadata))
        {
            if (metadata->Position == sf::Vector2f())
            {
                input->SetPosition(sf::Vector2f(
                    static_cast<float>(bound->position.x),
                    static_cast<float>(bound->position.y)
                ) + sf::Vector2f(2.f, 2.f));
            }
            else
                input->SetPosition(metadata->Position);

            input->SetLocalBounds({
                {},
                {
                    static_cast<float>(bound->size.x),
                    static_cast<float>(bound->size.y),
                }
            });
        }

        input->SetCharacterSize(metadata->FontSize);
        input->SetColor(metadata->Color);
        input->SetOutlineThickness(metadata->OutlineThickness);
        input->SetOutlineColor(metadata->OutlineColor);
        input->SetString(metadata->String);

        input->SetMaximumTextLength(metadata->MaximumLength);
        input->SetHighlightTextColor(metadata->HighlightTextColor);
        input->SetHighlightBackColor(metadata->HighlightBackColor);

        input->SetOrigin(metadata->Origin);
        input->SetScale(metadata->Scale);
        input->SetRotation(metadata->Rotation);

        auto container = ObjectContainer::Decorate(input.get());
        LoadChildren(container, meta, context);

        return input;
    }
}
