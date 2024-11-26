#pragma once

#include <OTwo/UI/Room/RoomButton.hpp>

#include <Genode/UI/List.hpp>

#include <vector>
#include <memory>
#include <map>

namespace Gx
{
    class AudioMixer;
    class ResourceManager;
}

class RoomList : public Gx::List
{
public:
    RoomList(Gx::AudioMixer& mixer, Gx::ResourceManager& resources);

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

    Gx::AudioMixer& m_mixer;
    Gx::ResourceManager& m_resources;

    std::map<unsigned int, Room> m_rooms;
    unsigned int m_page;
    bool m_waiting;
};
