#include <CXO2/IO/Loaders/UI/LabelLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/TransformLoader.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>
#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/Metadata/UI/LabelMetadata.hpp>

#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    Gx::ResourcePtr<Gx::Label> LabelLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        LabelMetadata metadata;
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto attributes = json.find("attributes"); attributes != json.end())
        {
            if (!ParseMetadata(attributes.value(), metadata, context))
                return Instantiate(context);
        }
    
        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<Gx::Label> LabelLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const LabelMetadata*>(&meta);
        if (!metadata)
            return Instantiate(context);
    
        auto label = Instantiate(context);
        const auto ctx = ResourceContextDecorator::Decorate(context);
        if (const auto font = ctx.Require<Gx::Font>(*metadata); font)
            label->SetFont(*font);

        if (const auto ffIt = metadata->Require.find("fallbackFonts"); ffIt != metadata->Require.end() && !ffIt->second.is_null())
        {
            auto fallbackFonts = std::vector<std::string>();
            if (ffIt->second.is_array())
                fallbackFonts = ffIt->second.get<std::vector<std::string>>();

            for (const auto& fontPath : fallbackFonts)
            {
                if (const auto font = ctx.Find<Gx::Font>(fontPath))
                    label->AddFallbackFont(*font);
                else if (Gx::FileSystem::Contains(fontPath))
                    label->AddFallbackFont(ctx.Acquire<Gx::Font>(fontPath));
            }
        }

        if (metadata->Bounds != sf::FloatRect() && !metadata->AllowOverflow)
            label->SetLocalBounds(metadata->Bounds);

        if (metadata->Position.has_value())
        {
            label->SetPosition(*metadata->Position);
        }

        if (const auto bound = ctx.Require<sf::IntRect>(*metadata))
        {
            if (metadata->Bounds == sf::FloatRect() && bound->size != sf::Vector2i() && !metadata->AllowOverflow)
            {
                label->SetLocalBounds({
                    {},
                    {
                        static_cast<float>(bound->size.x),
                        static_cast<float>(bound->size.y),
                    }
                });
            }

            if (!metadata->Position.has_value())
            {
                label->SetPosition(sf::Vector2f(
                    static_cast<float>(bound->position.x),
                    static_cast<float>(bound->position.y)
                ));
            }
        }

        std::uint32_t style = 0;
        if (metadata->Bold)
            style |= Gx::Label::Style::Bold;

        if (metadata->Italic)
            style |= Gx::Label::Style::Italic;

        if (metadata->Underlined)
            style |= Gx::Label::Style::Underlined;

        label->SetName(metadata->Name);
        label->SetStyle(style);
        label->SetCharacterSize(metadata->FontSize);
        label->SetCharacterWidth(metadata->FontWidth);
        label->SetColor(metadata->Color);
        label->SetOutlineColor(metadata->OutlineColor);
        label->SetOutlineThickness(metadata->OutlineThickness);
        label->SetOutlineOffset({0.f, 1.f});
        label->SetString(sf::String::fromUtf8(metadata->String.begin(), metadata->String.end()));
        label->SetOrigin(metadata->Origin);
        label->SetScale(metadata->Scale);
        label->SetRotation(metadata->Rotation);
        label->SetLineAlignment(metadata->Alignment);
        label->SetVerticalAlignment(metadata->VerticalAlignment);
        label->SetEllipsis(sf::String::fromUtf8(metadata->Ellipsis.begin(), metadata->Ellipsis.end()));

        if (metadata->Kerning > 0)
            label->SetLetterSpacing(metadata->Kerning);

        auto container = SceneComposer::Compose(*label);
        LoadChildren(container, meta, context);

        return label;
    }

    bool LabelLoader::ParseMetadata(const Gx::Json& attributes, LabelMetadata& metadata, const Gx::ResourceContext& context)
    {
        if (attributes.empty())
            return false;

        if (const auto transform = attributes.find("transform"); transform != attributes.end())
        {
            if (!TransformLoader::ParseMetadata(transform.value(), metadata, context))
                return false;
        }

        metadata.FontSize = 30;
        if (auto fontSize = attributes.find("fontSize"); fontSize != attributes.end())
            metadata.FontSize = fontSize->get<unsigned int>();

        metadata.FontWidth = 0;
        if (auto fontWidth = attributes.find("fontWidth"); fontWidth != attributes.end())
            metadata.FontWidth = fontWidth->get<unsigned int>();

        if (auto string = attributes.find("string"); string != attributes.end())
            metadata.String = string->get<std::string>();

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

        metadata.Bold = false;
        if (auto bold = attributes.find("bold"); bold != attributes.end())
            metadata.Bold = bold->get<bool>();

        metadata.Italic = false;
        if (auto italic = attributes.find("italic"); italic != attributes.end())
            metadata.Italic = italic->get<bool>();

        metadata.Underlined = false;
        if (auto underlined = attributes.find("underlined"); underlined != attributes.end())
            metadata.Underlined = underlined->get<bool>();

        metadata.Kerning = 0;
        if (auto spacing = attributes.find("kerning"); spacing != attributes.end())
            metadata.Kerning = spacing->get<float>();

        if (auto outline = attributes.find("outline"); outline != attributes.end())
        {
            if (auto thickness = outline->find("thickness"); thickness != outline->end())
                metadata.OutlineThickness = thickness->get<float>();

            color = outline->find("color");
            if (color != outline->end())
            {
                unsigned int a, r, g, b;
                color->at("a").get_to(a);
                color->at("r").get_to(r);
                color->at("g").get_to(g);
                color->at("b").get_to(b);
                metadata.OutlineColor = sf::Color(r, g, b, a);
            }
        }
        else
        {
            metadata.OutlineThickness = 0.f;
            metadata.OutlineColor = sf::Color::Transparent;
        }

        metadata.Alignment = Gx::Text::LineAlignment::Default;
        if (auto alignment = attributes.find("alignment"); alignment != attributes.end())
        {
            if (auto parsed = magic_enum::enum_cast<Gx::Text::LineAlignment>(alignment->get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
                metadata.Alignment = parsed.value();
        }

        metadata.VerticalAlignment = Gx::Label::VerticalAlignment::Center;
        if (auto alignment = attributes.find("verticalAlignment"); alignment != attributes.end())
        {
            if (auto parsed = magic_enum::enum_cast<Gx::Label::VerticalAlignment>(alignment->get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
                metadata.VerticalAlignment = parsed.value();
        }

        if (auto ellipsis = attributes.find("ellipsis"); ellipsis != attributes.end())
            metadata.Ellipsis = ellipsis->get<std::string>();

        metadata.AllowOverflow = false;
        if (auto allowOverflow = attributes.find("allowOverflow"); allowOverflow != attributes.end())
            metadata.AllowOverflow = allowOverflow->get<bool>();

        if (const auto bounds = attributes.find("bounds"); bounds != attributes.end())
        {
            if (bounds->type() == Gx::Json::value_t::object)
            {
                float x, y, w, h;
                bounds->at("x").get_to(x);
                bounds->at("y").get_to(y);
                bounds->at("width").get_to(w);
                bounds->at("height").get_to(h);

                metadata.Bounds = sf::FloatRect{ {x, y}, {w, h} };
            }
            else if (bounds->type() == Gx::Json::value_t::string)
            {
                const auto& bound = context.Acquire<sf::IntRect>(bounds.value().get<std::string>());
                metadata.Bounds = {
                    {},
                    {
                        static_cast<float>(bound.size.x),
                        static_cast<float>(bound.size.y),
                    }
                };
            }
        }

        return true;
    }
}
