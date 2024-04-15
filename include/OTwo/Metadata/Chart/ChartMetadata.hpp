#ifndef O2JAM_O2CHART_METADATA_HPP
#define O2JAM_O2CHART_METADATA_HPP

#include <OTwo/Data/Game.hpp>
#include <OTwo/Metadata/Chart/ChartMetadataView.hpp>

#include <Genode/IO/FileSystem/FileSystem.hpp>
#include <Genode/IO/ResourceLoaderFactory.hpp>
#include <Genode/IO/ResourceLoader.hpp>
#include <Genode/System/Primitives.hpp>
#include <Genode/Utilities/StringHelper.hpp>

#include <SFML/Graphics/Texture.hpp>
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
    static const Gx::Uint32 Size = 300;

    Gx::Uint32 ID;
    char Signature[4];
    float EncodingVersion;
    ::Genre Genre;
    float BPM;
    Gx::Uint16 LevelEx;
    Gx::Uint16 LevelNx;
    Gx::Uint16 LevelHx;
    Gx::Int16 Unk1;
    Gx::Uint32 EventCountEx;
    Gx::Uint32 EventCountNx;
    Gx::Uint32 EventCountHx;
    Gx::Uint32 NoteCountEx;
    Gx::Uint32 NoteCountNx;
    Gx::Uint32 NoteCountHx;
    Gx::Uint32 MeasureCountEx;
    Gx::Uint32 MeasureCountNx;
    Gx::Uint32 MeasureCountHx;
    Gx::Uint32 BlockCountEx;
    Gx::Uint32 BlockCountNx;
    Gx::Uint32 BlockCountHx;
    Gx::Int16 OldEncodingVersion;
    Gx::Int16 OldSongID;
    char OldGenre[20];
    Gx::Uint32 ThumbnailSize;
    Gx::Uint32 FileVersion;
    char Title[64];
    char Artist[32];
    char NoteArranger[32];
    char OJM[32];
    Gx::Uint32 CoverSize;
    Gx::Uint32 DurationEx;
    Gx::Uint32 DurationNx;
    Gx::Uint32 DurationHx;
    Gx::Uint32 BlockOffsetEx;
    Gx::Uint32 BlockOffsetNx;
    Gx::Uint32 BlockOffsetHx;
    Gx::Uint32 CoverOffset;

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

#endif
