#ifndef O2JAM_CHANNEL_BOARD_METADATA_HPP
#define O2JAM_CHANNEL_BOARD_METADATA_HPP

#include <OTwo/Metadata/UI/ImageMetadata.hpp>

struct ChannelBoardMetadata : public ImageMetadata
{
    unsigned int ChannelCount;
};

#endif
