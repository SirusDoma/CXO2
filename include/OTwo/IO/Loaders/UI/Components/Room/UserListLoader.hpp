#ifndef O2JAM_USER_LIST_LOADER_HPP
#define O2JAM_USER_LIST_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/UI/Room/UserList.hpp>

class UserListLoader : public ResourceLoader<UserList>
{
public :
    UserListLoader() = default;

    Gx::ResourcePtr<UserList> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<UserList> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;
};

#endif
