#pragma once

#include <OTwo/Models/Room.hpp>

#include <Genode/UI/UiContainer.hpp>
#include <Genode/UI/Gauge.hpp>

#include <unordered_map>

class Avatar;
class RoomSlot;
class AvatarInfo : public Gx::UiContainer
{
public:
    AvatarInfo() = default;

    void Initialize() override;

    Avatar* GetAvatar() const;
    Gx::Gauge* GetLifeBar() const;
    RoomSlot* GetSlot() const;
    const sf::Color& GetTeamColor(RoomTeam team);

    void SetSlot(RoomSlot& slot);

    void RegisterTeamColor(RoomTeam team, const sf::Color& color);

    void Reset();

    void Invalidate() override;

private:
    using ColorMap = std::unordered_map<RoomTeam, sf::Color>;

    RoomSlot* m_slot;
    ColorMap  m_teamColors;

};
