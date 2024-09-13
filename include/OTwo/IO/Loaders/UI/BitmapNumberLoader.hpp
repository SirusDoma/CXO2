#ifndef O2JAM_BITMAP_NUMBER_LOADER_HPP
#define O2JAM_BITMAP_NUMBER_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <Genode/UI/BitmapNumber.hpp>

class BitmapNumberLoader : public ResourceLoader<Gx::BitmapNumber>
{
public :
    BitmapNumberLoader() = default;

    Gx::ResourcePtr<Gx::BitmapNumber> LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const override;
    Gx::ResourcePtr<Gx::BitmapNumber> LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const override;
};


#endif