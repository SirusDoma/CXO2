#ifndef O2JAM_CHECK_BOX_LOADER_HPP
#define O2JAM_CHECK_BOX_LOADER_HPP

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/UI/CheckBox.hpp>

#include <OTwo/Loaders/O2JamResourceLoader.hpp>

class CheckBoxLoader : public O2JamResourceLoader<Gx::CheckBox>
{
public :
    CheckBoxLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void* data, std::size_t size) const;
    virtual Gx::ResourcePtr<Gx::CheckBox> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};


#endif