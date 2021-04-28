#ifndef O2JAM_CHANNEL_BOARD_LOADER_HPP
#define O2JAM_CHANNEL_BOARD_LOADER_HPP

#include <O2/Loaders/O2JamResourceLoader.hpp>

class ChannelBoard;
class ChannelBoardLoader : public O2JamResourceLoader<ChannelBoard>
{
public:
    ChannelBoardLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<ChannelBoard> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};

#endif
