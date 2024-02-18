#ifndef O2JAM_LABEL_LOADER_HPP
#define O2JAM_LABEL_LOADER_HPP

#include <OTwo/Loaders/O2JamResourceLoader.hpp>
#include <OTwo/Metadata/UI/LabelMetadata.hpp>
#include <Genode/UI/Label.hpp>

class LabelLoader : public O2JamResourceLoader<Gx::Label>
{
public :
    LabelLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<Gx::Label> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;

    static void ParseLabel(Json attributes, LabelMetadata& metadata);
};

#endif