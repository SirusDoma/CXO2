#ifndef O2JAM_ROOM_LIST_HPP
#define O2JAM_ROOM_LIST_HPP

#include <Genode/UI/List.hpp>
#include <OTwo/UI/Room/RoomButton.hpp>

#include <vector>
#include <memory>
#include <map>

namespace Gx
{
    class Mixer;
    class ResourceManager;
}

class RoomList : public Gx::List
{
public:
    RoomList(Gx::Mixer& mixer, Gx::ResourceManager& resources);

    void Initialize() override;

    void Add(const Room& room);
    void Clear();

    void ShowAll();
    void ShowWaitingOnly();

    void NextPage();
    void PreviousPage();

private:
    constexpr static unsigned int MAX_NUMBER_OF_ROOM = 100;

    void Invalidate() override;

    Gx::Mixer& m_mixer;
    Gx::ResourceManager& m_resources;

    std::map<unsigned int, Room> m_rooms;
    unsigned int m_page;
    bool m_waiting;
};

#endif
