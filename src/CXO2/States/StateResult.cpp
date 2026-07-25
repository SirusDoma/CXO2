#include <CXO2/States/StateResult.hpp>
#include <CXO2/States/StateWaiting7K.hpp>
#include <CXO2/States/StateLoading.hpp>
#include <CXO2/States/StatePlaying7K.hpp>
#include <CXO2/States/StatePlanet.hpp>

#include <CXO2/Core/ScoreTracker.hpp>
#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/RoomContext.hpp>

#include <CXO2/Services/WaitingService.hpp>
#include <CXO2/Services/PlayingService.hpp>


#include <CXO2/Constants/Identifiers/Cache.hpp>
#include <CXO2/Constants/Identifiers/Sound.hpp>
#include <CXO2/Constants/Identifiers/Result.hpp>

#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Tween/Move.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/Utilities/Randomizer.hpp>

namespace Cx
{
    using namespace Constants::Identifiers;

    StateResult::StateResult(Gx::AudioMixer& mixer, SessionContext& session, RoomContext& room, const ScoreTracker& scoreTracker, WaitingService& waiting, PlayingService& service) :
        m_mixer(mixer),
        m_session(session),
        m_room(room),
        m_waiting(waiting),
        m_service(service),
        m_scoreTracker(scoreTracker)
    {
    }

    void StateResult::Initialize(GameContext game)
    {
        m_context = std::move(game);
        Initialize();
    }

    void StateResult::Initialize()
    {
        State::Initialize();
        if (const auto container = Instantiate<Gx::UiContainer>(Resource::Result::IDC_CONTAINER_BACKGROUND); container)
        {
            const auto& resources = GetResources(ResourceScope::Shared);
            if (const auto texture = resources.Find<sf::Texture>(Resource::Cache::IDC_TEXTURE_STATE_PLAYING); texture)
            {
                auto& fragment = Create<Gx::Sprite>(*texture);
                container->AddChild(fragment);
            }
        }

        auto const view = GetDefaultView();
        const auto background = Instantiate<Gx::Sprite>(Resource::Result::IDC_IMAGE_STATE_RESULT);
        background->SetVisible(false);

        auto top = Instantiate<Gx::Image>(Resource::Result::IDC_IMAGE_STATE_RESULT_TOP);
        auto bottom = Instantiate<Gx::Image>(Resource::Result::IDC_IMAGE_STATE_RESULT_BOTTOM);

        const auto banner = Instantiate<Gx::Image>(Resource::Result::IDC_IMAGE_WINLOSE);
        banner->SetVisible(false);

        auto height = bottom->GetLocalBounds().size.y;
        if (height < top->GetLocalBounds().size.y)
            height = top->GetLocalBounds().size.y;

        top->SetPosition(0.f, -height + -top->GetLocalBounds().size.y);
        bottom->SetPosition(0.f, view.getSize().y + height);

        if (const auto container = top->FindChild<Gx::UiContainer>(Resource::Result::Top::IDC_CONTAINER_PLAYER_SCORE); container)
        {
            if (const auto cool = container->FindChild<Gx::Label>(Resource::Result::Top::Score::IDC_TEXT_PLAYER_COOL); cool)
                cool->SetString(std::to_string(m_scoreTracker.GetPoint(Accuracy::Cool)));

            if (const auto good = container->FindChild<Gx::Label>(Resource::Result::Top::Score::IDC_TEXT_PLAYER_GOOD); good)
                good->SetString(std::to_string(m_scoreTracker.GetPoint(Accuracy::Good)));

            if (const auto bad = container->FindChild<Gx::Label>(Resource::Result::Top::Score::IDC_TEXT_PLAYER_BAD); bad)
                bad->SetString(std::to_string(m_scoreTracker.GetPoint(Accuracy::Bad)));

            if (const auto miss = container->FindChild<Gx::Label>(Resource::Result::Top::Score::IDC_TEXT_PLAYER_MISS); miss)
                miss->SetString(std::to_string(m_scoreTracker.GetPoint(Accuracy::Miss)));

            if (const auto maxCombo = container->FindChild<Gx::Label>(Resource::Result::Top::Score::IDC_TEXT_PLAYER_MAX_COMBO); maxCombo)
                maxCombo->SetString(std::to_string(m_scoreTracker.GetMaxCombo()));

            if (const auto maxJamCombo = container->FindChild<Gx::Label>(Resource::Result::Top::Score::IDC_TEXT_PLAYER_MAX_JAM_COMBO); maxJamCombo)
                maxJamCombo->SetString(std::to_string(m_scoreTracker.GetMaxJamCombo()));
        }

        if (const auto point = top->FindChild<Gx::BitmapNumber>(Resource::Result::Top::IDC_NUMBER_POINT); point)
            point->SetValue(m_scoreTracker.GetScorePoint());

        if (const auto gem = top->FindChild<Gx::Label>(Resource::Result::Top::IDC_TEXT_GEM); gem)
            gem->SetString(std::to_string(100));

        if (const auto list = bottom->FindChild<Gx::List>(Resource::Result::Bottom::IDC_LIST_RANK_SCORE); list)
        {
            const auto listItems  = list->GetChildren();
            const auto& scoreItems = m_context.Scores;
            for (std::size_t i = 0; i < listItems.size(); i++)
            {
                const auto item = dynamic_cast<Gx::UiContainer*>(listItems[i]);
                if (!item)
                    continue;

                item->SetVisible(false);
                if (i >= scoreItems.size())
                    continue;

                const auto& entry = scoreItems[i];
                if (!entry.Active)
                    continue;

                const auto& slot = m_room.GetSlot(entry.ID);
                if (slot.State != Room::SlotState::Occupied)
                    continue;

                const auto& member = slot;
                if (member.Name.isEmpty())
                    continue;

                // TODO: Make this adjustable?
                item->SetVisible(true);

                auto primaryTeamColor = std::unordered_map<Room::Team, sf::Color>
                ({
                    { Room::Team::A, sf::Color(255, 000, 16, 50) },
                    { Room::Team::B, sf::Color(255, 190, 16, 50) },
                    { Room::Team::C, sf::Color(247, 255, 0, 50) },
                    { Room::Team::D, sf::Color(155, 206, 82, 50) },
                    { Room::Team::E, sf::Color(107, 235, 230, 50) },
                    { Room::Team::F, sf::Color(000, 000, 230, 50) },
                    { Room::Team::G, sf::Color(156, 81, 214, 50) },
                    { Room::Team::H, sf::Color(181, 77, 025, 50) },
                });

                auto secondaryTeamColor = std::unordered_map<Room::Team, sf::Color>
                ({
                    { Room::Team::A, sf::Color(123, 130, 16, 50) },
                    { Room::Team::B, sf::Color(123, 223, 16, 50) },
                    { Room::Team::C, sf::Color(123, 125, 0, 50) },
                    { Room::Team::D, sf::Color(74, 231, 82, 50) },
                    { Room::Team::E, sf::Color(49, 247, 230, 50) },
                    { Room::Team::F, sf::Color(000, 000, 230, 50) },
                    { Room::Team::G, sf::Color(74, 170, 214, 50) },
                    { Room::Team::H, sf::Color(90, 36, 25, 50) },
                });

                if (member.Name == m_session.GetName())
                    banner->SetFrame(i == 0 ? "win" : "lose");

                if (const auto highlighter = item->FindChild<Gx::Rectangle>(Resource::Result::Bottom::Score::IDC_RECTANGLE_HIGHLIGHT); highlighter)
                {
                    if (member.Name == m_session.GetName())
                        highlighter->SetColor(primaryTeamColor[slot.Team]);
                    else
                        highlighter->SetColor(secondaryTeamColor[slot.Team]);
                }

                if (const auto rank = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_NUMBER); rank)
                    rank->SetString(std::to_string(i + 1));

                if (const auto name = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_NAME); name)
                    name->SetString(member.Name);

                if (const auto cool = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_COOL); cool)
                    cool->SetString(std::to_string(entry.Cool));

                if (const auto good = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_GOOD); good)
                    good->SetString(std::to_string(entry.Good));

                if (const auto bad = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_BAD); bad)
                    bad->SetString(std::to_string(entry.Bad));

                if (const auto miss = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_MISS); miss)
                    miss->SetString(std::to_string(entry.Miss));

                if (const auto maxCombo = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_MAX_COMBO); maxCombo)
                    maxCombo->SetString(std::to_string(entry.MaxCombo));

                if (const auto maxJamCombo = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_MAX_JAM_COMBO); maxJamCombo)
                    maxJamCombo->SetString(std::to_string(entry.MaxJamCombo));

                if (const auto point = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_POINT); point)
                    point->SetString(std::to_string(entry.Score));
            }
        }

        const auto btnBack = bottom->FindChild<Gx::Button>(Resource::Result::Bottom::IDC_BUTTON_BACK);
        const auto btnRetry = bottom->FindChild<Gx::Button>(Resource::Result::Bottom::IDC_BUTTON_PLAY_RETRY);

        btnRetry->SetVisible(m_room.GetMode() == GameMode::Single);
        btnRetry->SetEnabled(false);
        btnRetry->SetClickCallback([this, btnBack] (auto& sender, const auto&)
        {
            sender.SetEnabled(false);
            btnBack->SetEnabled(false);

            const auto showError = [this] (const Gx::Exception& e)
            {
                ShowDialog(std::string(e.what()), DialogStyle::Information, false, [this] (bool)
                {
                    GetDirector().Dismiss<StatePlanet>();
                });
            };

            m_service.ConfirmResult();
            m_waiting.UpdateReadyState([=] (const auto& ev)
            {
                try
                {
                    const auto& _ = ev.Open();
                }
                catch (const Gx::Exception& e)
                {
                    showError(e);
                    return;
                }

                m_waiting.StartGame([=] (const auto& e)
                {
                    try
                    {
                        const auto& _ = e.Open();
                    }
                    catch (const Gx::Exception& e)
                    {
                        showError(e);
                        return;
                    }

                    GetDirector().Present<StateLoading>(m_room.CreateGameContext());
                });
            });
        });


        btnBack->SetEnabled(false);
        btnBack->SetFocusChangedCallback([btnRetry] (auto& sender, const auto&)
        {
            btnRetry->SetFocus(sender.IsFocused() ? false : btnRetry->IsFocused());
            btnRetry->SetEnabled(!sender.IsFocused() && btnRetry->IsVisible());
        });
        btnBack->SetClickCallback([this] (auto& sender, const auto&)
        {
            sender.SetEnabled(false);
            m_service.ConfirmResult([=] (const auto& ev)
            {
                try
                {
                    const auto& _ = ev.Open();

                    GetDirector().Dismiss<StateWaiting7K>();
                }
                catch (const Gx::Exception& e)
                {
                    StopAll();
                    ShowDialog(std::string(e.what()), DialogStyle::Information, false, [=] (bool)
                    {
                        GetDirector().Dismiss<StatePlanet>();
                    });
                }
            });
        });

        auto topFx    = Run<Gx::Move>(*top, sf::Vector2f(0, 0), sf::seconds(2.f));
        auto bottomFx = Run<Gx::Sequence>([=]
            {
                background->SetVisible(true);
                banner->SetVisible(true);
                btnRetry->SetEnabled(m_room.GetMode() == GameMode::Single);
                btnBack->SetEnabled(true);
            },
            Gx::Move(*bottom, sf::Vector2f(0, view.getSize().y - bottom->GetLocalBounds().size.y), sf::seconds(2.f))
        );

        if (const auto bgm = Instantiate<sf::Music>(Sound::BGM::BG_RESULT); bgm)
            m_mixer.Play(*bgm, Sound::Channel::BGM);

        Run<Gx::Delay>(sf::seconds(10.f), [=] { btnBack->PerformClick(); });
    }

    void StateResult::Update(const sf::Time& delta)
    {
        State::Update(delta);
    }

    Gx::RenderStates StateResult::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        return State::Render(surface, states);
    }

    void StateResult::OnKeyPressed(const sf::Event::KeyPressed& ev)
    {
        State::OnKeyPressed(ev);

        if (ev.code == sf::Keyboard::Key::Enter)
        {
            if (const auto bottom = Instantiate<Gx::Image>(Resource::Result::IDC_IMAGE_STATE_RESULT_BOTTOM); bottom)
            {
                if (const auto btnBack = bottom->FindChild<Gx::Button>(Resource::Result::Bottom::IDC_BUTTON_BACK); btnBack)
                    btnBack->PerformClick();
            }
        }
    }
}
