#include <CXO2/States/StateLoading.hpp>

#include <CXO2/Events/LoadingEvents.hpp>

#include <CXO2/States/StatePlanet.hpp>

#include <CXO2/States/StatePlaying7K.hpp>
#include <CXO2/Config/GameConfig.hpp>
#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/RoomContext.hpp>
#include <CXO2/Services/PlayingService.hpp>

#include <CXO2/Network/Events/MemberMusicLoadedEventData.hpp>
#include <CXO2/Network/Events/WaitingMemberLeftEventData.hpp>

#include <CXO2/IO/PlayingResourceContext.hpp>

#include <CXO2/Constants/Identifiers/Cache.hpp>
#include <CXO2/Constants/Identifiers/Loading.hpp>
#include <CXO2/Constants/Messages/Loading.hpp>
#include <CXO2/Constants/Messages/Result.hpp>
#include <CXO2/Utilities/StringFormatter.hpp>

#include <Genode/UI/Image.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Utilities/Randomizer.hpp>

#include <fmt/format.h>
#include <thread>

namespace Gx
{
    class List;
}

namespace Cx
{

    using namespace Constants::Identifiers;

    StateLoading::StateLoading(GameConfig& config, SessionContext& session, RoomContext& room, PlayingService& service) :
        m_texture(),
        m_config(config),
        m_context(session),
        m_session(session),
        m_room(room),
        m_service(service),
        m_signal(),
        m_mutex(),
        m_loadedUsers()
    {
    }

    void StateLoading::Initialize(GameContext game)
    {
        m_context = std::move(game);
        Initialize();
    }

    void StateLoading::Initialize()
    {
        if (!State::Initialize(StateGameEventArgs{GetName(), m_context}))
            return;

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
        auto loader           = O2JamChartLoader(m_context.GetMode(), m_context.GetDifficulty());

        const auto list = Instantiate<Gx::List>(Resource::Loading::IDC_LIST_LOADING_SIGN);
        for (std::size_t i = 0; i < list->GetChildrenCount(); i++)
        {
            if (i >= RoomContext::MaxCapacity)
                break;

            const auto container = dynamic_cast<Gx::UiContainer*>(list->GetChildren()[i]);
            if (!container)
                continue;

            const auto& slot = m_room.GetSlot(i);
            if (slot.State != Room::SlotState::Occupied)
            {
                container->SetVisible(false);
                continue;
            }

            if (const auto level = container->FindChild<Gx::Label>(Resource::Loading::IDC_TEXT_USER_LEVEL))
                level->SetString(fmt::format("Lv. {}", slot.Level));

            if (const auto name = container->FindChild<Gx::Label>(Resource::Loading::IDC_TEXT_USER_NAME))
            {
                name->SetString(slot.Name);
                name->SetColor(slot.TeamColor);
            }

            if (const auto sign = container->FindChild<Gx::Image>(Resource::Loading::IDC_IMAGE_LOADING_SIGN))
                sign->SetFrame("Loading");
        }

        const auto& metadata = chart->GetMetadata();
        const auto noteIt    = metadata.NoteCounts.find(m_context.GetDifficulty());
        const auto noteCount = noteIt != metadata.NoteCounts.end() ? noteIt->second : 0u;

        Instantiate<Gx::Label>(Resource::Loading::IDC_TEXT_MUSIC_TITLE)->SetString(metadata.Title);
        Instantiate<Gx::Label>(Resource::Loading::IDC_TEXT_MUSIC_ARTIST)->SetString(metadata.Artist);
        Instantiate<Gx::Label>(Resource::Loading::IDC_TEXT_MUSIC_NOTE_COUNT)->SetString(std::to_string(noteCount));
        Instantiate<Gx::Label>(Resource::Loading::IDC_TEXT_MUSIC_NOTE_DESIGNER)->SetString(metadata.NoteDesigner);
        Instantiate<Gx::Label>(Resource::Loading::IDC_TEXT_MUSIC_SPEED)->SetString(fmt::format(Constants::Messages::Loading::SPEED, m_context.GetSpeed()));

        const auto missionHeader      = Instantiate<Gx::Label>(Resource::Loading::IDC_TEXT_MISSION_HEADER);
        const auto missionRequirement = Instantiate<Gx::Label>(Resource::Loading::IDC_TEXT_MISSION_REQUIREMENT);
        const auto missionNotice      = Instantiate<Gx::Label>(Resource::Loading::IDC_TEXT_MISSION_NOTICE);

        missionHeader->SetVisible(false);
        missionRequirement->SetVisible(false);
        missionNotice->SetVisible(false);

        const auto missionID = m_context.GetMissionID();
        if (m_context.GetMode() != GameMode::Tutorial && missionID >= 0)
        {
            const auto level = m_session.GetLevel();
            m_context.SetActiveMissionID(missionID);

            missionHeader->SetVisible(true);
            missionHeader->SetString(fmt::format(Constants::Messages::Loading::MISSION_HEADER, sf::String(std::to_string(level + 1))));

            missionRequirement->SetVisible(true);
            missionRequirement->SetString(Constants::Messages::Result::Mission::REQUIREMENTS[m_context.GetMissionID()]);

            if (m_context.GetMode() == GameMode::Single)
            {
                missionNotice->SetVisible(true);
                missionNotice->SetString(Constants::Messages::Loading::MISSION_NEEDS_MULTIPLAY);
            }
            else if (!m_context.IsMissionEligible())
            {
                missionNotice->SetVisible(true);
                missionNotice->SetString(Constants::Messages::Loading::MISSION_NEEDS_OTHER_TUNE);
            }
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
            if (Dispatch(LoadingEvents::OnMemberMusicLoaded, LoadingMemberMusicLoadedEventArgs{ev}))
                return;

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
            ShowDialog(std::string(ex.what()), DialogStyle::Information, [this] (const bool)
            {
                GetDirector().Present<StatePlanet>();
            });
        }
    }

    void StateLoading::OnMemberLeft(const WaitingMemberLeftEventData& ev)
    {
        if (Dispatch(LoadingEvents::OnMemberLeft, LoadingMemberLeftEventArgs{ev}))
            return;

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
        if (Dispatch(LoadingEvents::OnMusicLoaded, LoadingMusicEventArgs{*chart}))
            return;

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
                if (slot.State == Room::SlotState::Occupied)
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
                    ctx.SetFxEnabled(m_config.UseFx);
                    ctx.SetMapID(m_context.GetMapID());
                    ctx.SetEffectID(m_context.GetEffectID());
                    ctx.SetMode(m_context.GetMode());

                    GetDirector().Present<StatePlaying7K>(ctx, std::move(m_context));
                });
            }).detach();
        });
    }
}
