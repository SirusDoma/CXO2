#ifndef O2JAM_AVATAR_LOADER_HPP
#define O2JAM_AVATAR_LOADER_HPP

#include <OTwo/Character/Avatar.hpp>
#include <OTwo/Loaders/O2JamResourceLoader.hpp>

class AvatarMetadata;
class AvatarLoader : public O2JamResourceLoader<Avatar>
{
public :
    AvatarLoader();

    virtual std::unique_ptr<Gx::ResourceMetadata> LoadMetadata(const void* data, std::size_t size) const;
    virtual Gx::ResourcePtr<Avatar> Load(const Gx::ResourceMetadata& metadata, const Gx::ResourceContext& context = Gx::ResourceContext()) const;
};

#endif
