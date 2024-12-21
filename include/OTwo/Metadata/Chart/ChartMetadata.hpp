#pragma once

#include <OTwo/Models/Game.hpp>
#include <OTwo/Metadata/Chart/ChartMetadataView.hpp>

#include <Genode/Utilities/StringHelper.hpp>

#include <SFML/System/Time.hpp>

enum class Genre
{
    Ballad      = 0,
    Rock        = 1,
    Dance       = 2,
    Techno      = 3,
    HipHop      = 4,
    Soul        = 5,
    Jazz        = 6,
    Funk        = 7,
    Classical   = 8,
    Traditional = 9,
    Etc         = 10
};

struct ChartMetadata
{
    static constexpr std::uint32_t Size = 300;

    std::uint32_t ID;
    char Signature[4];
    float EncodingVersion;
    ::Genre Genre;
    float BPM;
    std::uint16_t LevelEx;
    std::uint16_t LevelNx;
    std::uint16_t LevelHx;
    std::int16_t Unk1;
    std::uint32_t EventCountEx;
    std::uint32_t EventCountNx;
    std::uint32_t EventCountHx;
    std::uint32_t NoteCountEx;
    std::uint32_t NoteCountNx;
    std::uint32_t NoteCountHx;
    std::uint32_t MeasureCountEx;
    std::uint32_t MeasureCountNx;
    std::uint32_t MeasureCountHx;
    std::uint32_t BlockCountEx;
    std::uint32_t BlockCountNx;
    std::uint32_t BlockCountHx;
    std::int16_t OldEncodingVersion;
    std::int16_t OldSongID;
    char OldGenre[20];
    std::uint32_t ThumbnailSize;
    std::uint32_t FileVersion;
    char Title[64];
    char Artist[32];
    char NoteArranger[32];
    char OJM[32];
    std::uint32_t CoverSize;
    std::uint32_t DurationEx;
    std::uint32_t DurationNx;
    std::uint32_t DurationHx;
    std::uint32_t BlockOffsetEx;
    std::uint32_t BlockOffsetNx;
    std::uint32_t BlockOffsetHx;
    std::uint32_t CoverOffset;

    std::string Source;

    ChartMetadataView ToChartMetadataView(Difficulty difficulty) const
    {
        sf::String genre;
        switch (Genre)
        {
            case Genre::Ballad:      genre = "Ballad";      break;
            case Genre::Rock:        genre = "Rock";        break;
            case Genre::Dance:       genre = "Dance";       break;
            case Genre::Techno:      genre = "Techno";      break;
            case Genre::HipHop:      genre = "HipHop";      break;
            case Genre::Soul:        genre = "Soul";        break;
            case Genre::Jazz:        genre = "Jazz";        break;
            case Genre::Funk:        genre = "Funk";        break;
            case Genre::Classical:   genre = "Classical";   break;
            case Genre::Traditional: genre = "Traditional"; break;
            default:                 genre = "Etc.";        break;
        }

        unsigned int level = 0;
        unsigned int duration = 0;
        unsigned int noteCount = 0;
        auto diff = std::string();
        switch (difficulty)
        {
            case Difficulty::EX:
                diff = "EX";
                level = LevelEx;
                duration = DurationEx;
                noteCount = NoteCountEx;
                break;
            case Difficulty::NX:
                diff = "NX";
                level = LevelNx;
                duration = DurationNx;
                noteCount = NoteCountNx;
                break;
            case Difficulty::HX:
            case Difficulty::MX:
                diff = "HX";
                level = LevelHx;
                duration = DurationHx;
                noteCount = NoteCountHx;
                break;
        }

        return ChartMetadataView{
            /* .ID           = */ std::to_string(ID),
            /* .New          = */ false,
            /* .Title        = */ Gx::StringHelper::Trim(Title),
            /* .Artist       = */ Gx::StringHelper::Trim(Artist),
            /* .NoteDesigner = */ Gx::StringHelper::Trim(NoteArranger),
            /* .BPM          = */ BPM,
            /* .Genre        = */ genre,
            /* .Difficulty   = */ diff,
            /* .Level        = */ level,
            /* .NoteCount    = */ noteCount,
            /* .Duration     = */ sf::seconds(static_cast<float>(duration)),
            /* .Source       = */ Source
        };
    }
};
