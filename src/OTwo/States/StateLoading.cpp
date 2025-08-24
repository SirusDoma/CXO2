#include <OTwo/States/StateLoading.hpp>
#include <OTwo/States/StatePlanet.hpp>

#include <OTwo/States/StatePlaying7K.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/GameContext.hpp>
#include <OTwo/Contexts/RoomContext.hpp>
#include <OTwo/Services/PlayingService.hpp>

#include <OTwo/Messages/RoomInfo.hpp>
#include <OTwo/Messages/Events/MemberMusicLoadedEventData.hpp>
#include <OTwo/Messages/Events/WaitingMemberLeftEventData.hpp>

#include <OTwo/IO/PlayingResourceContext.hpp>

#include <OTwo/StringTable/Identifiers/Cache.hpp>
#include <OTwo/StringTable/Identifiers/Loading.hpp>

#include <Genode/UI/Image.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Utilities/Randomizer.hpp>

#include <thread>

namespace Gx
{
    class List;
}

using namespace StringTable::Identifiers;

StateLoading::StateLoading(GameContext& game, SessionContext& session, RoomContext& room, PlayingService& service) :
    m_texture(),
    m_context(game),
    m_session(session),
    m_room(room),
    m_service(service),
    m_signal(),
    m_mutex(),
    m_loadedUsers()
{
}

void StateLoading::Initialize()
{
    State::Initialize();

    std::size_t index = 0;
    auto imageSet = std::vector<Gx::Image*>();
    for (const auto child : GetChildren())
    {
        if (const auto image = dynamic_cast<Gx::Image*>(child); image && Gx::StringHelper::StartsWith(image->GetName(), Resource::Loading::IDC_IMAGE_STATE_LOADING))
            imageSet.push_back(image);
    }

    const int result = Gx::Randomizer::Randomize(0, static_cast<int>(imageSet.size() - 1));
    bool randomized = false;
    for (const auto image : imageSet)
    {
        randomized = randomized || index == result;
        image->SetVisible(index == result);
        index++;
    }

    if (!randomized && !imageSet.empty())
        imageSet[0]->SetVisible(true);

    const auto chart      = m_context.GetChart();
    const auto& resources = GetResources(ResourceScope::Shared);
    auto loader           = O2JamChartLoader(m_context);

    const auto list = Instantiate<Gx::List>(Resource::Loading::IDC_LIST_LOADING_SIGN);
    for (std::size_t i = 0; i < list->GetChildrenCount(); i++)
    {
        if (i >= RoomContext::MaxCapacity)
            break;

        const auto container = dynamic_cast<Gx::UiContainer*>(list->GetChildren()[i]);
        if (!container)
            continue;

        const auto& slot = m_room.GetSlot(i);
        if (slot.State != RoomSlotState::Occupied)
        {
            container->SetVisible(false);
            continue;
        }

        if (const auto level = container->FindChild<Gx::Label>(Resource::Loading::IDC_TEXT_USER_LEVEL))
            level->SetString(fmt::format("Lv. {}", slot.Member->Level));

        if (const auto name = container->FindChild<Gx::Label>(Resource::Loading::IDC_TEXT_USER_NAME))
        {
            name->SetString(slot.Member->Name);
            name->SetColor(slot.TeamColor);
        }

        if (const auto sign = container->FindChild<Gx::Image>(Resource::Loading::IDC_IMAGE_LOADING_SIGN))
            sign->SetFrame("Loading");
    }

    if (const auto image = resources.Find<sf::Image>(Resource::Cache::IDC_IMAGE_STATE_LOADING_COVER))
    {
        OnCoverLoaded(image);
    }
    else
    {
        loader.SetCoverLoadCallback([this] (const sf::Image* cover)
        {
            Invoke([=]
            {
                if (cover && cover->getSize() != sf::Vector2u{})
                    OnCoverLoaded(cover);
            });
        });
    }

    auto thread = std::thread([=] ()
    {
        m_context.SetChart(loader.LoadFromFile(chart->Source, Gx::ResourceContext::Default));
        Invoke([this] { OnChartLoaded(m_context.GetChart()); });
    });

    thread.detach();
}

bool StateLoading::IsTrackable()
{
    return false;
}

void StateLoading::OnMemberConfirmMusicLoaded(const MemberMusicLoadedEventData& ev)
{
    auto lock = std::lock_guard(m_mutex);

    m_loadedUsers.insert(ev.ID);
    m_signal.notify_one();

    const auto list = Instantiate<Gx::List>(Resource::Loading::IDC_LIST_LOADING_SIGN);
    if (ev.ID < list->GetChildrenCount())
    {
        if (const auto container = dynamic_cast<Gx::UiContainer*>(list->GetChildren()[ev.ID]))
        {
            if (const auto sign = container->FindChild<Gx::Image>(Resource::Loading::IDC_IMAGE_LOADING_SIGN))
                sign->SetFrame("Completed");
        }
    }
}

void StateLoading::OnMemberLeft(const WaitingMemberLeftEventData& ev)
{
    auto lock = std::lock_guard(m_mutex);

    m_loadedUsers.erase(ev.ID);
    m_signal.notify_one();

    const auto list = Instantiate<Gx::List>(Resource::Loading::IDC_LIST_LOADING_SIGN);
    if (ev.ID < list->GetChildrenCount())
    {
        if (const auto container = dynamic_cast<Gx::UiContainer*>(list->GetChildren()[ev.ID]))
        {
            if (const auto sign = container->FindChild<Gx::Image>(Resource::Loading::IDC_IMAGE_LOADING_SIGN))
                sign->SetFrame("Failed");
        }
    }
}

void StateLoading::OnCoverLoaded(const sf::Image* cover)
{
    for (const auto child : GetChildren())
    {
        if (const auto image = dynamic_cast<Gx::Image*>(child); image && image->IsVisible() && m_texture.loadFromImage(*cover))
        {
            image->SetTexture(m_texture);
            return;
        }
    }
}

void StateLoading::OnChartLoaded(const Chart* chart)
{
    m_service.ConfirmMusicLoaded([this, chart]
    {
        std::uint8_t userCount = 0;
        for (std::size_t i = 0; i < RoomContext::MaxCapacity; i++)
        {
            const auto& slot = m_room.GetSlot(i);
            if (slot.State == RoomSlotState::Occupied)
                userCount++;
        }

        auto lock = std::unique_lock(m_mutex);
        m_signal.wait_for(lock, sf::seconds(15).toDuration(),
        [this, userCount]
        {
            return m_loadedUsers.size() >= userCount;
        });

        auto ctx = PlayingResourceContext();
        ctx.SetFxEnabled(m_context.GetConfig().UseFx);
        ctx.SetMapID(m_context.GetMapID());
        ctx.SetEffectID(m_context.GetEffectID());
        ctx.SetMode(m_context.GetMode());

        GetDirector().Present<StatePlaying7K>(ctx);
    },
    [=] (const auto& ex)
    {
        GetDirector().Dismiss<StatePlanet>();
    });
}
