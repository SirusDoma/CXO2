#include <OTwo/States/StateResult.hpp>
#include <OTwo/States/StateWaiting7K.hpp>
#include <OTwo/States/StateLoading.hpp>
#include <OTwo/States/StatePlaying7K.hpp>

#include <OTwo/Core/ScoreTracker.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/GameContext.hpp>
#include <OTwo/IO/Loaders/Chart/ChartLoader.hpp>

#include <OTwo/StringTable/Identifiers/Cache.hpp>
#include <OTwo/StringTable/Identifiers/Sound.hpp>
#include <OTwo/StringTable/Identifiers/Result.hpp>

#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Tween/Move.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/Utilities/Randomizer.hpp>

using namespace StringTable::Identifiers;

StateResult::                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       StateResult(Gx::AudioMixer& mixer, SessionContext& session, const ScoreTracker& scoreTracker) :
    m_mixer(mixer),
    m_session(session),
    m_scoreTracker(scoreTracker)
{
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

    auto top = Instantiate<Gx::Sprite>(Resource::Result::IDC_IMAGE_STATE_RESULT_TOP);
    auto bottom = Instantiate<Gx::Sprite>(Resource::Result::IDC_IMAGE_STATE_RESULT_BOTTOM);

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
        const auto scoreItems = m_session.GetLatestScoreResults();
        for (std::size_t i = 0; i < listItems.size(); i++)
        {
            const auto item = dynamic_cast<Gx::UiContainer*>(listItems[i]);
            if (!item)
                continue;

            if (i >= scoreItems.size() || scoreItems[i].Member.ID == 0)
            {
                item->SetVisible(false);
                continue;
            }

            // TODO: Make this adjustable?

            auto primaryTeamColor = std::unordered_map<RoomTeam, sf::Color>
            ({
                { RoomTeam::A, sf::Color(255, 000, 16, 50) },
                { RoomTeam::B, sf::Color(255, 190, 16, 50) },
                { RoomTeam::C, sf::Color(247, 255, 0, 50) },
                { RoomTeam::D, sf::Color(155, 206, 82, 50) },
                { RoomTeam::E, sf::Color(107, 235, 230, 50) },
                { RoomTeam::F, sf::Color(000, 000, 230, 50) },
                { RoomTeam::G, sf::Color(156, 81, 214, 50) },
                { RoomTeam::H, sf::Color(181, 77, 025, 50) },
            });

            auto secondaryTeamColor = std::unordered_map<RoomTeam, sf::Color>
            ({
                { RoomTeam::A, sf::Color(123, 130, 16, 50) },
                { RoomTeam::B, sf::Color(123, 223, 16, 50) },
                { RoomTeam::C, sf::Color(123, 125, 0, 50) },
                { RoomTeam::D, sf::Color(74, 231, 82, 50) },
                { RoomTeam::E, sf::Color(49, 247, 230, 50) },
                { RoomTeam::F, sf::Color(000, 000, 230, 50) },
                { RoomTeam::G, sf::Color(74, 170, 214, 50) },
                { RoomTeam::H, sf::Color(90, 36, 25, 50) },
            });

            if (scoreItems[i].Member.ID == m_session.GetCurrentPlayer().ID)
                banner->SetFrame(i == 0 ? "win" : "lose");

            if (const auto highlighter = item->FindChild<Gx::Rectangle>(Resource::Result::Bottom::Score::IDC_RECTANGLE_HIGHLIGHT); highlighter)
            {
                if (scoreItems[i].Member.ID == m_session.GetCurrentPlayer().ID)
                    highlighter->SetColor(primaryTeamColor[scoreItems[i].Member.Team]);
                else
                    highlighter->SetColor(secondaryTeamColor[scoreItems[i].Member.Team]);
            }

            if (const auto rank = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_NUMBER); rank)
                rank->SetString(std::to_string(i + 1));

            if (const auto name = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_NAME); name)
                name->SetString(scoreItems[i].Member.Name);

            if (const auto cool = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_COOL); cool)
                cool->SetString(std::to_string(scoreItems[i].Cool));

            if (const auto good = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_GOOD); good)
                good->SetString(std::to_string(scoreItems[i].Good));

            if (const auto bad = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_BAD); bad)
                bad->SetString(std::to_string(scoreItems[i].Bad));

            if (const auto miss = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_MISS); miss)
                miss->SetString(std::to_string(scoreItems[i].Miss));

            if (const auto maxCombo = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_MAX_COMBO); maxCombo)
                maxCombo->SetString(std::to_string(scoreItems[i].MaxCombo));

            if (const auto maxJamCombo = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_MAX_JAM_COMBO); maxJamCombo)
                maxJamCombo->SetString(std::to_string(scoreItems[i].MaxJamCombo));

            if (const auto point = item->FindChild<Gx::Label>(Resource::Result::Bottom::Score::IDC_TEXT_RANK_POINT); point)
                point->SetString(std::to_string(scoreItems[i].ScorePoint));
        }
    }

    const auto btnRetry = bottom->FindChild<Gx::Button>(Resource::Result::Bottom::IDC_BUTTON_PLAY_RETRY);
    btnRetry->SetEnabled(false);
    btnRetry->SetClickCallback([this] (auto& sender, const auto& ev)
    {
        sender.SetEnabled(false);
        GetDirector().Present<StateLoading>();
    });

    const auto btnBack = bottom->FindChild<Gx::Button>(Resource::Result::Bottom::IDC_BUTTON_BACK);
    btnBack->SetEnabled(false);
    btnBack->SetFocusChangedCallback([btnRetry] (auto& sender, const auto& ev)
    {
        btnRetry->SetFocus(sender.IsFocused() ? false : btnRetry->IsFocused());
        btnRetry->SetEnabled(!sender.IsFocused());
    });
    btnBack->SetClickCallback([this] (auto& sender, const auto& ev)
    {
        sender.SetEnabled(false);
        GetDirector().Dismiss<StateWaiting7K>();
    });

    auto topFx    = Run<Gx::Move>(*top, sf::Vector2f(0, 0), sf::seconds(2.f));
    auto bottomFx = Run<Gx::Sequence>([=]
        {
            background->SetVisible(true);
            banner->SetVisible(true);
            btnRetry->SetEnabled(true);
            btnBack->SetEnabled(true);
        },
        Gx::Move(*bottom, sf::Vector2f(0, view.getSize().y - bottom->GetLocalBounds().size.y), sf::seconds(2.f))
    );

    if (const auto bgm = Instantiate<sf::Music>(Sound::BGM::BG_RESULT); bgm)
        m_mixer.Play(*bgm, Sound::Channel::BGM);

    Run<Gx::Delay>(sf::seconds(10.f), [=] { btnBack->PerformClick(); });
}

void StateResult::Update(const double delta)
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
        if (const auto bottom = Instantiate<Gx::Sprite>(Resource::Result::IDC_IMAGE_STATE_RESULT_BOTTOM); bottom)
        {
            if (const auto btnBack = bottom->FindChild<Gx::Button>(Resource::Result::Bottom::IDC_BUTTON_BACK); btnBack)
                btnBack->PerformClick();
        }
    }
}
