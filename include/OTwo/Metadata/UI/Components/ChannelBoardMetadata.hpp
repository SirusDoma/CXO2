#ifndef O2JAM_CHANNEL_BOARD_METADATA_HPP
#define O2JAM_CHANNEL_BOARD_METADATA_HPP

#include <OTwo/Metadata/UI/ImageMetadata.hpp>

struct ChannelBoardMetadata : public ImageMetadata
{
    ChannelBoardMetadata() {};
    virtual ~ChannelBoardMetadata() {};

    unsigned int ChannelCount;
};

#endif
