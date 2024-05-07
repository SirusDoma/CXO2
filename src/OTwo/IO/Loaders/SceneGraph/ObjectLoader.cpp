#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>

#include <OTwo/Avatar/Avatar.hpp>

#include <OTwo/States/Components/Common/Marquee.hpp>
#include <OTwo/States/Components/Common/ChatPanel.hpp>
#include <OTwo/States/Components/Common/ChatWindow.hpp>
#include <OTwo/States/Components/Planet/ChannelButton.hpp>
#include <OTwo/States/Components/Planet/ChannelBoard.hpp>
#include <OTwo/States/Components/Room/UserList.hpp>
#include <OTwo/States/Components/Room/RoomContainer.hpp>
#include <OTwo/States/Components/Room/RoomButton.hpp>
#include <OTwo/States/Components/Waiting/AvatarInfo.hpp>

#include <Genode/Graphics.hpp>
#include <Genode/UI.hpp>

void ObjectLoader::Load(const std::string &name, const Gx::Json &json, ObjectPopulator &populator, Gx::ResourceContext &ctx)
{
    const auto metaLoader = MetadataLoader();
    Gx::ResourcePtr<ResourceMetadata> metadata;
    if (json.type() == Gx::Json::value_t::string)
        metadata = metaLoader.LoadFromFile(json.get<std::string>(), ctx);
    else
        metadata = metaLoader.LoadFromJson(json, ctx);

    switch (metadata->Type)
    {
        case ResourceMetadata::ResourceType::Avatar:
        {
            populator.Populate(name, LoadResource<Avatar>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Sound:
        {
            populator.Populate(name, LoadResource<sf::Sound>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Music:
        {
            populator.Populate(name, LoadResource<sf::Music>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Sprite:
        {
            populator.Populate(name, LoadResource<Gx::Sprite>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Shape:
        {
            populator.Populate(name, LoadResource<Gx::Shape>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Image:
        {
            populator.Populate(name, LoadResource<Gx::Image>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Animation:
        {
            populator.Populate(name, LoadResource<Gx::Animation>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::List:
        {
            populator.Populate(name, LoadResource<Gx::List>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Label:
        {
            populator.Populate(name, LoadResource<Gx::Label>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Number:
        {
            populator.Populate(name, LoadResource<Gx::Number>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::TextBox:
        {
            populator.Populate(name, LoadResource<Gx::TextBox>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Button:
        {
            populator.Populate(name, LoadResource<Gx::Button>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::CheckBox:
        {
            populator.Populate(name, LoadResource<Gx::CheckBox>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::RadioButton:
        {
            populator.Populate(name, LoadResource<Gx::RadioButton>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Gauge:
        {
            populator.Populate(name, LoadResource<Gx::Gauge>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::ScrollBar:
        {
            populator.Populate(name, LoadResource<Gx::ScrollBar>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::ToolTip:
        {
            populator.Populate(name, LoadResource<Gx::ToolTip>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Dialog:
        {
            populator.Populate(name, LoadResource<Gx::Dialog>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::UiContainer:
        {
            populator.Populate(name, LoadResource<Gx::UiContainer>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Marquee:
        {
            populator.Populate(name, LoadResource<Marquee>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::ChatPanel:
        {
            populator.Populate(name, LoadResource<ChatPanel>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::ChatWindow:
        {
            populator.Populate(name, LoadResource<ChatWindow>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::ChannelButton:
        {
            populator.Populate(name, LoadResource<ChannelButton>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::ChannelBoard:
        {
            populator.Populate(name, LoadResource<ChannelBoard>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::UserList:
        {
            populator.Populate(name, LoadResource<UserList>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::RoomContainer:
        {
            populator.Populate(name, LoadResource<RoomContainer>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::RoomButton:
        {
            populator.Populate(name, LoadResource<RoomButton>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::AvatarInfo:
        {
            populator.Populate(name, LoadResource<AvatarInfo>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::None:
        default:
            break;
    }
}