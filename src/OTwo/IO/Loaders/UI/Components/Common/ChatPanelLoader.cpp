#include <OTwo/IO/Loaders/UI/Components/Common/ChatPanelLoader.hpp>
#include <OTwo/Metadata/UI/Components/Common/ChatPanelMetadata.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectPopulator.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

#include <Genode/UI/List.hpp>

Gx::ResourcePtr<ChatPanel> ChatPanelLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &ctx) const
{
    ChatPanelMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, ctx))
        return nullptr;

    auto attributes = json.at("attributes");
    if (const auto transform = attributes.find("transform"); transform != attributes.end())
        TransformLoader::ParseMetadata(transform.value(), metadata, ctx);

    return LoadFromMetadata(metadata, ctx);
}

Gx::ResourcePtr<ChatPanel> ChatPanelLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    const auto metadata = dynamic_cast<const ChatPanelMetadata*>(&meta);
    if (metadata == nullptr)
        return nullptr;

    auto chatPanel = std::make_unique<ChatPanel>();
    auto populator = ObjectPopulator::Decorate(chatPanel.get());
    auto ctx       = ResourceContextDecorator::Decorate(context);
    for (auto [key, object] : metadata->Objects)
    {
        auto name = meta.Name + "/" + key;
        ObjectLoader::Load(name, object, populator, ctx);
    }

    chatPanel->SetName(meta.Name);
    chatPanel->SetOrigin(metadata->Origin);
    chatPanel->SetPosition(metadata->Position);
    chatPanel->SetScale(metadata->Scale);
    chatPanel->SetRotation(metadata->Rotation);

    return chatPanel;
}
