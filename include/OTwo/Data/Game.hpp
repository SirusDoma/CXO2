#ifndef O2JAM_GAME_DATA_HPP
#define O2JAM_GAME_DATA_HPP

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <Genode/Utilities/StringHelper.hpp>
#include <Genode/System/Primitives.hpp>
#include <Genode/IO/ResourceLoader.hpp>
#include <Genode/IO/ResourceLoaderFactory.hpp>
#include <Genode/IO/FileSystem/FileSystem.hpp>
#include <OTwo/Metadata/Chart/ChartMetadataView.hpp>

const float SupportedHiSpeeds[] = {0.5f, 1.f, 1.5f, 2.f, 2.5f, 3.f, 3.5f, 4.f, 4.5f, 5.f, 6.f, 8.f };

const float XrSpeed = -1.0f;
const float TdSpeed = -2.0f;

enum class GameMode
{
    Single,
    Versus,
    Album,
    Couple,
    Live,
    Jam
};

enum class  Difficulty
{
    EX = 0,
    NX = 1,
    HX = 2,
    MX = 3
};
#endif
