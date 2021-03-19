#ifndef LABEL_METADATA_HPP
#define LABEL_METADATA_HPP

#include <O2/IO/Metadata/TransformMetadata.hpp>

struct LabelMetadata : TransformMetadata
{
public:
    std::string String;
    unsigned int FontSize;
};

#endif
