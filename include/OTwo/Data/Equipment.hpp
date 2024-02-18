#ifndef O2JAM_EQUIPMENT_HPP
#define O2JAM_EQUIPMENT_HPP

namespace Equipment
{
    enum class Type
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
        Armlet,
        Accessories,
        Glove,
        Pants,
        Shoes,
        Piano,
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
        Piano,
        Drum
    };
}

#endif
