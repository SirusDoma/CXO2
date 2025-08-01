#pragma once

#include <OTwo/Services/EventService.hpp>

#include <SFML/System/String.hpp>

#include <functional>

enum class StartGameResult : std::uint32_t;
enum class Difficulty : std::uint8_t;
enum class RoomTeam : std::uint8_t;
enum class RoomSlotEventType : std::uint8_t;

struct RoomMember;

struct UpdateRoomMusicRequest;
struct UpdateMapRequest;

struct WaitingSlotChangedEventData;
struct WaitingMemberJoinedEventData;
struct WaitingMemberLeftEventData;
struct WaitingMemberTeamChangedEventData;
struct WaitingMemberReadyStateChangedEventData;
struct WaitingMusicChangedEventData;
struct WaitingTitleChangedEventData;
struct WaitingMapChangedEventData;
struct WaitingKickEventData;

class NetworkAdapter;
class NetworkException;
class SessionContext;
class WaitingService : public virtual Service
{
public:
    virtual void StartGame(
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const = 0;

    virtual void UpdateReadyState(
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const = 0;

    virtual void UpdateMusic(
        const UpdateRoomMusicRequest& request,
        std::function<void()> callback, const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const = 0;

    virtual void UpdateRoomSlot(
        std::uint8_t id,
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const = 0;

    virtual void UpdateTeam(
        RoomTeam team,
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const = 0;

    virtual void UpdateRoomTitle(
        const std::string& title,
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const = 0;

    virtual void UpdateMap(
        const UpdateMapRequest& request,
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const = 0;

    virtual void ExitRoom(
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const = 0;
};

class WaitingOnlineService : public WaitingService, public EventService
{
public:
    WaitingOnlineService(NetworkAdapter& adapter, SessionContext& session);

    void StartGame(
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const override;

    void UpdateReadyState(
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const override;

    void UpdateMusic(
        const UpdateRoomMusicRequest& request,
        std::function<void()> callback, const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const override;

    void UpdateRoomSlot(
        std::uint8_t id,
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const override;

    void UpdateTeam(
        RoomTeam team,
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const override;

    void UpdateRoomTitle(
        const std::string& title,
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const override;

    void UpdateMap(
        const UpdateMapRequest& request,
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const override;

    void ExitRoom(
        std::function<void()> callback,
        const std::function<void(const NetworkException&)>& errorCallback = nullptr
    ) const override;

private:
    SessionContext& m_session;
};