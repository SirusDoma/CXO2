#pragma once

#include <CXO2/Metadata/UI/UiContainerMetadata.hpp>

namespace Cx
{
    struct EqualizerMetadata : public UiContainerMetadata
    {
        unsigned int BufferSampleCount;
    };
}
