#pragma once

#include <Genode/IO/Json.hpp>

#include <any>
#include <unordered_map>

namespace Cx
{
    struct ObjectMetadata
    {
        std::string Name;
        Gx::Json Data;
    };

    struct ResourceMetadata
    {
        enum class ResourceType
        {
            None,

            // Root
            ControlList,
            ItemData,
            SetInfoData,

            // SceneGraph
            State,

            // Audio
            Music,
            Sound,

            // Graphics
            Sprite,
            Text,
            Shape,
            Image,
            Animation,

            // UI
            Cursor,
            List,
            Label,
            BitmapNumber,
            InputField,
            Button,
            ToggleButton,
            RadioButton,
            Gauge,
            ScrollBar,
            ToolTip,
            Dialog,
            UiContainer,

            // Custom Components
            AvatarInfo,
            ChatPanel,
            ChatWindow,
            ChannelBoard,
            ChannelButton,
            InstrumentSelector,
            UserList,
            Equalizer,
            PlayMenu,
            RoomList,
            RoomButton,
            MapSelector,
            Marquee,

            // Custom Dialogs
            OptionDialog,
            CreateRoomDialog,
            SelectMusicDialog,

            // Avatar
            Avatar,
            Item,
            SetInfo
        };

        virtual ~ResourceMetadata() = default;

        using RequireMap = std::unordered_map<std::string, std::any>;
        using ObjectMap  = std::vector<ObjectMetadata>; //std::unordered_map<std::string, Gx::Json>;

        std::string  Name;
        std::string  SourceFormat;
        ResourceType Type;
        RequireMap   Require;
        ObjectMap    Objects;
    };
}
