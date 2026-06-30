#pragma once

namespace Cx::StringTable::Identifiers::Sound
{
    namespace Channel
    {
        constexpr static auto BGM    = "BGM";
        constexpr static auto SFX    = "SFX";
        constexpr static auto BGTest = "BGTest";
        constexpr static auto EFTest = "EFTest";
    }

    namespace BGM
    {
        constexpr static auto BG_LOGO       = "BGM:1007"; // "BGM/bgLogo.ogg"
        constexpr static auto BG_LOGIN      = "BGM:1000"; // "BGM/bgLogin.ogg"
        constexpr static auto BG_MAIN_ROOM  = "BGM:1008"; // "BGM/bgMainRoom.ogg"
        constexpr static auto BG_ROOM       = "BGM:1002"; // "BGM/bgRoom.ogg"
        constexpr static auto BG_MY_ROOM    = "BGM:1004"; // "BGM/bgMyroom.ogg"
        constexpr static auto BG_ITEM_SHOP  = "BGM:1003"; // "BGM/bgItemShop.ogg"
        constexpr static auto BG_MUSIC_SHOP = "BGM:1009"; // "BGM/bgMusicShop.ogg"
        constexpr static auto BG_WAITING    = "BGM:1005"; // "BGM/bgWaiting.ogg"
        constexpr static auto BG_RESULT     = "BGM:1006"; // "BGM/bgResult.ogg"
    }

    namespace Effects
    {
        constexpr static auto PLANET_BEEP  = "Planet/BEEppq";
        constexpr static auto MUSIC_VOLUME = "bgEffect:1000"; // "bgEffect/MusicVolumn"
        constexpr static auto SAMPLE_SONG  = "bgEffect:1000"; // "bgEffect/SampleSong.ogg"

        constexpr static auto EF_01       = "bgEffect:00"; // "bgEffect/01"
        constexpr static auto EF_02       = "bgEffect:01"; // "bgEffect/02"
        constexpr static auto EF_03       = "bgEffect:02"; // "bgEffect/03"
        constexpr static auto EF_06       = "bgEffect:05"; // "bgEffect/06"
        constexpr static auto EF_07       = "bgEffect:06"; // "bgEffect/07"
        constexpr static auto EF_10       = "bgEffect:09"; // "bgEffect/10"
        constexpr static auto EF_11       = "bgEffect:10"; // "bgEffect/11"
        constexpr static auto EF_14       = "bgEffect:13"; // "bgEffect/14"
        constexpr static auto EF_15       = "bgEffect:14"; // "bgEffect/15"
        constexpr static auto EF_19_1     = "bgEffect:19"; // "bgEffect/19_1"
        constexpr static auto EF_19_2     = "bgEffect:20"; // "bgEffect/19_2"
        constexpr static auto EF_24_      = "bgEffect:28"; // "bgEffect/24_"
        constexpr static auto EF_27_dress = "bgEffect:31"; // "bgEffect/27_dress"
        constexpr static auto EF_25       = "bgEffect:24"; // "bgEffect/25"
        constexpr static auto EF_33       = "bgEffect:32"; // "bgEffect/33"
        constexpr static auto EF_34       = "bgEffect:33"; // "bgEffect/34"
        constexpr static auto EF_35       = "bgEffect:34"; // "bgEffect/35"
        constexpr static auto EF_36       = "bgEffect:35"; // "bgEffect/36"
        constexpr static auto EF_38       = "bgEffect:38"; // "bgEffect/38"
    }

    namespace Speech
    {
        constexpr static auto NPC_1 = "O2PlanetNPC:0"; // "O2PlanetNPC/1"
        constexpr static auto NPC_5 = "O2PlanetNPC:3"; // "O2PlanetNPC/5"
        constexpr static auto NPC_6 = "O2PlanetNPC:4"; // "O2PlanetNPC/6"
        constexpr static auto NPC_7 = "O2PlanetNPC:5"; // "O2PlanetNPC/7"
    }
}
