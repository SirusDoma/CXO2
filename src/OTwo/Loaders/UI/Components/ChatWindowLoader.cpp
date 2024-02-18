#include <OTwo/Loaders/UI/Components/ChatWindowLoader.hpp>

#include <OTwo/Metadata/UI/Components/ChatWindowMetadata.hpp>
#include <OTwo/Loaders/TransformLoader.hpp>
#include <OTwo/Loaders/UI/ListLoader.hpp>

ChatWindowLoader::ChatWindowLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> ChatWindowLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    ChatWindowMetadata metadata;

    metadata.ResourceType = json.at("type").get<std::string>();
    auto attributes = json.at("attributes");

    ParseReferences(json["require"], metadata);
    TransformLoader::ParseTransform(attributes["transform"], metadata);

    auto bounds = attributes.find("bounds");
    if (bounds != attributes.end())
    {
        unsigned int x, y, w, h;
        bounds->at("x").get_to(x);
        bounds->at("y").get_to(y);
        bounds->at("width").get_to(w);
        bounds->at("height").get_to(h);
        metadata.Bounds = sf::FloatRect(x, y, w, h);
    }

    auto fontSize = attributes.find("fontSize");
    if (fontSize != attributes.end())
        metadata.FontSize = fontSize->get<unsigned int>();
    else
        metadata.FontSize = 13;

    auto maxChatLength = attributes.find("maximumChatLength");
    if (maxChatLength != attributes.end())
        metadata.MaximumChatsLength = maxChatLength->get<unsigned int>();

    return std::make_unique<ChatWindowMetadata>(metadata);
}

Gx::ResourcePtr<ChatWindow> ChatWindowLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const ChatWindowMetadata*>(&metadata);
    if (!spec || !context.Font)
        return nullptr;

    auto window = std::make_unique<ChatWindow>(*context.Font, spec->Bounds, spec->FontSize);
    window->SetMaximumChatLength(spec->MaximumChatsLength);

    window->SetName(context.Name);
    window->SetOrigin(spec->Origin);
    window->SetPosition(spec->Position);
    window->SetScale(spec->Scale);
    window->SetRotation(spec->Rotation);

    return window;
}
