#ifndef O2JAM_CHANNEL_BUTTON_LOADER_HPP
#define O2JAM_CHANNEL_BUTTON_LOADER_HPP

#include <O2/Loaders/O2JamResourceLoader.hpp>

class ChannelButton;
class ChannelButtonLoader : public O2JamResourceLoader<ChannelButton>
{
public:
    ChannelButtonLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<ChannelButton> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};

#endif
