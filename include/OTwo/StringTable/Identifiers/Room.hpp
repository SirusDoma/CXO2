#pragma once

namespace StringTable::Identifiers::Resource::Room
{
    constexpr static auto IDC_AVATAR                 = "STATE_ROOM/IDC_AVATAR";
    constexpr static auto IDC_TEXT_NICKNAME          = "STATE_ROOM/IDC_TEXT_NICKNAME";
    constexpr static auto IDC_TEXT_NOTICE            = "STATE_ROOM/IDC_TEXT_NOTICE";
    constexpr static auto IDC_IMAGE_CHANNEL_CATEGORY = "STATE_ROOM/IDC_IMAGE_CHANNEL_CATEGORY";
    constexpr static auto IDC_NUMBER_CHANNEL_ID      = "STATE_ROOM/IDC_NUMBER_CHANNEL_ID";
    constexpr static auto IDC_CHAT_PANEL             = "STATE_ROOM/IDC_CHAT_PANEL";
    constexpr static auto IDC_USER_LIST              = "STATE_ROOM/IDC_USER_LIST";
    constexpr static auto IDC_ROOM_LIST              = "STATE_ROOM/IDC_ROOM_LIST";
    constexpr static auto IDC_ROOM_BUTTON            = "STATE_ROOM/IDC_ROOM_LIST/IDC_ROOM_BUTTON*";
    constexpr static auto IDC_BUTTON_CREATE_ROOM     = "STATE_ROOM/IDC_BUTTON_CREATE_ROOM";
    constexpr static auto IDC_DIALOG_CREATE_ROOM     = "STATE_ROOM/IDC_DIALOG_CREATE_ROOM";
    constexpr static auto IDC_BUTTON_SHOW_ALL        = "STATE_ROOM/IDC_BUTTON_SHOW_ALL";
    constexpr static auto IDC_BUTTON_SHOW_WAITING    = "STATE_ROOM/IDC_BUTTON_SHOW_WAITING";
    constexpr static auto IDC_BUTTON_ROOM_LEFT       = "STATE_ROOM/IDC_BUTTON_ROOM_LEFT";
    constexpr static auto IDC_BUTTON_ROOM_RIGHT      = "STATE_ROOM/IDC_BUTTON_ROOM_RIGHT";
    constexpr static auto IDC_BUTTON_MUSIC_SHOP      = "STATE_ROOM/IDC_BUTTON_MUSIC_SHOP";
    constexpr static auto IDC_BUTTON_ITEM_SHOP       = "STATE_ROOM/IDC_BUTTON_ITEM_SHOP";
    constexpr static auto IDC_BUTTON_MY_ROOM         = "STATE_ROOM/IDC_BUTTON_MY_ROOM";
    constexpr static auto IDC_BUTTON_BULLETIN        = "STATE_ROOM/IDC_BUTTON_BULLETIN";
    constexpr static auto IDC_BUTTON_TUTORIAL        = "STATE_ROOM/IDC_BUTTON_TUTORIAL";
    constexpr static auto IDC_BUTTON_OPTION          = "STATE_ROOM/IDC_BUTTON_OPTION";
    constexpr static auto IDC_DIALOG_OPTION          = "STATE_ROOM/IDC_DIALOG_OPTION";
    constexpr static auto IDC_BUTTON_BACK            = "STATE_ROOM/IDC_BUTTON_BACK";

    namespace Button
    {
        constexpr static auto IDC_NUMBER_ROOM_ID      = "IDC_NUMBER_ROOM_ID";
        constexpr static auto IDC_TEXT_ROOM_NAME      = "IDC_TEXT_ROOM_NAME";
        constexpr static auto IDC_TEXT_CAPACITY       = "IDC_TEXT_CAPACITY";
        constexpr static auto IDC_IMAGE_GAME_SPEED    = "IDC_IMAGE_GAME_SPEED";
        constexpr static auto IDC_IMAGE_STATE         = "IDC_IMAGE_STATE";
        constexpr static auto IDC_IMAGE_GAME_MODE     = "IDC_IMAGE_GAME_MODE";
        constexpr static auto IDC_IMAGE_OHM_LEVEL     = "IDC_IMAGE_OHM_LEVEL";
        constexpr static auto IDC_IMAGE_PASSWORD      = "IDC_IMAGE_PASSWORD";
        constexpr static auto IDC_IMAGE_LEVEL_LIMIT   = "IDC_IMAGE_LEVEL_LIMIT";
        constexpr static auto IDC_IMAGE_NEW_MUSIC     = "IDC_IMAGE_NEW_MUSIC";
        constexpr static auto IDC_TEXT_MUSIC_NAME     = "IDC_TEXT_MUSIC_NAME";
        constexpr static auto IDC_TEXT_NEW_MUSIC_NAME = "IDC_TEXT_NEW_MUSIC_NAME";
        constexpr static auto IDC_TEXT_LEVEL_RANGE    = "IDC_TEXT_LEVEL_RANGE";
        constexpr static auto IDC_IMAGE_NOT_HAVE      = "IDC_IMAGE_NOT_HAVE";
        constexpr static auto IDC_IMAGE_ROOM_HOVER    = "IDC_IMAGE_ROOM_HOVER";
    }

    namespace UserList
    {
        constexpr static auto IDC_LIST_USER_BUTTON  = "STATE_ROOM/IDC_USER_LIST/IDC_LIST_USER_BUTTON";
        constexpr static auto IDC_TEXT_USER_COUNT   = "STATE_ROOM/IDC_USER_LIST/IDC_TEXT_USER_COUNT";
        constexpr static auto IDC_BUTTON_REFRESH    = "STATE_ROOM/IDC_USER_LIST/IDC_BUTTON_REFRESH";
        constexpr static auto IDC_BUTTON_USER_LEFT  = "STATE_ROOM/IDC_USER_LIST/IDC_BUTTON_USER_LEFT";
        constexpr static auto IDC_BUTTON_USER_RIGHT = "STATE_ROOM/IDC_USER_LIST/IDC_BUTTON_USER_RIGHT";
        constexpr static auto IDC_TEXT_USER_NAME    = "IDC_TEXT_USER_NAME";
    }

    namespace CreateRoom
    {
        constexpr static auto IDC_EDIT_TITLE           = "STATE_ROOM/IDC_DIALOG_CREATE_ROOM/IDC_EDIT_TITLE";
        constexpr static auto IDC_EDIT_PASSWORD        = "STATE_ROOM/IDC_DIALOG_CREATE_ROOM/IDC_EDIT_PASSWORD";
        constexpr static auto IDC_RADIO_JAM_MODE       = "STATE_ROOM/IDC_DIALOG_CREATE_ROOM/IDC_RADIO_JAM_MODE";
        constexpr static auto IDC_RADIO_VERSUS_MODE    = "STATE_ROOM/IDC_DIALOG_CREATE_ROOM/IDC_RADIO_VERSUS_MODE";
        constexpr static auto IDC_RADIO_SINGLE_MODE    = "STATE_ROOM/IDC_DIALOG_CREATE_ROOM/IDC_RADIO_SINGLE_MODE";
        constexpr static auto IDC_ANIMATION_JAM        = "STATE_ROOM/IDC_DIALOG_CREATE_ROOM/IDC_RADIO_JAM_MODE/IDC_ANIMATION_JAM";
        constexpr static auto IDC_ANIMATION_VERSUS     = "STATE_ROOM/IDC_DIALOG_CREATE_ROOM/IDC_RADIO_VERSUS_MODE/IDC_ANIMATION_VERSUS";
        constexpr static auto IDC_ANIMATION_SINGLE     = "STATE_ROOM/IDC_DIALOG_CREATE_ROOM/IDC_RADIO_SINGLE_MODE/IDC_ANIMATION_SINGLE";
        constexpr static auto IDC_TOGGLE_LEVEL_LIMIT   = "STATE_ROOM/IDC_DIALOG_CREATE_ROOM/IDC_TOGGLE_LEVEL_LIMIT";
        constexpr static auto IDC_EDIT_MIN_LEVEL_LIMIT = "STATE_ROOM/IDC_DIALOG_CREATE_ROOM/IDC_EDIT_MIN_LEVEL_LIMIT";
        constexpr static auto IDC_EDIT_MAX_LEVEL_LIMIT = "STATE_ROOM/IDC_DIALOG_CREATE_ROOM/IDC_EDIT_MAX_LEVEL_LIMIT";
        constexpr static auto IDC_TOOLTIP_INFO         = "STATE_ROOM/IDC_DIALOG_CREATE_ROOM/IDC_TOOLTIP_INFO";
    }
}
