#ifndef O2JAM_ROOM_CONTAINER_LOADER_HPP
#define O2JAM_ROOM_CONTAINER_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/UI/Room/RoomContainer.hpp>

class RoomContainerLoader : public ResourceLoader<RoomContainer>
{
public :
    RoomContainerLoader() = default;

    Gx::ResourcePtr<RoomContainer> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<RoomContainer> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;
};

#endif
