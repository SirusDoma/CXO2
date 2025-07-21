#pragma once
#include <cstdint>

enum class EquipmentType : std::uint8_t
{
    Body,
    LeftArm,
    RightArm,
    LeftHand,
    RightHand,
    Face,
    Hair,
    Glasses,
    Earrings,
    Necklace,
    ClothesAccessories,
    Accessories,
    Gloves,
    Pants,
    Shoes,
    Keyboard,
    Guitar,
    Drum,
    Bass,
    Top,
    Wings,
    InstrumentAccessories,
    Pet,
    HairAccessories,
    AttributiveItem,
    NicknameChanger,
    PenaltyResetter,
    BagExpansion,
    Costume
};

enum class RenderPart : std::uint8_t
{
    LargeThumbnail,
    SmallThumbnail,
    Body,
    LeftArm,
    RightArm,
    Back
};

enum class Instrument : std::uint8_t
{
    None,
    Guitar,
    Bass,
    Keyboard,
    Drum
};
