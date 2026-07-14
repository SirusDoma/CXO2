#pragma once

#include <CXO2/States/State.hpp>
#include <CXO2/Models/Character.hpp>
#include <CXO2/Services/WaitingService.hpp>
#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/UI/ToggleButton.hpp>

#include <SFML/System/String.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace Gx
{
    class RadioButton;
    class Sprite;
}

namespace Cx
{
    struct RoomSlot;
    struct ItemMetadata;

    class Avatar;
    class AvatarInfo;
    class ItemFactory;
    class SessionContext;
    class RoomContext;
    class GameContext;
    class ChatService;
    class StateWaiting7K : public State
    {
    public:
        StateWaiting7K(Gx::AudioMixer& mixer, SessionContext& session, RoomContext& room, GameContext& game, WaitingService& service, ChatService& messaging, ItemFactory& items);

        void Initialize() override;

        void OnMemberEmoticon(const CharacterInfo& sender, const sf::String& chatData);

    private:
        void InitializeAvatars();
        void RegisterMessageEvents();

        void OnSlotChanged(const MessageEnvelope<WaitingSlotChangedEventData>& ev);
        void OnKicked(const MessageEnvelope<WaitingKickEventData>& ev);

        void OnMemberJoined(const MessageEnvelope<WaitingMemberJoinedEventData>& ev);
        void OnMemberLeft(const MessageEnvelope<WaitingMemberLeftEventData>& ev);
        void OnMemberTeamChanged(const MessageEnvelope<WaitingMemberTeamChangedEventData>& ev);
        void OnMemberReadyStateChanged(const MessageEnvelope<WaitingMemberReadyStateChangedEventData>& ev);

        void OnMusicChanged(const MessageEnvelope<WaitingMusicChangedEventData>& ev);
        void OnTitleChanged(const MessageEnvelope<WaitingTitleChangedEventData>& ev);
        void OnMapChanged(const MessageEnvelope<WaitingMapChangedEventData>& ev);
        void OnStartGame(const MessageEnvelope<StartGameEventData>& ev);

        void OnStartGameResponded(const MessageEnvelope<StartGameRequest>& ev);
        void OnUpdateReadyStateResponded(Gx::ToggleButton& sender, const MessageEnvelope<UpdateMemberReadyStateRequest>& ev);
        void OnUpdateMusicResponded(const MessageEnvelope<UpdateRoomMusicRequest>& ev);
        void OnUpdateRoomTitleResponded(const MessageEnvelope<UpdateRoomTitleRequest>& ev);
        void OnUpdateTeamResponded(const MessageEnvelope<UpdateMemberTeamRequest>& ev, RoomTeam team);
        void OnExitRoomResponded(const MessageEnvelope<ExitWaitingResponse>& ev);

        void OnReadyStateChanged(Gx::ToggleButton& sender);
        void OnStartStateChanged(Gx::ToggleButton& sender);

        void OnSelectMusicButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnSelectMusicDialogAccepted();

        void OnChangeTitleButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnChangeTitleDialogAccepted();

        void OnTeamButtonStateChanged(Gx::RadioButton& sender);
        void OnInstrumentSelectorStateChanged(const ItemMetadata& metadata) const;
        void OnMapSelectorStateChanged(unsigned int mapID);
        void OnEffectSelectorStateChanged(unsigned int effectID);

        void OnEmoticonButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnEmoticonNextPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnEmoticonPreviousPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnExtendButtonDoubleClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnBackButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnKeyPressed(const sf::Event::KeyPressed& ev) override;
        void OnKeyReleased(const sf::Event::KeyReleased& ev) override;

        void ExtendSlot(unsigned int slotID);
        void ShowEmoticon(const Avatar* avatar, const std::string& emoticonID);
        void SendEmoticon(const std::vector<std::uint8_t>& code, const std::string& emoticonID);

        void InvalidateRoomInfo();
        void InvalidateAvatarInfo();
        void InvalidateMembers();
        void InvalidateSlotMarkers(Gx::Sprite* bossMark, Gx::Sprite* noMusic, const RoomSlot& slot);

        Gx::AudioMixer& m_mixer;
        SessionContext& m_session;
        RoomContext& m_room;
        GameContext& m_game;
        WaitingService& m_service;
        ChatService& m_messaging;
        ItemFactory& m_items;

        Avatar* m_mainAvatar{};
        AvatarInfo* m_avatarInfo{};
        RoomSlot* m_slot{};

        std::vector<Avatar*> m_avatars;

        std::unordered_map<Gx::RadioButton*, RoomTeam> m_teamButtons;
        std::unordered_map<Gx::Control*, int> m_extendButtonSlotIDs;
    };
}
