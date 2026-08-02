#pragma once

#include <CXO2/Metadata/UI/DialogMetadata.hpp>

namespace Cx
{
    struct SelectMusicDialogMetadata : public DialogMetadata
    {
        struct
        {
            sf::Color Random   = sf::Color();
            sf::Color Notice   = sf::Color();
            sf::Color Disabled = sf::Color();
            sf::Color Warning  = sf::Color();
        } Colors;
    };
}
