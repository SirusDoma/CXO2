#pragma once

namespace Cx::Constants::Identifiers::Resource::MusicShop
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

    namespace Shop
    {
        constexpr static auto IDC_LIST_SHOP           = "STATE_MUSIC_SHOP/IDC_CONTAINER_SHOP/IDC_LIST_SHOP";
        constexpr static auto IDC_BUTTON_LEFT         = "STATE_MUSIC_SHOP/IDC_CONTAINER_SHOP/IDC_BUTTON_LEFT";
        constexpr static auto IDC_BUTTON_RIGHT        = "STATE_MUSIC_SHOP/IDC_CONTAINER_SHOP/IDC_BUTTON_RIGHT";
        constexpr static auto IDC_NUMBER_CURRENT_PAGE = "STATE_MUSIC_SHOP/IDC_CONTAINER_SHOP/IDC_NUMBER_CURRENT_PAGE";
        constexpr static auto IDC_NUMBER_MAX_PAGE     = "STATE_MUSIC_SHOP/IDC_CONTAINER_SHOP/IDC_NUMBER_MAX_PAGE";

        namespace Sort
        {
            constexpr static auto IDC_CONTAINER    = "STATE_MUSIC_SHOP/IDC_CONTAINER_SHOP/IDC_CONTAINER_SORT";
            constexpr static auto IDC_BUTTON_NEW   = "STATE_MUSIC_SHOP/IDC_CONTAINER_SHOP/IDC_CONTAINER_SORT/IDC_BUTTON_SORT_NEW";
            constexpr static auto IDC_BUTTON_GENRE = "STATE_MUSIC_SHOP/IDC_CONTAINER_SHOP/IDC_CONTAINER_SORT/IDC_BUTTON_SORT_GENRE";
            constexpr static auto IDC_BUTTON_TITLE = "STATE_MUSIC_SHOP/IDC_CONTAINER_SHOP/IDC_CONTAINER_SORT/IDC_BUTTON_SORT_TITLE";
            constexpr static auto IDC_BUTTON_LEVEL = "STATE_MUSIC_SHOP/IDC_CONTAINER_SHOP/IDC_CONTAINER_SORT/IDC_BUTTON_SORT_LEVEL";
            constexpr static auto IDC_BUTTON_BPM   = "STATE_MUSIC_SHOP/IDC_CONTAINER_SHOP/IDC_CONTAINER_SORT/IDC_BUTTON_SORT_BPM";
            constexpr static auto IDC_BUTTON_PRICE = "STATE_MUSIC_SHOP/IDC_CONTAINER_SHOP/IDC_CONTAINER_SORT/IDC_BUTTON_SORT_PRICE";
        }

        namespace Item
        {
            constexpr static auto IDC_IMAGE_THUMBNAIL  = "IDC_IMAGE_THUMBNAIL";
            constexpr static auto IDC_IMAGE_NEW        = "IDC_IMAGE_NEW";
            constexpr static auto IDC_TEXT_TITLE       = "IDC_TEXT_TITLE";
            constexpr static auto IDC_TEXT_ARTIST      = "IDC_TEXT_ARTIST";
            constexpr static auto IDC_TEXT_EDITOR      = "IDC_TEXT_EDITOR";
            constexpr static auto IDC_TEXT_GENRE       = "IDC_TEXT_GENRE";
            constexpr static auto IDC_TEXT_BPM         = "IDC_TEXT_BPM";
            constexpr static auto IDC_TEXT_TIME        = "IDC_TEXT_TIME";
            constexpr static auto IDC_TEXT_LEVEL       = "IDC_TEXT_LEVEL";
            constexpr static auto IDC_TEXT_GEM_PRICE   = "IDC_TEXT_GEM_PRICE";
            constexpr static auto IDC_BUTTON_BUY       = "IDC_BUTTON_BUY";
        }
    }

    namespace Music
    {
        constexpr static auto IDC_BUTTON_LEFT         = "STATE_MUSIC_SHOP/IDC_CONTAINER_MUSIC/IDC_BUTTON_LEFT";
        constexpr static auto IDC_BUTTON_RIGHT        = "STATE_MUSIC_SHOP/IDC_CONTAINER_MUSIC/IDC_BUTTON_RIGHT";
        constexpr static auto IDC_BUTTON_DOWNLOAD     = "STATE_MUSIC_SHOP/IDC_CONTAINER_MUSIC/IDC_BUTTON_DOWNLOAD";
        constexpr static auto IDC_NUMBER_CURRENT_PAGE = "STATE_MUSIC_SHOP/IDC_CONTAINER_MUSIC/IDC_NUMBER_CURRENT_PAGE";
        constexpr static auto IDC_NUMBER_MAX_PAGE     = "STATE_MUSIC_SHOP/IDC_CONTAINER_MUSIC/IDC_NUMBER_MAX_PAGE";

        namespace Sort
        {
            constexpr static auto IDC_CONTAINER     = "STATE_MUSIC_SHOP/IDC_CONTAINER_MUSIC/IDC_CONTAINER_SORT";
            constexpr static auto IDC_BUTTON_GENRE  = "STATE_MUSIC_SHOP/IDC_CONTAINER_MUSIC/IDC_CONTAINER_SORT/IDC_BUTTON_SORT_GENRE";
            constexpr static auto IDC_BUTTON_TITLE  = "STATE_MUSIC_SHOP/IDC_CONTAINER_MUSIC/IDC_CONTAINER_SORT/IDC_BUTTON_SORT_TITLE";
            constexpr static auto IDC_BUTTON_LEVEL  = "STATE_MUSIC_SHOP/IDC_CONTAINER_MUSIC/IDC_CONTAINER_SORT/IDC_BUTTON_SORT_LEVEL";
            constexpr static auto IDC_BUTTON_TIME   = "STATE_MUSIC_SHOP/IDC_CONTAINER_MUSIC/IDC_CONTAINER_SORT/IDC_BUTTON_SORT_TIME";
            constexpr static auto IDC_BUTTON_STATUS = "STATE_MUSIC_SHOP/IDC_CONTAINER_MUSIC/IDC_CONTAINER_SORT/IDC_BUTTON_SORT_STATUS";
        }
    }

    namespace MusicItem
    {
        constexpr static auto IDC_IMAGE_SELECTOR = "IDC_IMAGE_SELECTOR";
        constexpr static auto IDC_IMAGE_STATUS   = "IDC_IMAGE_STATUS";
        constexpr static auto IDC_TOGGLE_SELECT  = "IDC_TOGGLE_SELECT";
        constexpr static auto IDC_TEXT_GENRE     = "IDC_TEXT_GENRE";
        constexpr static auto IDC_TEXT_TITLE     = "IDC_TEXT_TITLE";
        constexpr static auto IDC_TEXT_LEVEL     = "IDC_TEXT_LEVEL";
        constexpr static auto IDC_TEXT_TIME      = "IDC_TEXT_TIME";
    }

    namespace Download
    {
        constexpr static auto IDC_TEXT_DOWNLOAD_MUSIC_GAUGE = "STATE_MUSIC_SHOP/IDC_CONTAINER_DOWNLOAD/IDC_TEXT_DOWNLOAD_MUSIC_GAUGE";
        constexpr static auto IDC_TEXT_DOWNLOAD_TOTAL_GAUGE = "STATE_MUSIC_SHOP/IDC_CONTAINER_DOWNLOAD/IDC_TEXT_DOWNLOAD_TOTAL_GAUGE";
        constexpr static auto IDC_TEXT_DOWNLOAD_MUSIC       = "STATE_MUSIC_SHOP/IDC_CONTAINER_DOWNLOAD/IDC_TEXT_DOWNLOAD_MUSIC";
        constexpr static auto IDC_TEXT_DOWNLOAD_MUSIC_TIME  = "STATE_MUSIC_SHOP/IDC_CONTAINER_DOWNLOAD/IDC_TEXT_DOWNLOAD_MUSIC_TIME";
        constexpr static auto IDC_TEXT_DOWNLOAD_TOTAL_TIME  = "STATE_MUSIC_SHOP/IDC_CONTAINER_DOWNLOAD/IDC_TEXT_DOWNLOAD_TOTAL_TIME";
        constexpr static auto IDC_TEXT_DOWNLOAD_SPEED       = "STATE_MUSIC_SHOP/IDC_CONTAINER_DOWNLOAD/IDC_TEXT_DOWNLOAD_SPEED";
        constexpr static auto IDC_TEXT_DOWNLOAD_COUNT       = "STATE_MUSIC_SHOP/IDC_CONTAINER_DOWNLOAD/IDC_TEXT_DOWNLOAD_COUNT";
        constexpr static auto IDC_TEXT_DOWNLOAD_SIZE        = "STATE_MUSIC_SHOP/IDC_CONTAINER_DOWNLOAD/IDC_TEXT_DOWNLOAD_SIZE";
        constexpr static auto IDC_TEXT_DOWNLOAD_STATUS      = "STATE_MUSIC_SHOP/IDC_CONTAINER_DOWNLOAD/IDC_TEXT_DOWNLOAD_STATUS";
        constexpr static auto IDC_ANIMATION_INDICATOR       = "STATE_MUSIC_SHOP/IDC_CONTAINER_DOWNLOAD/IDC_ANIMATION_DOWNLOAD_INDICATOR";
        constexpr static auto IDC_BUTTON_CANCEL             = "STATE_MUSIC_SHOP/IDC_CONTAINER_DOWNLOAD/IDC_BUTTON_CANCEL";
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
