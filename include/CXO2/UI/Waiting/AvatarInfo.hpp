#pragma once

#include <CXO2/Models/Room.hpp>

#include <Genode/UI/UiContainer.hpp>
#include <Genode/UI/Gauge.hpp>

#include <unordered_map>

namespace Cx
{
    class Avatar;
    class AvatarInfo : public Gx::UiContainer
    {
    public:
        AvatarInfo() = default;

        void Initialize() override;

        Avatar* GetAvatar() const;
        Gx::Gauge* GetLifeBar() const;
        const Room::Slot* GetSlot() const;
        const sf::Color& GetTeamColor(Room::Team team);
        sf::Color ResolveTeamColor() const;

        void SetSlot(const Room::Slot& slot);

        void RegisterTeamColor(Room::Team team, const sf::Color& color);

        void Reset();

        void Invalidate() override;

    private:
        using ColorMap = std::unordered_map<Room::Team, sf::Color>;

        const Room::Slot* m_slot = nullptr;
        ColorMap  m_teamColors;
    };
}
