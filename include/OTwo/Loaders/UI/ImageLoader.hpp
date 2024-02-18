#ifndef O2JAM_IMAGE_LOADER_HPP
#define O2JAM_IMAGE_LOADER_HPP

#include <OTwo/Loaders/O2JamResourceLoader.hpp>
#include <OTwo/Metadata/UI/ImageMetadata.hpp>

class ImageLoader : public O2JamResourceLoader<Gx::Image>
{
public :
    ImageLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void *data, std::size_t size) const;
    virtual Gx::ResourcePtr<Gx::Image> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;

    static void ParseImage(Json attributes, ImageMetadata &metadata);
};

#endif