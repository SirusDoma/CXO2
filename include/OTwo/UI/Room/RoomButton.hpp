#pragma once

#include <OTwo/Messages/RoomInfo.hpp>
#include <OTwo/Metadata/Chart/ChartMetadata.hpp>

#include <Genode/UI/Image.hpp>

class SessionContext;

class RoomButton : public Gx::Image
{
public:
    RoomButton(const SessionContext& session);

    void Initialize() override;
    sf::FloatRect GetLocalBounds() const override;

    bool IsActive() const;

    const RoomInfo& GetRoomInfo() const;
    void SetRoomInfo(const RoomInfo& data);
    void Reset();

private:
    void OnMouseMoved(const sf::Event::MouseMoved& ev) override;
    void Invalidate() override;

    RoomInfo m_room;

    ChartMetadata m_music;
    const std::vector<ChartMetadata>& m_musicList;

    Gx::Image* m_hover;

    unsigned int m_randomStart{}, m_randomEnd{};
    bool m_active;
};
