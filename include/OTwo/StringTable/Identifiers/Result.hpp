#pragma once

namespace StringTable::Identifiers::Resource::Result
{
    constexpr static auto IDC_CONTAINER_BACKGROUND       = "STATE_RESULT/IDC_CONTAINER_BACKGROUND";
    constexpr static auto IDC_IMAGE_STATE_RESULT         = "STATE_RESULT/IDC_IMAGE_STATE_RESULT";
    constexpr static auto IDC_IMAGE_STATE_RESULT_TOP     = "STATE_RESULT/IDC_IMAGE_STATE_RESULT_TOP";
    constexpr static auto IDC_IMAGE_STATE_RESULT_BOTTOM  = "STATE_RESULT/IDC_IMAGE_STATE_RESULT_BOTTOM";
    constexpr static auto IDC_IMAGE_WINLOSE              = "STATE_RESULT/IDC_IMAGE_WINLOSE";

    namespace Top
    {
        constexpr static auto IDC_NUMBER_POINT           = "STATE_RESULT/IDC_IMAGE_STATE_RESULT_TOP/IDC_NUMBER_POINT";
        constexpr static auto IDC_TEXT_GEM               = "STATE_RESULT/IDC_IMAGE_STATE_RESULT_TOP/IDC_TEXT_GEM";
        constexpr static auto IDC_CONTAINER_PLAYER_SCORE = "STATE_RESULT/IDC_IMAGE_STATE_RESULT_TOP/IDC_CONTAINER_PLAYER_SCORE";

        namespace Score
        {
            constexpr static auto IDC_TEXT_PLAYER_COOL          = "STATE_RESULT/IDC_IMAGE_STATE_RESULT_TOP/IDC_CONTAINER_PLAYER_SCORE/IDC_TEXT_PLAYER_COOL";
            constexpr static auto IDC_TEXT_PLAYER_GOOD          = "STATE_RESULT/IDC_IMAGE_STATE_RESULT_TOP/IDC_CONTAINER_PLAYER_SCORE/IDC_TEXT_PLAYER_GOOD";
            constexpr static auto IDC_TEXT_PLAYER_BAD           = "STATE_RESULT/IDC_IMAGE_STATE_RESULT_TOP/IDC_CONTAINER_PLAYER_SCORE/IDC_TEXT_PLAYER_BAD";
            constexpr static auto IDC_TEXT_PLAYER_MISS          = "STATE_RESULT/IDC_IMAGE_STATE_RESULT_TOP/IDC_CONTAINER_PLAYER_SCORE/IDC_TEXT_PLAYER_MISS";
            constexpr static auto IDC_TEXT_PLAYER_MAX_COMBO     = "STATE_RESULT/IDC_IMAGE_STATE_RESULT_TOP/IDC_CONTAINER_PLAYER_SCORE/IDC_TEXT_PLAYER_MAX_COMBO";
            constexpr static auto IDC_TEXT_PLAYER_MAX_JAM_COMBO = "STATE_RESULT/IDC_IMAGE_STATE_RESULT_TOP/IDC_CONTAINER_PLAYER_SCORE/IDC_TEXT_PLAYER_MAX_JAM_COMBO";
        }
    }

    namespace Bottom
    {
        constexpr static auto IDC_LIST_RANK_SCORE   = "STATE_RESULT/IDC_IMAGE_STATE_RESULT_BOTTOM/IDC_LIST_RANK_SCORE";
        constexpr static auto IDC_BUTTON_PLAY_RETRY = "STATE_RESULT/IDC_IMAGE_STATE_RESULT_BOTTOM/IDC_BUTTON_PLAY_RETRY";
        constexpr static auto IDC_BUTTON_BACK       = "STATE_RESULT/IDC_IMAGE_STATE_RESULT_BOTTOM/IDC_BUTTON_BACK";

        namespace Score
        {
            constexpr static auto IDC_RECTANGLE_HIGHLIGHT     = "IDC_RECTANGLE_HIGHLIGHT";
            constexpr static auto IDC_TEXT_RANK_NUMBER        = "IDC_TEXT_RANK_NUMBER";
            constexpr static auto IDC_TEXT_RANK_NAME          = "IDC_TEXT_RANK_NAME";
            constexpr static auto IDC_TEXT_RANK_COOL          = "IDC_TEXT_RANK_COOL";
            constexpr static auto IDC_TEXT_RANK_GOOD          = "IDC_TEXT_RANK_GOOD";
            constexpr static auto IDC_TEXT_RANK_BAD           = "IDC_TEXT_RANK_BAD";
            constexpr static auto IDC_TEXT_RANK_MISS          = "IDC_TEXT_RANK_MISS";
            constexpr static auto IDC_TEXT_RANK_MAX_COMBO     = "IDC_TEXT_RANK_MAX_COMBO";
            constexpr static auto IDC_TEXT_RANK_MAX_JAM_COMBO = "IDC_TEXT_RANK_MAX_JAM_COMBO";
            constexpr static auto IDC_TEXT_RANK_POINT         = "IDC_TEXT_RANK_POINT";
        }
    }
}
