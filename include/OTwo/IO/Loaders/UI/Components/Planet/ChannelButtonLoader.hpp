#ifndef O2JAM_CHANNEL_BUTTON_LOADER_HPP
#define O2JAM_CHANNEL_BUTTON_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/UI/Planet/ChannelButton.hpp>

class ChannelButtonLoader : public ResourceLoader<ChannelButton>
{
public :
    ChannelButtonLoader() = default;

    Gx::ResourcePtr<ChannelButton> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<ChannelButton> LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const override;
};


#endif