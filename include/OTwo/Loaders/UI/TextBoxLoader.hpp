#ifndef O2JAM_TEXTBOX_LOADER_HPP
#define O2JAM_TEXTBOX_LOADER_HPP

#include <OTwo/Loaders/O2JamResourceLoader.hpp>
#include <OTwo/Metadata/UI/TextBoxMetadata.hpp>
#include <Genode/UI/TextBox.hpp>

class TextBoxLoader : public O2JamResourceLoader<Gx::TextBox>
{
public :
    TextBoxLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<Gx::TextBox> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};

#endif