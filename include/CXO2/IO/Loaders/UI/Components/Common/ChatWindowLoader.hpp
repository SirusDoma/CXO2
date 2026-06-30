#pragma once

#include <CXO2/IO/ResourceLoader.hpp>
#include <CXO2/UI/Common/ChatWindow.hpp>

namespace Cx
{
    class ChatWindowLoader : public ResourceLoader<ChatWindow>
    {
    public :
        ChatWindowLoader() = default;

        Gx::ResourcePtr<ChatWindow> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
        Gx::ResourcePtr<ChatWindow> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
    };
}
