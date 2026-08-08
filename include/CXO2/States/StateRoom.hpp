#pragma once

#include <CXO2/States/State.hpp>
#include <CXO2/Config/GameConfig.hpp>
#include <Genode/Audio/AudioMixer.hpp>

#include <CXO2/Network/MessageEnvelope.hpp>
#include <CXO2/Network/Responses/CharacterInfoResponse.hpp>
#include <CXO2/Network/Responses/CreateRoomResponse.hpp>
#include <CXO2/Network/Responses/JoinRoomResponse.hpp>
#include <CXO2/Network/Responses/RoomListResponse.hpp>
#include <CXO2/Network/Responses/UserListResponse.hpp>

#include <CXO2/Network/Events/RoomCreatedEventData.hpp>
#include <CXO2/Network/Events/RoomMusicChangedEventData.hpp>
#include <CXO2/Network/Events/RoomRemovedEventData.hpp>
#include <CXO2/Network/Events/RoomStateChangedEventData.hpp>
#include <CXO2/Network/Events/RoomTitleChangedEventData.hpp>
#include <CXO2/Network/Events/RoomUserCountChangedEventData.hpp>

namespace Cx
{
    enum class GameMode : std::uint8_t;
    enum class RoomTransitionEventType : std::uint8_t;

    struct CreateRoomRequest;
    struct Room;

    class ChartMetadata;
    class ItemFactory;
    class RoomContext;
    class ChannelService;
    class CharacterService;
    class SessionContext;
    class StateRoom : public State
    {
    public:
        StateRoom(Gx::AudioMixer& mixer, SessionContext& session, RoomContext& room, GameConfig& config, ChannelService& roomService, CharacterService& charService, ItemFactory& items);

        void Initialize() override;
        void Initialize(RoomTransitionEventType evType);
        void ShowChatHelp();

    private:
        void SyncCharacterInfo();
        void SyncChannelInfo();
        void RegisterMessageEvents();

        void CreateRoom(
            sf::String title,
            GameMode mode,
            std::string password,
            unsigned int minLevelLimit,
            unsigned int maxLevelLimit
        );

        void JoinRoom(const Room& room);

        void OnCharacterInfoLoad(const MessageEnvelope<CharacterInfoResponse>& ev);
        void OnRoomListLoad(const MessageEnvelope<RoomListResponse>& ev);
        void OnUserListLoad(const MessageEnvelope<UserListResponse>& ev);

        void OnCreateRoomResponded(const MessageEnvelope<CreateRoomResponse>& ev, const CreateRoomRequest& request, const ChartMetadata& music);
        void OnJoinRoomResponded(const MessageEnvelope<JoinRoomResponse>& ev);

        void OnRoomCreated(const MessageEnvelope<RoomCreatedEventData>& ev);
        void OnRoomMusicChanged(const MessageEnvelope<RoomMusicChangedEventData>& ev);
        void OnRoomStateChanged(const MessageEnvelope<RoomStateChangedEventData>& ev);
        void OnRoomTitleChanged(const MessageEnvelope<RoomTitleChangedEventData>& ev);
        void OnRoomUserCountChanged(const MessageEnvelope<RoomUserCountChangedEventData>& ev);
        void OnRoomRemoved(const MessageEnvelope<RoomRemovedEventData>& ev);

        void OnRoomEntered(const Room& room);

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
        ChannelService& m_service;
        SessionContext& m_session;
        RoomContext& m_room;
        GameConfig& m_config;
        ItemFactory& m_items;

        bool m_busy{false};
    };
}
