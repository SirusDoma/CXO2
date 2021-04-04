#ifndef O2JAM_METADATA_OJN_HPP
#define O2JAM_METADATA_OJN_HPP

#include <SFML/System/String.hpp>
#include <O2/Metadata/Chart/ChartMetadata.hpp>

enum O2ChartGenre
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

struct O2ChartMetadata
{
public:
    int Id;
    sf::String Signature;
    float EncodingVersion;
    O2ChartGenre Genre;
    float BPM;
    short LevelEx;
    short LevelNx;
    short LevelHx;
    short Padding;
    int EventCountEx;
    int EventCountNx;
    int EventCountHx;
    int NoteCountEx;
    int NoteCountNx;
    int NoteCountHx;
    int MeasureCountEx;
    int MeasureCountNx;
    int MeasureCountHx;
    int BlockCountEx;
    int BlockCountNx;
    int BlockCountHx;
    short OldEncodingVersion;
    short OldSongId;
    sf::String OldGenre;
    int ThumbnailSize;
    int FileVersion;
    sf::String Title;
    sf::String Artist;
    sf::String NoteArranger;
    sf::String OJM;
    int CoverSize;
    int DurationEx;
    int DurationNx;
    int DurationHx;
    int BlockOffsetEx;
    int BlockOffsetNx;
    int BlockOffsetHx;
    int CoverOffset;

    ChartMetadata ToChartMetadata(Difficulty difficulty)
    {
        sf::String genre;
        switch (Genre)
        {
            case O2ChartGenre::Ballad:      genre = "Ballad";      break;
            case O2ChartGenre::Rock:        genre = "Rock";        break;
            case O2ChartGenre::Dance:       genre = "Dance";       break;
            case O2ChartGenre::Techno:      genre = "Techno";      break;
            case O2ChartGenre::HipHop:      genre = "HipHop";      break;
            case O2ChartGenre::Soul:        genre = "Soul";        break;
            case O2ChartGenre::Jazz:        genre = "Jazz";        break;
            case O2ChartGenre::Funk:        genre = "Funk";        break;
            case O2ChartGenre::Classical:   genre = "Classical";   break;
            case O2ChartGenre::Traditional: genre = "Traditional"; break;
            default:                        genre = "Etc.";        break;
        }

        unsigned int level = 0;
        switch (difficulty)
        {
            case Difficulty::Easy:   level = LevelEx; break;
            case Difficulty::Normal: level = LevelNx; break;
            case Difficulty::Hard:
            case Difficulty::Master:
                level = LevelHx;
                break;
        }

        return ChartMetadata{Title, Artist, NoteArranger, LevelEx, genre, level};
    }
}

#endif
