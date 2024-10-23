#ifndef O2JAM_EQUALIZER_METADATA_HPP
#define O2JAM_EQUALIZER_METADATA_HPP

#include <OTwo/Metadata/UI/UiContainerMetadata.hpp>

struct EqualizerMetadata : public UiContainerMetadata
{
    unsigned int BufferSampleCount;
};

#endif
