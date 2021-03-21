#ifndef REPEATER_METADATA_HPP
#define REPEATER_METADATA_HPP

#include <O2/IO/Metadata/TransformMetadata.hpp>

struct RepeaterMetadata : TransformMetadata
{
public:
    int VerticalCount, HorizontalCount;
    float VerticalSpacing, HorizontalSpacing;
};

#endif
