#pragma once

namespace Cx::StringTable::Identifiers::Resource::MusicShop
{
    constexpr static auto IDC_NUMBER_GEM          = "STATE_MUSIC_SHOP/IDC_NUMBER_GEM";
    constexpr static auto IDC_NUMBER_CASH         = "STATE_MUSIC_SHOP/IDC_NUMBER_CASH";
    constexpr static auto IDC_CONTAINER_SHOP      = "STATE_MUSIC_SHOP/IDC_CONTAINER_SHOP";
    constexpr static auto IDC_CONTAINER_MUSIC     = "STATE_MUSIC_SHOP/IDC_CONTAINER_MUSIC";
    constexpr static auto IDC_CONTAINER_DOWNLOAD  = "STATE_MUSIC_SHOP/IDC_CONTAINER_DOWNLOAD";
    constexpr static auto IDC_CONTAINER_CART      = "STATE_MUSIC_SHOP/IDC_CONTAINER_CART";
    constexpr static auto IDC_BUTTON_DOWNLOAD_TAB = "STATE_MUSIC_SHOP/IDC_BUTTON_DOWNLOAD_TAB";
    constexpr static auto IDC_BUTTON_CART_TAB     = "STATE_MUSIC_SHOP/IDC_BUTTON_CART_TAB";
    constexpr static auto IDC_BUTTON_SHOW_ALL     = "STATE_MUSIC_SHOP/IDC_CONTAINER_SHOP/IDC_BUTTON_SHOW_ALL";
    constexpr static auto IDC_BUTTON_SHOW_BUYABLE = "STATE_MUSIC_SHOP/IDC_CONTAINER_SHOP/IDC_BUTTON_SHOW_BUYABLE";
    constexpr static auto IDC_LIST_MUSIC          = "STATE_MUSIC_SHOP/IDC_CONTAINER_MUSIC/IDC_LIST_MUSIC";
    constexpr static auto IDC_BUTTON_BACK         = "STATE_MUSIC_SHOP/IDC_BUTTON_BACK";

    namespace MusicItem
    {
        constexpr static auto IDC_IMAGE_SELECTOR = "IDC_IMAGE_SELECTOR";
        constexpr static auto IDC_IMAGE_STATUS   = "IDC_IMAGE_STATUS";
        constexpr static auto IDC_TOGGLE_SELECT  = "IDC_TOGGLE_SELECT";
    }

    namespace Download
    {
        constexpr static auto IDC_TEXT_DOWNLOAD_MUSIC_GAUGE = "STATE_MUSIC_SHOP/IDC_CONTAINER_DOWNLOAD/IDC_TEXT_DOWNLOAD_MUSIC_GAUGE";
        constexpr static auto IDC_TEXT_DOWNLOAD_TOTAL_GAUGE = "STATE_MUSIC_SHOP/IDC_CONTAINER_DOWNLOAD/IDC_TEXT_DOWNLOAD_TOTAL_GAUGE";
    }

    namespace Cart
    {
        constexpr static auto IDC_LIST_CART           = "STATE_MUSIC_SHOP/IDC_CONTAINER_CART/IDC_LIST_CART";
        constexpr static auto IDC_BUTTON_BUY          = "STATE_MUSIC_SHOP/IDC_CONTAINER_CART/IDC_BUTTON_BUY";
        constexpr static auto IDC_BUTTON_GIFT         = "STATE_MUSIC_SHOP/IDC_CONTAINER_CART/IDC_BUTTON_GIFT";
        constexpr static auto IDC_BUTTON_LEFT         = "STATE_MUSIC_SHOP/IDC_CONTAINER_CART/IDC_BUTTON_LEFT";
        constexpr static auto IDC_BUTTON_RIGHT        = "STATE_MUSIC_SHOP/IDC_CONTAINER_CART/IDC_BUTTON_RIGHT";
        constexpr static auto IDC_NUMBER_CURRENT_PAGE = "STATE_MUSIC_SHOP/IDC_CONTAINER_CART/IDC_NUMBER_CURRENT_PAGE";
        constexpr static auto IDC_NUMBER_MAX_PAGE     = "STATE_MUSIC_SHOP/IDC_CONTAINER_CART/IDC_NUMBER_MAX_PAGE";
        constexpr static auto IDC_NUMBER_TOTAL_GEM    = "STATE_MUSIC_SHOP/IDC_CONTAINER_CART/IDC_NUMBER_TOTAL_GEM";
        constexpr static auto IDC_NUMBER_TOTAL_CASH   = "STATE_MUSIC_SHOP/IDC_CONTAINER_CART/IDC_NUMBER_TOTAL_CASH";

        namespace Item
        {
            constexpr static auto IDC_TEXT_NUMBER     = "IDC_TEXT_NUMBER";
            constexpr static auto IDC_TEXT_NAME       = "IDC_TEXT_NAME";
            constexpr static auto IDC_IMAGE_ITEM_TYPE = "IDC_IMAGE_ITEM_TYPE";
            constexpr static auto IDC_TEXT_PRICE      = "IDC_TEXT_PRICE";
            constexpr static auto IDC_BUTTON_DELETE   = "IDC_BUTTON_DELETE";
        }
    }
}
