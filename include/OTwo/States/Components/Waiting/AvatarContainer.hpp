#ifndef OTWO_ROOM_AVATAR_CONTAINER
#define OTWO_ROOM_AVATAR_CONTAINER

#include <OTwo/Data/Room.hpp>

#include <Genode/UI/UiContainer.hpp>

class AvatarContainer : public Gx::UiContainer
{
public:
    AvatarContainer(const Gx::UiContainer &&other) noexcept;

    void Initialize() override;

    const Player *GetPlayer() const;
    void SetPlayer(const Player &player);

    void Invalidate() override;

private:
    const Player *m_player;
};

#endif