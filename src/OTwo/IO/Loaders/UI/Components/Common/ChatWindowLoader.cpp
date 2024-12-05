#include <OTwo/IO/Loaders/UI/Components/Common/ChatWindowLoader.hpp>

#include <OTwo/Metadata/UI/Components/Common/ChatWindowMetadata.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

Gx::ResourcePtr<ChatWindow> ChatWindowLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
{
    ChatWindowMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto attributes = json.at("attributes");
    if (const auto transform = attributes.find("transform"); transform != attributes.end())
    {
        if (!TransformLoader::ParseMetadata(transform.value(), metadata, context))
            return nullptr;
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

    metadata.MaximumChatsLength = attributes.at("maximumChatLength").get<unsigned int>();
    if (const auto lineSpacing = attributes.find("lineSpacing"); lineSpacing != attributes.end())
    {
        metadata.LineSpacing = lineSpacing->get<unsigned int>();
        if (metadata.LineSpacing < 0)
            metadata.LineSpacing = 0;
    }
    else
        metadata.LineSpacing = 0;

    if (const auto bounds = attributes.find("bounds"); bounds != attributes.end())
    {
        unsigned int x, y, w, h;
        bounds->at("x").get_to(x);
        bounds->at("y").get_to(y);
        bounds->at("width").get_to(w);
        bounds->at("height").get_to(h);
        metadata.Bounds = sf::FloatRect(sf::Vector2f(x, y), sf::Vector2f(w, h));
    }

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<ChatWindow> ChatWindowLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const ChatWindowMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    auto window = std::make_unique<ChatWindow>();
    window->SetName(metadata->Name);
    window->SetCharacterSize(metadata->FontSize);
    window->SetTextColor(metadata->Color);
    window->SetMaximumChatLength(metadata->MaximumChatsLength);
    window->SetLineSpacing(metadata->LineSpacing);
    window->SetLocalBounds(metadata->Bounds);

    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto font = ctx.Find<Gx::Font>(*metadata); font)
        window->SetFont(*font);
    else
        return nullptr;
    
    window->SetOrigin(metadata->Origin);
    window->SetPosition(metadata->Position);
    window->SetScale(metadata->Scale);
    window->SetRotation(metadata->Rotation);

    auto container = ObjectContainer::Decorate(window.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(context, name);

            ObjectLoader::Load(name, object, container, objectCtx);
        }
    }
    
    return window;
}
