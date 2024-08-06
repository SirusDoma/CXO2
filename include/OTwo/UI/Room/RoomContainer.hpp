#ifndef O2JAM_ROOM_CONTAINER_HPP
#define O2JAM_ROOM_CONTAINER_HPP

#include <Genode/UI/UiContainer.hpp>
#include <OTwo/UI/Room/RoomButton.hpp>

#include <vector>
#include <memory>
#include <map>

class RoomContainer : public Gx::UiContainer
{
public:
    RoomContainer();

    void Initialize() override;

    void Add(const Room &room);
    void Clear();

    void ShowAll();
    void ShowWaitingOnly();

    void NextPage();
    void PreviousPage();

private:
    constexpr static const unsigned int MAX_NUMBER_OF_ROOM = 100;

    void Invalidate() override;

    std::map<unsigned int, Room> m_rooms;

    unsigned int m_page;
    bool m_waiting;
};

#endif
