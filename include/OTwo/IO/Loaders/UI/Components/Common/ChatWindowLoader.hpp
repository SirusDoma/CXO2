#ifndef O2JAM_CHAT_WINDOW_LOADER_HPP
#define O2JAM_CHAT_WINDOW_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/States/Components/Common/ChatWindow.hpp>

class ChatWindowLoader : public ResourceLoader<ChatWindow>
{
public :
    ChatWindowLoader() = default;

    Gx::ResourcePtr<ChatWindow> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<ChatWindow> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;
};

#endif