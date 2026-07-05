#pragma once

#include <CXO2/States/State.hpp>
#include <Genode/Audio/AudioMixer.hpp>

#include <CXO2/Messages/MessageEnvelope.hpp>
#include <CXO2/Messages/Responses/CharacterInfoResponse.hpp>
#include <CXO2/Messages/Responses/CreateRoomResponse.hpp>
#include <CXO2/Messages/Responses/JoinRoomResponse.hpp>
#include <CXO2/Messages/Responses/RoomListResponse.hpp>
#include <CXO2/Messages/Responses/UserListResponse.hpp>

#include <CXO2/Messages/Events/RoomCreatedEventData.hpp>
#include <CXO2/Messages/Events/RoomMusicChangedEventData.hpp>
#include <CXO2/Messages/Events/RoomRemovedEventData.hpp>
#include <CXO2/Messages/Events/RoomStateChangedEventData.hpp>
#include <CXO2/Messages/Events/RoomTitleChangedEventData.hpp>
#include <CXO2/Messages/Events/RoomUserCountChangedEventData.hpp>

namespace Cx
{
    enum class GameMode : std::uint8_t;
    enum class RoomTransitionEventType : std::uint8_t;

    struct CreateRoomRequest;

    struct Room;
    struct RoomInfo;

    class GameContext;
    class ChartMetadata;
    class ItemFactory;
    class RoomContext;
    class ChannelService;
    class CharacterService;
    class SessionContext;
    class StateRoom : public State
    {
    public:
        StateRoom(Gx::AudioMixer& mixer, SessionContext& session, RoomContext& room, GameContext& game, ChannelService& roomService, CharacterService& charService, ItemFactory& items);

        void Initialize() override;
        void Initialize(RoomTransitionEventType evType);

    private:
        void SyncCharacterInfo();
        void SyncChannelInfo();
        void RegisterMessageEvents();

        void CreateRoom(
            const sf::String& title,
            GameMode mode,
            const std::string& password,
            unsigned int minLevelLimit,
            unsigned int maxLevelLimit
        );

        void JoinRoom(const RoomInfo& room);

        void OnCharacterInfoLoad(const MessageEnvelope<CharacterInfoResponse>& ev);
        void OnRoomListLoad(const MessageEnvelope<RoomListResponse>& ev);
        void OnUserListLoad(const MessageEnvelope<UserListResponse>& ev);

        void OnCreateRoomResponded(const CreateRoomRequest& request, const ChartMetadata& music, const MessageEnvelope<CreateRoomResponse>& ev);
        void OnJoinRoomResponded(const RoomInfo& room, const MessageEnvelope<JoinRoomResponse>& ev);

        void OnRoomCreated(const MessageEnvelope<RoomCreatedEventData>& ev);
        void OnRoomMusicChanged(const MessageEnvelope<RoomMusicChangedEventData>& ev);
        void OnRoomStateChanged(const MessageEnvelope<RoomStateChangedEventData>& ev);
        void OnRoomTitleChanged(const MessageEnvelope<RoomTitleChangedEventData>& ev);
        void OnRoomUserCountChanged(const MessageEnvelope<RoomUserCountChangedEventData>& ev);
        void OnRoomRemoved(const MessageEnvelope<RoomRemovedEventData>& ev);

        void OnCreateRoomButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnRoomButtonClicked(const RoomInfo& room);
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
        ChannelService& m_service;
        SessionContext& m_session;
        RoomContext& m_room;
        GameContext& m_game;
        ItemFactory& m_items;

        bool m_busy{false};
    };
}
