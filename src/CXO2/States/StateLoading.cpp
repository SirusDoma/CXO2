#include <CXO2/States/StateLoading.hpp>
#include <CXO2/States/StatePlanet.hpp>

#include <CXO2/States/StatePlaying7K.hpp>
#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/GameContext.hpp>
#include <CXO2/Contexts/RoomContext.hpp>
#include <CXO2/Services/PlayingService.hpp>

#include <CXO2/Messages/RoomInfo.hpp>
#include <CXO2/Messages/Events/MemberMusicLoadedEventData.hpp>
#include <CXO2/Messages/Events/WaitingMemberLeftEventData.hpp>

#include <CXO2/IO/PlayingResourceContext.hpp>

#include <CXO2/StringTable/Identifiers/Cache.hpp>
#include <CXO2/StringTable/Identifiers/Loading.hpp>

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

namespace Cx
{

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

        m_service.SetMemberMusicLoadedEventCallback([this] (const auto& ev) { OnMemberConfirmMusicLoaded(ev); });

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

    void StateLoading::OnMemberConfirmMusicLoaded(const MessageEnvelope<MemberMusicLoadedEventData>& envelope)
    {
        try
        {
            const auto& ev = envelope.Open();

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
        catch (const Gx::Exception& ex)
        {
            ShowDialog(std::string(ex.what()), DialogStyle::Information, false, [this] (const bool)
            {
                GetDirector().Present<StatePlanet>();
            });
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
        m_service.ConfirmMusicLoaded([this] (const auto& ev)
        {
            try
            {
                const auto& _ = ev.Open();
            }
            catch (const Gx::Exception&)
            {
                GetDirector().Dismiss<StatePlanet>();
                return;
            }

            std::uint8_t userCount = 0;
            for (std::size_t i = 0; i < RoomContext::MaxCapacity; i++)
            {
                const auto& slot = m_room.GetSlot(i);
                if (slot.State == RoomSlotState::Occupied)
                    userCount++;
            }

            std::thread([this, userCount]
            {
                {
                    auto lock = std::unique_lock(m_mutex);
                    m_signal.wait_for(lock, sf::seconds(15).toDuration(),
                    [this, userCount]
                    {
                        return m_loadedUsers.size() >= userCount;
                    });
                }

                Invoke([this]
                {
                    m_service.SetMemberMusicLoadedEventCallback(nullptr);

                    auto ctx = PlayingResourceContext();
                    ctx.SetFxEnabled(m_context.GetConfig().UseFx);
                    ctx.SetMapID(m_context.GetMapID());
                    ctx.SetEffectID(m_context.GetEffectID());
                    ctx.SetMode(m_context.GetMode());

                    GetDirector().Present<StatePlaying7K>(ctx);
                });
            }).detach();
        });
    }
}
