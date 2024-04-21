#ifndef O2JAM_LABEL_METADATA_HPP
#define O2JAM_LABEL_METADATA_HPP

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>
#include <Genode/UI/Label.hpp>

struct LabelMetadata : public TransformMetadata
{
    std::string          String;
    unsigned int         FontSize;
    bool                 Bold;
    bool                 Italic;
    bool                 Underlined;
    float                OutlineThickness;
    sf::Color            Color;
    sf::Color            OutlineColor;
    Gx::Label::Alignment Alignment;
};

#endif
