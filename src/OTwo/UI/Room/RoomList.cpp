#include <OTwo/UI/Room/RoomList.hpp>
#include <OTwo/States/State.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/UI/List.hpp>

#include <cmath>

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
    Gx::List::Initialize();

    auto sfxInvalid = &m_resources.AddFromFile<sf::Sound>("bgEffect/15");
    for (const auto child : GetChildren())
    {
        auto roomButton = dynamic_cast<RoomButton*>(child);
        roomButton->SetClickCallback([this, sfx = sfxInvalid, button = roomButton] (auto& sender, auto& ev)
        {
           if (!button->IsActive())
           {
               m_mixer.Play(*sfx, "SFX");
               return;
           }
        });
    }
}

void RoomList::Add(const Room& room)
{
    m_rooms[room.ID] = room;
    Invalidate();
}

void RoomList::Clear()
{
    m_rooms.clear();
    Invalidate();
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

void RoomList::Invalidate()
{
    const auto children = GetChildren();
    m_page = std::min(m_page, static_cast<unsigned int>(std::round(static_cast<float>(MAX_NUMBER_OF_ROOM) / children.size())));
    m_page = std::max(m_page, static_cast<unsigned int>(1));
    for (size_t i = 0; i < children.size(); i++)
    {
        const auto roomButton = dynamic_cast<RoomButton*>(children[i]);
        auto roomNumber = ((m_page - 1) * children.size()) + i;
        auto room = m_rooms.find(roomNumber);

        if (room != m_rooms.end() && (!m_waiting || room->second.State == RoomState::Waiting))
            roomButton->SetRoomInfo(room->second);
        else
            roomButton->Reset();
    }
}
