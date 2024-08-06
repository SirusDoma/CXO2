#include <OTwo/UI/Room/RoomContainer.hpp>
#include <OTwo/States/State.hpp>

#include <Genode/UI/List.hpp>

#include <cmath>

RoomContainer::RoomContainer() :
    m_rooms(),
    m_waiting(false),
    m_page(1)
{
}

void RoomContainer::Initialize()
{
    auto parent = GetParent<::State>();
    if (!parent)
        return;

    auto& app   = Gx::Application::Instance();
    auto& mixer = app.Require<Gx::Mixer>();

    auto sfxInvalid  = parent->Instantiate<sf::Sound>("STATE_ROOM/IDC_SOUND_15");
    auto roomList    = FindChild<Gx::List>("IDC_LIST_ROOM");
    for (auto child : roomList->GetChildren())
    {
        auto roomButton = dynamic_cast<RoomButton*>(child);
        roomButton->Initialize();
        roomButton->SetClickCallback([&, sfx = sfxInvalid, button = roomButton] (auto& sender, auto& ev)
        {
           if (!button->IsActive())
           {
               mixer.Play(sfx, "SFX");
               return;
           }
        });

    }
    AddChild(roomList);
}

void RoomContainer::Add(const Room &room)
{
    m_rooms[room.ID] = room;
    Invalidate();
}

void RoomContainer::Clear()
{
    m_rooms.clear();
    Invalidate();
}

void RoomContainer::ShowAll()
{
    m_waiting = false;
    Invalidate();
}

void RoomContainer::ShowWaitingOnly()
{
    m_waiting = true;
    Invalidate();
}

void RoomContainer::NextPage()
{
    m_page++;
    Invalidate();
}

void RoomContainer::PreviousPage()
{
    m_page--;
    Invalidate();
}

void RoomContainer::Invalidate()
{
    auto roomList = FindChild<Gx::List>("IDC_LIST_ROOM");
    if (!roomList)
        return;

    auto children = roomList->GetChildren();
    m_page = std::min(m_page, static_cast<unsigned int>(std::round(static_cast<float>(MAX_NUMBER_OF_ROOM) / children.size())));
    m_page = std::max(m_page, static_cast<unsigned int>(1));
    for (size_t i = 0; i < children.size(); i++)
    {
        auto roomButton = dynamic_cast<RoomButton*>(children[i]);
        auto roomNumber = ((m_page - 1) * children.size()) + i;
        auto room = m_rooms.find(roomNumber);

        if (room != m_rooms.end() && (!m_waiting || room->second.State == RoomState::Waiting))
            roomButton->SetRoomInfo(room->second);
        else
            roomButton->Reset();
    }
}
