#include <CXO2/IO/Loaders/UI/Components/Common/ChatWindowLoader.hpp>

#include <CXO2/Metadata/UI/Components/Common/ChatWindowMetadata.hpp>
#include <CXO2/IO/Loaders/Graphics/TransformLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>

namespace Cx
{
    Gx::ResourcePtr<ChatWindow> ChatWindowLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        ChatWindowMetadata metadata;
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (const auto transform = attributes.find("transform"); transform != attributes.end())
            {
                if (!TransformLoader::ParseMetadata(transform.value(), metadata, context))
                    return Instantiate(context);
            }

            if (const auto fontSize = attributes.find("fontSize"); fontSize != attributes.end())
                metadata.FontSize = fontSize->get<unsigned int>();
            else
                metadata.FontSize = 13;

            if (const auto textColor = attributes.find("color"); textColor != attributes.end())
            {
                unsigned int a, r, g, b;
                textColor->at("a").get_to(a);
                textColor->at("r").get_to(r);
                textColor->at("g").get_to(g);
                textColor->at("b").get_to(b);
                metadata.Color = sf::Color(r, g, b, a);
            }
            else
                metadata.Color = sf::Color::White;

            if (const auto maxChatLength = attributes.find("maximumChatLength"); maxChatLength != attributes.end())
                metadata.MaximumChatsLength = maxChatLength->get<unsigned int>();

            if (const auto lineSpacing = attributes.find("lineSpacing"); lineSpacing != attributes.end())
                metadata.LineSpacing = lineSpacing->get<float>();
            else
                metadata.LineSpacing = 0;

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

    Gx::ResourcePtr<ChatWindow> ChatWindowLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const ChatWindowMetadata*>(&meta);
        if (!metadata)
            return Instantiate(context);

        auto window = Instantiate(context);
        window->SetName(metadata->Name);
        window->SetCharacterSize(metadata->FontSize);
        window->SetTextColor(metadata->Color);
        window->SetMaximumChatLength(metadata->MaximumChatsLength);
        window->SetLineSpacing(metadata->LineSpacing);

        const auto ctx = ResourceContextDecorator::Decorate(context);
        if (const auto ffIt = metadata->Require.find("fallbackFonts"); ffIt != metadata->Require.end() && !ffIt->second.is_null())
        {
            auto ffResource = ffIt->second;
            auto fallbackFonts = std::vector<std::string>();

            if (ffResource.is_array())
                fallbackFonts = ffResource.get<std::vector<std::string>>();

            for (const auto& fontPath : fallbackFonts)
            {
                if (const auto font = ctx.Find<Gx::Font>(fontPath))
                    window->AddFallbackFont(*font);
                else if (Gx::FileSystem::Contains(fontPath))
                    window->AddFallbackFont(ctx.Acquire<Gx::Font>(fontPath));
            }
        }

        if (const auto font = ctx.Require<Gx::Font>(*metadata); font)
            window->SetFont(*font);

        if (metadata->Bounds != sf::IntRect())
        {
            window->SetLocalBounds({
                {
                    static_cast<float>(metadata->Bounds.position.x),
                    static_cast<float>(metadata->Bounds.position.y)
                },
                {
                    static_cast<float>(metadata->Bounds.size.x),
                    static_cast<float>(metadata->Bounds.size.y),
                }
            });

            window->SetPosition(metadata->Position.value_or(sf::Vector2f()));
        }
        else if (const auto bound = ctx.Require<sf::IntRect>(*metadata))
        {
            if (!metadata->Position.has_value())
            {
                window->SetPosition({
                    static_cast<float>(bound->position.x),
                    static_cast<float>(bound->position.y),
                });
            }
            else
                window->SetPosition(*metadata->Position);

            window->SetLocalBounds({
                {},
                {
                    static_cast<float>(bound->size.x),
                    static_cast<float>(bound->size.y),
                }
            });
        }
    
        window->SetOrigin(metadata->Origin);

        window->SetScale(metadata->Scale);
        window->SetRotation(metadata->Rotation);

        auto container = SceneComposer::Compose(*window);
        LoadChildren(container, meta, context);
    
        return window;
    }
}
