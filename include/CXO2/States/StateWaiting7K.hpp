#pragma once

#include <CXO2/States/State.hpp>
#include <CXO2/Models/Character.hpp>
#include <CXO2/Models/Room.hpp>
#include <CXO2/Services/WaitingService.hpp>
#include <Genode/Audio/AudioMixer.hpp>
#include <CXO2/UI/ToggleButton.hpp>

#include <SFML/System/String.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace Gx
{
    class Sprite;
}

namespace Cx
{
    class RadioButton;

    struct ItemMetadata;

    class Avatar;
    class AvatarInfo;
    class ItemFactory;
    class SessionContext;
    class RoomContext;
    class ChatService;
    class StateWaiting7K : public State
    {
    public:
        StateWaiting7K(Gx::AudioMixer& mixer, SessionContext& session, RoomContext& room, WaitingService& service, ChatService& messaging, ItemFactory& items);

        void Initialize() override;
        void ShowChatHelp();

        void OnMemberEmoticon(const sf::String& sender, const sf::String& chatData);
        void ChangeRoomTitle(const sf::String& title);

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
        void OnUpdateReadyStateResponded(Cx::ToggleButton& sender, const MessageEnvelope<UpdateMemberReadyStateRequest>& ev);
        void OnUpdateMusicResponded(const MessageEnvelope<UpdateRoomMusicRequest>& ev);
        void OnUpdateRoomTitleResponded(const MessageEnvelope<UpdateRoomTitleRequest>& ev);
        void OnUpdateTeamResponded(const MessageEnvelope<UpdateMemberTeamRequest>& ev);
        void OnExitRoomResponded(const MessageEnvelope<ExitWaitingResponse>& ev);

        void OnReadyStateChanged(Cx::ToggleButton& sender, Control::Event& ev);
        void OnStartStateChanged(Cx::ToggleButton& sender, Control::Event& ev);

        void OnSelectMusicButtonClicked(Control& sender, Control::Event& ev);
        void OnSelectMusicDialogAccepted();

        void OnChangeTitleButtonClicked(Control& sender, Control::Event& ev);
        void OnChangeTitleDialogAccepted();

        void OnTeamButtonStateChanged(Cx::RadioButton& sender, Control::Event& ev);
        void OnInstrumentSelectorStateChanged(const ItemMetadata& metadata) const;
        void OnMapSelectorStateChanged(unsigned int mapID);
        void OnEffectSelectorStateChanged(unsigned int effectID);

        void OnEmoticonButtonClicked(Control& sender, Control::Event& ev);
        void OnEmoticonNextPageButtonClicked(Control& sender, Control::Event& ev);
        void OnEmoticonPreviousPageButtonClicked(Control& sender, Control::Event& ev);

        void OnExtendButtonDoubleClicked(Control& sender, Control::Event& ev);

        void OnBackButtonClicked(Control& sender, Control::Event& ev);

        void OnKeyPressed(const sf::Event::KeyPressed& ev) override;
        void OnKeyReleased(const sf::Event::KeyReleased& ev) override;

        void ExtendSlot(unsigned int slotID);
        void ShowEmoticon(const Avatar* avatar, const std::string& emoticonID);
        void SendEmoticon(std::string command, const std::string& emoticonID);

        void InvalidateRoomInfo();
        void InvalidateAvatarInfo();
        void InvalidateMembers();
        void InvalidateSlotMarkers(Gx::Sprite* bossMark, Gx::Sprite* noMusic, const Room::Slot& slot);

        Gx::AudioMixer& m_mixer;
        SessionContext& m_session;
        RoomContext& m_room;
        WaitingService& m_service;
        ChatService& m_messaging;
        ItemFactory& m_items;

        Avatar* m_mainAvatar{};
        AvatarInfo* m_avatarInfo{};
        const Room::Slot* m_slot{};

        std::vector<Avatar*> m_avatars;

        std::unordered_map<Cx::RadioButton*, Room::Team> m_teamButtons;
        std::unordered_map<Control*, int> m_extendButtonSlotIDs;
    };
}
