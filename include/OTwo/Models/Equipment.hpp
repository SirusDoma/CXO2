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
    ClothesAccessories,
    Accessories,
    Gloves,
    Pants,
    Shoes,
    Keyboard,
    Bass,
    Drum,
    Guitar,
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

enum class RenderPart
{
    SmallThumbnail,
    LargeThumbnail,
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
