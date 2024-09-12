#ifndef O2JAM_CHANNEL_BOARD_LOADER_HPP
#define O2JAM_CHANNEL_BOARD_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/UI/Planet/ChannelBoard.hpp>

class ChannelBoardLoader : public ResourceLoader<ChannelBoard>
{
public :
    ChannelBoardLoader() = default;

    Gx::ResourcePtr<ChannelBoard> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<ChannelBoard> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};

#endif
