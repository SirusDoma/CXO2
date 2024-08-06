#ifndef O2JAM_CHAT_PANEL_LOADER_HPP
#define O2JAM_CHAT_PANEL_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/UI/Common/ChatPanel.hpp>

class ChatPanelLoader : public ResourceLoader<ChatPanel>
{
public :
    ChatPanelLoader() = default;

    Gx::ResourcePtr<ChatPanel> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<ChatPanel> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;
};

#endif