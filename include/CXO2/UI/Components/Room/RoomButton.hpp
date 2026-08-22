#pragma once

#include <CXO2/Models/Room.hpp>
#include <CXO2/Metadata/Chart/ChartMetadata.hpp>

#include <CXO2/UI/Image.hpp>

namespace Cx
{
    class SessionContext;

    class RoomButton : public Image
    {
    public:
        RoomButton(const SessionContext& session);

        void Initialize() override;
        sf::FloatRect GetLocalBounds() const override;

        bool IsActive() const;

        const Room& GetRoomInfo() const;
        void SetRoomInfo(const Room& data);
        void Reset();

    private:
        void OnMouseMoved(const sf::Event::MouseMoved& ev) override;
        void Invalidate() override;

        Room m_room;

        ChartMetadata m_music;
        const std::vector<ChartMetadata>& m_musicList;

        Image* m_hover;

        unsigned int m_randomStart{}, m_randomEnd{};
        bool m_active;
    };
}
