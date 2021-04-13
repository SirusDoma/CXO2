#ifndef O2JAM_ROOM_ROOM_LIST_HPP
#define O2JAM_ROOM_ROOM_LIST_HPP

#include <Genode/UI/UiContainer.hpp>
#include <O2/States/Components/Room/RoomButton.hpp>

#include <vector>
#include <memory>
#include <map>

class RoomList : public Gx::UiContainer
{
public:
    RoomList();
    void Initialize(Gx::Scene &scene);

    void PushRoomData(Room::RoomData room);
    void Clear();

private:
    constexpr static const unsigned int ROOM_PER_PAGE = 6;

    virtual void Invalidate();

    std::map<unsigned int, Room::RoomData> m_rooms;
    std::vector<std::unique_ptr<RoomButton>> m_roomButtons;

    unsigned int m_page;
    bool m_filterWaiting;
};

#endif
