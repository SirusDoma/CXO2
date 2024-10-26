#pragma once

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/UI/Common/ChatWindow.hpp>

class ChatWindowLoader : public ResourceLoader<ChatWindow>
{
public :
    ChatWindowLoader() = default;

    Gx::ResourcePtr<ChatWindow> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<ChatWindow> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};
