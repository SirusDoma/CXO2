#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/UI/Planet/ChannelBoard.hpp>

class ChannelBoardLoader : public ResourceLoader<ChannelBoard>
{
public :
    ChannelBoardLoader() = default;

    Gx::ResourcePtr<ChannelBoard> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<ChannelBoard> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};
