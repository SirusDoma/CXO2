#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/UI/Components/Planet/ChannelButton.hpp>

namespace Cx
{
    class ChannelButtonLoader : public ResourceLoader<ChannelButton>
    {
    public :
        ChannelButtonLoader() = default;

        Gx::ResourcePtr<ChannelButton> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<ChannelButton> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
