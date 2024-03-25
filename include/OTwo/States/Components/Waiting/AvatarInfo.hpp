#ifndef O2JAM_WAITING_AVATAR_INFO_HPP
#define O2JAM_WAITING_AVATAR_INFO_HPP

#include <OTwo/Data/Room.hpp>

#include <Genode/UI/UiContainer.hpp>

#include <unordered_map>

class AvatarInfo : public Gx::UiContainer
{
public:
    AvatarInfo() = default;

    void Initialize() override;

    RoomMember *GetMember() const;
    const sf::Color &GetTeamColor(RoomTeam team);

    void SetMember(RoomMember &member);
    void RegisterTeamColor(RoomTeam team, const sf::Color &color);

    void Reset();

    void Invalidate() override;

private:
    using ColorMap = std::unordered_map<RoomTeam, sf::Color>;

    RoomMember *m_member;
    ColorMap    m_teamColors;

};

#endif