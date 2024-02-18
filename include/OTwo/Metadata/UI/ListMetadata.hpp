#ifndef O2JAM_LIST_METADATA_HPP
#define O2JAM_LIST_METADATA_HPP

#include <OTwo/Metadata/TransformMetadata.hpp>

struct ListMetadata : public TransformMetadata
{
    int   VerticalCount;
    int   HorizontalCount;
    float VerticalSpacing;
    float HorizontalSpacing;
};

#endif
