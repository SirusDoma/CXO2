#pragma once

#include <Genode/IO/Json.hpp>

#include <any>
#include <unordered_map>

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

        // SceneGraph
        State,

        // Audio
        Music,
        Sound,

        // Graphics
        Sprite,
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

        // Dialogs
        OptionDialog,
        CreateRoomDialog,
        SelectMusicDialog,

        // Custom Components
        Marquee,
        ChatPanel,
        ChatWindow,
        ChannelBoard,
        ChannelButton,
        RoomList,
        RoomButton,
        AvatarInfo,
        MapSelector,
        InstrumentSelector,
        PlayMenu,
        Equalizer,

        // Avatar
        Avatar,
        ItemData,
        Item
    };

    virtual ~ResourceMetadata() = default;

    using RequireMap = std::unordered_map<std::string, std::any>;
    using ObjectMap  = std::vector<ObjectMetadata>; //std::unordered_map<std::string, Gx::Json>;

    std::string  Name;
    ResourceType Type;
    RequireMap   Require;
    ObjectMap    Objects;
};
