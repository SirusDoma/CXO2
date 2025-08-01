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

    void Upsert(const RoomInfo& room);
    RoomInfo& GetRoom(std::uint32_t id);
    void Remove(std::uint32_t id);
    void Clear();

    const RoomInfo* GetWaitingRoom();

    void ShowAll();
    void ShowWaitingOnly();

    void NextPage();
    void PreviousPage();

    void SetEnterRoomCallback(std::function<void(const RoomInfo&)> callback) const;

    void Invalidate() override;

private:
    constexpr static unsigned int MAX_NUMBER_OF_ROOM = 100;

    Gx::AudioMixer& m_mixer;
    Gx::ResourceManager& m_resources;

    std::map<unsigned int, RoomInfo> m_rooms;
    unsigned int m_page;
    bool m_waiting;
};
