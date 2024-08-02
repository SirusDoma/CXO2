#ifndef O2JAM_MODELS_EQUIPMENT_HPP
#define O2JAM_MODELS_EQUIPMENT_HPP

enum class EquipmentType
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
    Amulet,
    Accessories,
    Glove,
    Pants,
    Shoes,
    Keyboard,
    Bass,
    Drum,
    Guitar,
    Jacket,
    Wings,
    MusicalAccessories,
    Pet,
    HairAccessories,
    AttributiveItem,
    NicknameChanger,
    PenaltyResetter,
    BagExpansion,
    Costume
};

enum class RenderPart
{
    SmallPreview,
    LargePreview,
    Body,
    RightArm,
    LeftArm,
    Cape
};
enum class Instrument
{
    None,
    Bass,
    Guitar,
    Keyboard,
    Drum
};

#endif
