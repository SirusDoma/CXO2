#ifndef O2JAM_RESOURCE_METADATA_HPP
#define O2JAM_RESOURCE_METADATA_HPP

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
    enum ResourceType
    {
        None,

        // SceneGraph
        Scene,

        // Audio
        Music,
        Sound,

        // Graphics
        Sprite,
        Shape,
        Image,
        Animation,
        Avatar,

        // UI
        List,
        Label,
        Number,
        TextBox,
        Button,
        CheckBox,
        RadioButton,
        ProgressBar,
        ScrollBar,
        ToolTip,
        Dialog,
        UiContainer,

        // Custom Components
        Marquee,
        ChatPanel,
        ChatWindow,
        ChannelBoard,
        ChannelButton,
        UserList,
        RoomContainer,
        RoomButton,
        AvatarInfo,

        // Avatar
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

#endif
