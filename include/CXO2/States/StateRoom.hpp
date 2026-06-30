#pragma once

#include <CXO2/States/State.hpp>
#include <Genode/Audio/AudioMixer.hpp>

namespace Cx
{
    enum class GameMode : std::uint8_t;
    enum class RoomTransitionEventType : std::uint8_t;

    struct Room;
    struct RoomInfo;

    struct RoomCreatedEventData;
    struct RoomMusicChangedEventData;
    struct RoomStateChangedEventData;
    struct RoomTitleChangedEventData;
    struct RoomUserCountChangedEventData;
    struct RoomRemovedEventData;

    class GameContext;
    class ItemFactory;
    class RoomContext;
    class RoomService;
    class CharacterService;
    class SessionContext;
    class StateRoom : public State
    {
    public:
        StateRoom(Gx::AudioMixer& mixer, SessionContext& session, RoomContext& room, GameContext& game, RoomService& roomService, CharacterService& charService, ItemFactory& items);

        void Initialize() override;
        void Initialize(RoomTransitionEventType evType);

        void OnRoomCreated(const RoomCreatedEventData& ev);
        void OnRoomMusicChanged(const RoomMusicChangedEventData& ev);
        void OnRoomStateChanged(const RoomStateChangedEventData& ev);
        void OnRoomTitleChanged(const RoomTitleChangedEventData& ev);
        void OnRoomUserCountChanged(const RoomUserCountChangedEventData& ev);
        void OnRoomRemoved(const RoomRemovedEventData& ev);

    private:
        void LoadCharacterInfo();
        void LoadChannelInfo();

        void CreateRoom(
            const sf::String& title,
            GameMode mode,
            const std::string& password,
            unsigned int minLevelLimit,
            unsigned int maxLevelLimit
        );

        void JoinRoom(const RoomInfo& room);

        void OnCreateRoomButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnQuickJoinRoomButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnShowAllButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnWaitingButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnRoomLeftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnRoomRightButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnMusicShopButtonClicked(Gx::Control& sender, Gx::Control::Event& ev) const;
        void OnItemShopButtonClicked(Gx::Control& sender, Gx::Control::Event& ev) const;
        void OnMyRoomButtonClicked(Gx::Control& sender, Gx::Control::Event& ev) const;
        void OnBulletinButtonClicked(Gx::Control& sender, Gx::Control::Event& ev) const;

        void OnOptionButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnTutorialButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnBackButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        Gx::AudioMixer& m_mixer;
        CharacterService& m_charService;
        RoomService& m_service;
        SessionContext& m_session;
        RoomContext& m_room;
        GameContext& m_game;
        ItemFactory& m_items;

        bool m_busy{false};
    };
}
