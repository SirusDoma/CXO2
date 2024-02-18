#ifndef O2JAM_CHAT_WINDOW_LOADER_HPP
#define O2JAM_CHAT_WINDOW_LOADER_HPP

#include <OTwo/Loaders/O2JamResourceLoader.hpp>
#include <OTwo/States/Components/Common/ChatWindow.hpp>

class ChatWindowLoader : public O2JamResourceLoader<ChatWindow>
{
public :
    ChatWindowLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<ChatWindow> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;

};

#endif