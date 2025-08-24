#pragma once

#ifdef __APPLE__
#include <Genode/Utilities/Reflection.hpp>
#include <tuple>

// TODO: Fix this mess, perhaps consider boost.di?
namespace sf { class Sound; class SoundBuffer; }
struct SpriteSheet;

namespace Gx
{
    // On Clang, the friend-injection pattern may fail due to redefinition error
    // Need to be fixed since this may happen to other types in the future
    template <>
    struct ConstructorDescriptorTraits<sf::Sound>
    {
        using type = std::tuple<sf::SoundBuffer&>;
    };

    template <>
    struct Constructible<SpriteSheet>
    {
        static constexpr bool value = false;
    };

    template <>
    struct Constructible<sf::Sound>
    {
        static constexpr bool value = true;
    };
}

#endif

