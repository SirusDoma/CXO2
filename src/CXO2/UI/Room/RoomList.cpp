#include <CXO2/UI/Room/RoomList.hpp>

#include <CXO2/Models/Room.hpp>
#include <CXO2/StringTable/Identifiers/Sound.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/UI/List.hpp>

#include <cmath>

namespace Cx
{
    using namespace StringTable::Identifiers;

    RoomList::RoomList(Gx::AudioMixer& mixer, Gx::ResourceManager& resources) :
        m_mixer(mixer),
        m_resources(resources),
        m_rooms(),
        m_page(1),
        m_waiting(false)
    {
    }

    void RoomList::Initialize()
    {
        List::Initialize();

        m_sfxInvalid = &m_resources.AddFromFile<sf::Sound>(Sound::Effects::EF_15);
        for (const auto child : GetChildren())
        {
            auto roomButton = dynamic_cast<RoomButton*>(child);
            roomButton->SetClickCallback([this] (auto& sender, auto& ev)
            {
                OnRoomButtonClicked(sender, ev);
            });
        }
    }

    void RoomList::Upsert(const RoomInfo& room)
    {
        if (const auto it = m_rooms.find(room.ID); it != m_rooms.end())
        {
            it->second.State         = room.State;
            it->second.Title         = room.Title;
            it->second.Locked        = room.Locked;
            it->second.MusicID       = room.MusicID;
            it->second.Difficulty    = room.Difficulty;
            it->second.Mode          = room.Mode;
            it->second.SpeedID       = room.SpeedID;
            it->second.Capacity      = room.Capacity;
            it->second.UserCount     = room.UserCount;
            it->second.MinLevelLimit = room.MinLevelLimit;
            it->second.MaxLevelLimit = room.MaxLevelLimit;
        }
        else
            m_rooms[room.ID] = room;
    }

    RoomInfo& RoomList::GetRoom(const std::uint32_t id)
    {
        if (const auto it = m_rooms.find(id); it != m_rooms.end())
            return it->second;

        m_rooms[id] = RoomInfo{};
        return m_rooms[id];
    }

    void RoomList::Remove(const std::uint32_t id)
    {
        if (const auto it = m_rooms.find(id); it != m_rooms.end())
        {
            m_rooms.erase(it);
            Invalidate();
        }
    }

    void RoomList::Clear()
    {
        m_rooms.clear();
        Invalidate();
    }

    const RoomInfo* RoomList::GetWaitingRoom()
    {
        for (const auto& [id, room] : m_rooms)
        {
            if (room.State == RoomState::Waiting && !room.Locked && room.UserCount < room.Capacity)
                return &room;
        }

        return nullptr;
    }

    void RoomList::ShowAll()
    {
        m_waiting = false;
        Invalidate();
    }

    void RoomList::ShowWaitingOnly()
    {
        m_waiting = true;
        Invalidate();
    }

    void RoomList::NextPage()
    {
        m_page++;
        Invalidate();
    }

    void RoomList::PreviousPage()
    {
        m_page--;
        Invalidate();
    }

    void RoomList::SetEnterRoomCallback(std::function<void(const RoomInfo&)> callback) const
    {
        m_sfxInvalid        = &m_resources.AddFromFile<sf::Sound>(Sound::Effects::EF_15);
        m_enterRoomCallback = std::move(callback);
        for (const auto child : GetChildren())
        {
            auto roomButton = dynamic_cast<RoomButton*>(child);
            roomButton->SetClickCallback([this] (auto& sender, auto& ev)
            {
                OnEnterRoomButtonClicked(sender, ev);
            });
        }
    }

    void RoomList::OnRoomButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto button = dynamic_cast<RoomButton*>(&sender);
        if (!button->IsActive())
            m_mixer.Play(*m_sfxInvalid, Sound::Channel::SFX);
    }

    void RoomList::OnEnterRoomButtonClicked(Gx::Control& sender, Gx::Control::Event& ev) const
    {
        const auto button = dynamic_cast<RoomButton*>(&sender);
        if (!button->IsActive())
        {
            m_mixer.Play(*m_sfxInvalid, Sound::Channel::SFX);
            return;
        }

        if (m_enterRoomCallback)
            m_enterRoomCallback(button->GetRoomInfo());
    }

    void RoomList::Invalidate()
    {
        const auto children = GetChildren();
        m_page = std::min(m_page, static_cast<unsigned int>(std::round(static_cast<float>(MAX_NUMBER_OF_ROOM) / children.size())));
        m_page = std::max(m_page, static_cast<unsigned int>(1));
        for (size_t i = 0; i < children.size(); i++)
        {
            const auto roomButton = dynamic_cast<RoomButton*>(children[i]);
            if (!roomButton)
                continue;

            auto roomNumber = ((m_page - 1) * children.size()) + i;
            auto room = m_rooms.find(roomNumber);

            if (room != m_rooms.end() && room->second.MusicID != 0 && room->second.State != RoomState::Unavailable && (!m_waiting || room->second.State == RoomState::Waiting))
                roomButton->SetRoomInfo(room->second);
            else
                roomButton->Reset();
        }
    }
}
