#include <CXO2/Services/Offline/ChannelOfflineService.hpp>

#include <CXO2/Contexts/SessionContext.hpp>

#include <CXO2/Network/Requests/CreateRoomRequest.hpp>
#include <CXO2/Network/Requests/JoinRoomRequest.hpp>

#include <algorithm>

namespace Cx
{
    ChannelOfflineService::ChannelOfflineService(SessionContext& session) :
        m_session(session)
    {
    }

    void ChannelOfflineService::GetChannelInfo(
        const MessageCallback<RoomListResponse>& roomCallback,
        const MessageCallback<UserListResponse>& userListCallback
    )
    {
        if (roomCallback)
            roomCallback(MessageEnvelope<RoomListResponse>(RoomListResponse{ m_rooms }));

        GetUserList(userListCallback);
    }

    void ChannelOfflineService::GetUserList(const MessageCallback<UserListResponse>& callback)
    {
        if (!callback)
            return;

        auto list = std::vector<UserListResponse::User>();
        list.push_back({ m_session.GetName(), m_session.GetLevel() });

        callback(MessageEnvelope<UserListResponse>(UserListResponse{ list }));
    }

    void ChannelOfflineService::CreateRoom(
        const CreateRoomRequest& request,
        const MessageCallback<CreateRoomResponse>& callback
    )
    {
        std::uint32_t id = 0;
        for (const auto& room : m_rooms)
            id = std::max(id, room.ID + 1);

        auto room = RoomListResponse::Room{};
        room.ID            = id;
        room.State         = RoomState::Waiting;
        room.Title         = request.Title;
        room.Locked        = request.Locked;
        room.MusicID       = 0;
        room.Difficulty    = Difficulty::EX;
        room.Mode          = request.GameMode;
        room.Speed         = Speed::X15;
        room.Capacity      = 8;
        room.UserCount     = 1;
        room.MinLevelLimit = request.MinLevelLimit;
        room.MaxLevelLimit = request.MaxLevelLimit;
        m_rooms.push_back(room);

        if (m_createCallback)
        {
            m_createCallback(MessageEnvelope<RoomCreatedEventData>(RoomCreatedEventData{
                id,
                request.Title,
                request.GameMode,
                request.Locked,
                request.MinLevelLimit,
                request.MaxLevelLimit
            }));
        }

        if (callback)
            callback(MessageEnvelope<CreateRoomResponse>(CreateRoomResponse{ CreateRoomResult::Success, id }));
    }

    void ChannelOfflineService::JoinRoom(
        const JoinRoomRequest& request,
        const MessageCallback<JoinRoomResponse>& callback
    )
    {
        if (!callback)
            return;

        auto room = RoomListResponse::Room{};
        room.State      = RoomState::Waiting;
        room.Title      = sf::String("Offline Room");
        room.Difficulty = Difficulty::EX;
        room.Speed      = Speed::X15;
        room.Capacity   = 8;

        const auto it = std::find_if(m_rooms.begin(), m_rooms.end(), [&request] (const RoomListResponse::Room& entry)
        {
            return entry.ID == request.ID;
        });
        if (it != m_rooms.end())
            room = *it;

        auto member = JoinRoomResponse::Member{};
        member.Name            = m_session.GetName();
        member.Level           = m_session.GetLevel();
        member.Gender          = m_session.GetGender();
        member.IsRoomMaster    = true;
        member.Team            = Room::Team::A;
        member.Ready           = false;
        member.EquippedItemIDs = m_session.GetEquippedItemIDs();
        member.MusicIDs        = m_session.GetMusicIDs();

        auto slots = std::vector<JoinRoomResponse::Slot>();
        for (std::uint8_t i = 0; i < 8; i++)
        {
            auto slot = JoinRoomResponse::Slot{};
            slot.Index = i;
            slot.State = i == 0 ? Room::SlotState::Occupied : Room::SlotState::Unoccupied;
            if (i == 0)
                slot.Member = member;

            slots.push_back(slot);
        }

        auto response = JoinRoomResponse{};
        response.Result     = JoinResult::Success;
        response.SlotID     = 0;
        response.Team       = Room::Team::A;
        response.Title      = room.Title;
        response.MusicID    = room.MusicID;
        response.Mode       = room.Mode;
        response.Difficulty = room.Difficulty;
        response.Speed      = room.Speed;
        response.UserCount  = 1;
        response.Slots      = slots;

        callback(MessageEnvelope<JoinRoomResponse>(response));
    }

    void ChannelOfflineService::Logout(const MessageCallback<ChannelLogoutResponse>& callback)
    {
        if (callback)
            callback(MessageEnvelope<ChannelLogoutResponse>(ChannelLogoutResponse{}));
    }

    void ChannelOfflineService::SetRoomCreatedEventCallback(const MessageCallback<RoomCreatedEventData>& callback)
    {
        m_createCallback = callback;
    }

    void ChannelOfflineService::SetRoomMusicChangedEventCallback(const MessageCallback<RoomMusicChangedEventData>& callback)
    {
        m_musicCallback = callback;
    }

    void ChannelOfflineService::SetRoomStateChangedEventCallback(const MessageCallback<RoomStateChangedEventData>& callback)
    {
        m_stateCallback = callback;
    }

    void ChannelOfflineService::SetRoomTitleChangedEventCallback(const MessageCallback<RoomTitleChangedEventData>& callback)
    {
        m_titleCallback = callback;
    }

    void ChannelOfflineService::SetRoomUserCountChangedEventCallback(const MessageCallback<RoomUserCountChangedEventData>& callback)
    {
        m_countCallback = callback;
    }

    void ChannelOfflineService::SetRoomRemovedEventCallback(const MessageCallback<RoomRemovedEventData>& callback)
    {
        m_removeCallback = callback;
    }
}
