#pragma once

#include <OTwo/States/State.hpp>
#include <OTwo/Models/Character.hpp>
#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/UI/ToggleButton.hpp>

#include <SFML/System/String.hpp>

#include <vector>

enum class RoomTeam : std::uint8_t;

struct RoomSlot;
struct ItemMetadata;

struct WaitingMapChangedEventData;
struct WaitingTitleChangedEventData;
struct WaitingMusicChangedEventData;
struct WaitingMemberReadyStateChangedEventData;
struct WaitingMemberTeamChangedEventData;
struct WaitingMemberLeftEventData;
struct WaitingMemberJoinedEventData;
struct WaitingSlotChangedEventData;
struct WaitingKickEventData;
struct StartGameEventData;

class Avatar;
class AvatarInfo;
class ItemFactory;
class SessionContext;
class RoomContext;
class GameContext;
class WaitingService;
class MessagingService;
class StateWaiting7K : public State
{
public:
    StateWaiting7K(Gx::AudioMixer& mixer, SessionContext& session, RoomContext& room, GameContext& game, WaitingService& service, MessagingService& messaging, ItemFactory& items);

    void Initialize() override;

    void OnSlotChanged(const WaitingSlotChangedEventData& ev);
    void OnMemberJoined(const WaitingMemberJoinedEventData& ev);
    void OnMemberLeft(const WaitingMemberLeftEventData& ev);
    void OnMemberTeamChanged(const WaitingMemberTeamChangedEventData& ev);
    void OnMemberReadyStateChanged(const WaitingMemberReadyStateChangedEventData& ev);
    void OnMemberEmoticon(const CharacterInfo& sender, const sf::String& chatData);
    void OnMusicChanged(const WaitingMusicChangedEventData& ev);
    void OnTitleChanged(const WaitingTitleChangedEventData& ev);
    void OnMapChanged(const WaitingMapChangedEventData& ev);
    void OnKicked(const WaitingKickEventData& ev);
    void OnStartGame(const StartGameEventData& ev);

private:
    void InitializeAvatars();

    void OnReadyStateChanged(Gx::ToggleButton& sender);
    void OnStartStateChanged(Gx::ToggleButton& sender);

    void OnSelectMusicButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
    void OnSelectMusicDialogAccepted();

    void OnChangeTitleButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
    void OnChangeTitleDialogAccepted();

    void OnTeamButtonStateChanged(const Gx::ToggleButton& sender, RoomTeam team);
    void OnInstrumentSelectorStateChanged(const ItemMetadata& metadata) const;
    void OnMapSelectorStateChanged(unsigned int mapID);

    void OnEmoticonButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
    void OnEmoticonNextPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
    void OnEmoticonPreviousPageButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

    void OnBackButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

    void OnKeyPressed(const sf::Event::KeyPressed& ev) override;
    void OnKeyReleased(const sf::Event::KeyReleased& ev) override;

    void ExtendSlot(unsigned int slotID);
    void ShowEmoticon(const Avatar* avatar, const std::string& emoticonID);

    void InvalidateRoomInfo();
    void InvalidateAvatarInfo();
    void InvalidateMembers();

    Gx::AudioMixer& m_mixer;
    SessionContext& m_session;
    RoomContext& m_room;
    GameContext& m_game;
    WaitingService& m_service;
    MessagingService& m_messaging;
    ItemFactory& m_items;

    Avatar* m_mainAvatar{};
    AvatarInfo* m_avatarInfo{};
    RoomSlot* m_slot{};

    std::vector<Avatar*> m_avatars;
};
