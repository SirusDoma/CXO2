#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>

#include <OTwo/Avatar/Avatar.hpp>

#include <OTwo/UI/Common/Marquee.hpp>
#include <OTwo/UI/Common/ChatPanel.hpp>
#include <OTwo/UI/Common/ChatWindow.hpp>
#include <OTwo/UI/Planet/ChannelButton.hpp>
#include <OTwo/UI/Planet/ChannelBoard.hpp>
#include <OTwo/UI/Room/UserList.hpp>
#include <OTwo/UI/Room/RoomContainer.hpp>
#include <OTwo/UI/Room/RoomButton.hpp>
#include <OTwo/UI/Waiting/AvatarInfo.hpp>
#include <OTwo/UI/Waiting/MapSelector.hpp>
#include <OTwo/UI/Waiting/InstrumentSelector.hpp>
#include <OTwo/UI/Playing/Equalizer.hpp>

#include <Genode/Graphics.hpp>
#include <Genode/UI.hpp>

void ObjectLoader::Load(const std::string& name, const Gx::Json& json, ObjectContainer& container, Gx::ResourceContext& ctx)
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
            container.Add(name, LoadResource<Avatar>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Sound:
        {
            container.Add(name, LoadResource<sf::Sound>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Music:
        {
            container.Add(name, LoadResource<sf::Music>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Sprite:
        {
            container.Add(name, LoadResource<Gx::Sprite>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Shape:
        {
            container.Add(name, LoadResource<Gx::Shape>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Image:
        {
            container.Add(name, LoadResource<Gx::Image>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Animation:
        {
            container.Add(name, LoadResource<Gx::Animation>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Cursor:
        {
            container.Add(name, LoadResource<Gx::Cursor>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::List:
        {
            container.Add(name, LoadResource<Gx::List>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Label:
        {
            container.Add(name, LoadResource<Gx::Label>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Number:
        {
            container.Add(name, LoadResource<Gx::Number>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::TextBox:
        {
            container.Add(name, LoadResource<Gx::TextBox>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Button:
        {
            container.Add(name, LoadResource<Gx::Button>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::CheckBox:
        {
            container.Add(name, LoadResource<Gx::CheckBox>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::RadioButton:
        {
            container.Add(name, LoadResource<Gx::RadioButton>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Gauge:
        {
            container.Add(name, LoadResource<Gx::Gauge>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::ScrollBar:
        {
            container.Add(name, LoadResource<Gx::ScrollBar>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::ToolTip:
        {
            container.Add(name, LoadResource<Gx::ToolTip>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Dialog:
        {
            container.Add(name, LoadResource<Gx::Dialog>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::UiContainer:
        {
            container.Add(name, LoadResource<Gx::UiContainer>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Marquee:
        {
            container.Add(name, LoadResource<Marquee>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::ChatPanel:
        {
            container.Add(name, LoadResource<Gx::UiContainer, ChatPanel>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::ChatWindow:
        {
            container.Add(name, LoadResource<ChatWindow>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::ChannelButton:
        {
            container.Add(name, LoadResource<ChannelButton>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::ChannelBoard:
        {
            container.Add(name, LoadResource<ChannelBoard>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::UserList:
        {
            container.Add(name, LoadResource<Gx::UiContainer, UserList>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::RoomContainer:
        {
            container.Add(name, LoadResource<Gx::UiContainer, RoomContainer>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::RoomButton:
        {
            container.Add(name, LoadResource<Gx::Image, RoomButton>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::AvatarInfo:
        {
            container.Add(name, LoadResource<AvatarInfo>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::MapSelector:
        {
            container.Add(name, LoadResource<Gx::UiContainer, MapSelector>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::InstrumentSelector:
        {
            container.Add(name, LoadResource<Gx::UiContainer, InstrumentSelector>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Equalizer:
        {
            container.Add(name, LoadResource<Equalizer>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::None:
        default:
            break;
    }
}
