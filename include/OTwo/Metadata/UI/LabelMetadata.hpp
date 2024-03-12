#ifndef O2JAM_LABEL_METADATA_HPP
#define O2JAM_LABEL_METADATA_HPP

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

struct LabelMetadata : public TransformMetadata
{
    std::string  String;
    unsigned int FontSize;
    float        OutlineThickness;
    sf::Color    Color;
    sf::Color    OutlineColor;
};

#endif
