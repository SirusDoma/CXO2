#ifndef O2JAM_ROOM_BUTTON_LOADER_HPP
#define O2JAM_ROOM_BUTTON_LOADER_HPP

#include <OTwo/IO/ResourceLoader.hpp>
#include <OTwo/States/Components/Room/RoomButton.hpp>

class RoomButtonLoader : public ResourceLoader<RoomButton>
{
public :
    RoomButtonLoader() = default;

    Gx::ResourcePtr<RoomButton> LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const override;
    Gx::ResourcePtr<RoomButton> LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const override;
};

#endif
