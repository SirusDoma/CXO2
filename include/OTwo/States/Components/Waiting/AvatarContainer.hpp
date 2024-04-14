#ifndef OTWO_ROOM_AVATAR_CONTAINER
#define OTWO_ROOM_AVATAR_CONTAINER

#include <OTwo/Data/Room.hpp>

#include <Genode/UI/UiContainer.hpp>

class AvatarContainer : public Gx::UiContainer
{
public:
    AvatarContainer(const Gx::UiContainer &&other) noexcept;

    void Initialize() override;

    const PlayerData *GetPlayer() const;
    void SetPlayer(const PlayerData &player);

    void Invalidate() override;

private:
    const PlayerData *m_player;
};

#endif