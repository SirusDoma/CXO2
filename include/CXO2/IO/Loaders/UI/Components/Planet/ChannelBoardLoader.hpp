#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/UI/Planet/ChannelBoard.hpp>

namespace Cx
{
    class ChannelBoardLoader : public ResourceLoader<ChannelBoard>
    {
    public :
        ChannelBoardLoader() = default;

        Gx::ResourcePtr<ChannelBoard> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<ChannelBoard> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
