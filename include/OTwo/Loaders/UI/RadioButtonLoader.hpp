#ifndef O2JAM_RADIO_BUTTON_LOADER_HPP
#define O2JAM_RADIO_BUTTON_LOADER_HPP

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/UI/RadioButton.hpp>

#include <OTwo/Loaders/O2JamResourceLoader.hpp>

class RadioButtonLoader : public O2JamResourceLoader<Gx::RadioButton>
{
public :
    RadioButtonLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void* data, std::size_t size) const;
    virtual Gx::ResourcePtr<Gx::RadioButton> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};


#endif