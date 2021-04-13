#include <O2/Loaders/UI/Components/ChatWindowLoader.hpp>

#include <O2/Metadata/UI/Components/ChatWindowMetadata.hpp>
#include <O2/Loaders/TransformLoader.hpp>
#include <O2/Loaders/UI/ListLoader.hpp>

ChatWindowLoader::ChatWindowLoader()
{
}

std::unique_ptr<Gx::ResourceMetadata> ChatWindowLoader::LoadMetadata(const void *data, std::size_t size) const
{
    Json json = Json::parse(std::string(reinterpret_cast<const char*>(data), size));
    ChatWindowMetadata metadata;

    metadata.SetResourceType(json.at("type").get<std::string>());
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
        metadata.SetBounds(sf::FloatRect(x, y, w, h));
    }

    auto fontSize = attributes.find("fontSize");
    if (fontSize != attributes.end())
        metadata.SetFontSize(fontSize->get<unsigned int>());
    else
        metadata.SetFontSize(13);

    auto maxChatLength = attributes.find("maximumChatLength");
    if (maxChatLength != attributes.end())
        metadata.SetMaximumChatLength(maxChatLength->get<unsigned int>());

    return std::make_unique<ChatWindowMetadata>(metadata);
}

Gx::ResourcePtr<ChatWindow> ChatWindowLoader::Load(const Gx::ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    auto spec = dynamic_cast<const ChatWindowMetadata*>(&metadata);
    if (!spec || !context.Font)
        return nullptr;

    auto window = std::make_unique<ChatWindow>(*context.Font, spec->GetBounds(), spec->GetFontSize());
    window->SetMaximumChatLength(spec->GetMaximumChatLength());

    window->SetName(context.Name);
    window->SetOrigin(spec->GetOrigin());
    window->SetPosition(spec->GetPosition());
    window->SetScale(spec->GetScale());
    window->SetRotation(spec->GetRotation());

    return window;
}
