#include <OTwo/States/StateResult.hpp>
#include <OTwo/States/StateWaiting7K.hpp>
#include <OTwo/States/StateLoading.hpp>
#include <OTwo/States/StatePlaying7K.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/GameContext.hpp>
#include <OTwo/IO/Loaders/Chart/ChartLoader.hpp>

#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Utilities/Randomizer.hpp>
#include <Genode/Fx/Move.hpp>

StateResult::StateResult(State &&state) :
    State(std::move(state))
{
}

void StateResult::Initialize()
{
    State::Initialize();

    auto& mixer              = Require<Gx::Mixer>();
    const auto& scoreTracker = Require<ScoreTracker>();
    const auto& session      = Require<SessionContext>();

    if (const auto container = Instantiate<Gx::UiContainer>("IDC_CONTAINER_BACKGROUND"); container)
    {
        const auto& resources = GetResources(ResourceScope::Shared);
        if (const auto texture = resources.Find<sf::Texture>("IDC_TEXTURE_STATE_PLAYING"); texture)
        {
            const auto fragment = Create<Gx::Sprite>(*texture);
            container->AddChild(fragment);
        }
    }

    auto const view = GetView();
    const auto background = Instantiate<Gx::Sprite>("IDC_IMAGE_STATE_RESULT");
    background->SetVisible(false);

    auto top = Instantiate<Gx::Sprite>("IDC_IMAGE_STATE_RESULT_TOP");
    auto bottom = Instantiate<Gx::Sprite>("IDC_IMAGE_STATE_RESULT_BOTTOM");

    const auto banner = Instantiate<Gx::Image>("IDC_IMAGE_WINLOSE");
    banner->SetVisible(false);

    auto height = bottom->GetLocalBounds().height;
    if (height < top->GetLocalBounds().height)
        height = top->GetLocalBounds().height;

    top->SetPosition(0.f, -height);
    bottom->SetPosition(0.f, view.getSize().y + height);

    if (const auto container = top->FindChild<Gx::UiContainer>("IDC_CONTAINER_PLAYER_SCORE"); container)
    {

        if (const auto cool = container->FindChild<Gx::Label>("IDC_TEXT_PLAYER_COOL"); cool)
            cool->SetString(std::to_string(scoreTracker.GetPoint(Accuracy::Cool)));

        if (const auto good = container->FindChild<Gx::Label>("IDC_TEXT_PLAYER_GOOD"); good)
            good->SetString(std::to_string(scoreTracker.GetPoint(Accuracy::Good)));

        if (const auto bad = container->FindChild<Gx::Label>("IDC_TEXT_PLAYER_BAD"); bad)
            bad->SetString(std::to_string(scoreTracker.GetPoint(Accuracy::Bad)));

        if (const auto miss = container->FindChild<Gx::Label>("IDC_TEXT_PLAYER_MISS"); miss)
            miss->SetString(std::to_string(scoreTracker.GetPoint(Accuracy::Miss)));

        if (const auto maxCombo = container->FindChild<Gx::Label>("IDC_TEXT_PLAYER_MAX_COMBO"); maxCombo)
            maxCombo->SetString(std::to_string(scoreTracker.GetMaxCombo()));

        if (const auto maxJamCombo = container->FindChild<Gx::Label>("IDC_TEXT_PLAYER_MAX_JAM_COMBO"); maxJamCombo)
            maxJamCombo->SetString(std::to_string(scoreTracker.GetMaxJamCombo()));
    }

    if (const auto point = top->FindChild<Gx::Number>("IDC_NUMBER_POINT"); point)
        point->SetValue(scoreTracker.GetScorePoint());

    if (const auto gem = top->FindChild<Gx::Label>("IDC_TEXT_GEM"); gem)
        gem->SetString(std::to_string(100));

    if (const auto list = bottom->FindChild<Gx::List>("IDC_LIST_RANK_SCORE"); list)
    {
        const auto listItems  = list->GetChildren();
        const auto scoreItems = session.GetLatestScoreResults();
        for (int i = 0; i < listItems.size(); i++)
        {
            const auto item = dynamic_cast<Gx::UiContainer*>(listItems[i]);
            if (!item)
                continue;

            if (i >= scoreItems.size() || scoreItems[i].Member.ID == 0)
            {
                item->SetVisible(false);
                continue;
            }

            if (const auto highlighter = item->FindChild<Gx::Rectangle>("IDC_RECTANGLE_HIGHLIGHT"); highlighter)
            {
                if (scoreItems[i].Member.ID == session.GetCurrentPlayer().ID)
                {
                    banner->SetFrame(i == 0 ? "win" : "lose");
                    highlighter->SetColor(sf::Color(184, 72, 0, 90));
                }
                else
                    highlighter->SetColor(sf::Color(200, 200, 200, 90));
            }

            if (const auto rank = item->FindChild<Gx::Label>("IDC_TEXT_RANK_NUMBER"); rank)
                rank->SetString(std::to_string(i + 1));

            if (const auto name = item->FindChild<Gx::Label>("IDC_TEXT_RANK_NAME"); name)
                name->SetString(scoreItems[i].Member.Name);

            if (const auto cool = item->FindChild<Gx::Label>("IDC_TEXT_RANK_COOL"); cool)
                cool->SetString(std::to_string(scoreItems[i].Cool));

            if (const auto good = item->FindChild<Gx::Label>("IDC_TEXT_RANK_GOOD"); good)
                good->SetString(std::to_string(scoreItems[i].Good));

            if (const auto bad = item->FindChild<Gx::Label>("IDC_TEXT_RANK_BAD"); bad)
                bad->SetString(std::to_string(scoreItems[i].Bad));

            if (const auto miss = item->FindChild<Gx::Label>("IDC_TEXT_RANK_MISS"); miss)
                miss->SetString(std::to_string(scoreItems[i].Miss));

            if (const auto maxCombo = item->FindChild<Gx::Label>("IDC_TEXT_RANK_MAX_COMBO"); maxCombo)
                maxCombo->SetString(std::to_string(scoreItems[i].MaxCombo));

            if (const auto maxJamCombo = item->FindChild<Gx::Label>("IDC_TEXT_RANK_MAX_JAM_COMBO"); maxJamCombo)
                maxJamCombo->SetString(std::to_string(scoreItems[i].MaxJamCombo));

            if (const auto point = item->FindChild<Gx::Label>("IDC_TEXT_RANK_POINT"); point)
                point->SetString(std::to_string(scoreItems[i].ScorePoint));
        }
    }

    const auto btnRetry = bottom->FindChild<Gx::Button>("IDC_BUTTON_PLAY_RETRY");
    btnRetry->SetEnabled(false);
    btnRetry->SetClickCallback([this, &mixer] (auto &sender, const auto &ev)
    {
        sender.SetEnabled(false);

        mixer.StopAll();
        GetDirector().Present<StateLoading>();
    });

    const auto btnBack = bottom->FindChild<Gx::Button>("IDC_BUTTON_BACK");
    btnBack->SetEnabled(false);
    btnBack->SetFocusChangedCallback([btnRetry] (auto &sender, const auto &ev)
    {
        btnRetry->SetFocus(sender.IsFocused() ? false : btnRetry->IsFocused());
        btnRetry->SetEnabled(!sender.IsFocused());
    });
    btnBack->SetClickCallback([this, &mixer] (auto &sender, const auto &ev)
    {
        sender.SetEnabled(false);

        mixer.StopAll();
        GetDirector().Present<StateWaiting7K>();
    });

    const auto topFx    = Create<Gx::Move>(top, sf::Vector2f(0, 0), sf::seconds(2.f));
    const auto bottomFx = Create<Gx::Sequence>([=]
        {
            background->SetVisible(true);
            banner->SetVisible(true);
            btnRetry->SetEnabled(true);
            btnBack->SetEnabled(true);
        },
        Gx::Sequence::ListOf(
        {
            Create<Gx::Move>(bottom, sf::Vector2f(0, view.getSize().y - bottom->GetLocalBounds().height), sf::seconds(2.f)),
        })
    );

    if (const auto bgm = Instantiate<sf::Music>("IDC_MUSIC"); bgm)
        mixer.Play(bgm, "BGM");

    Run(topFx, bottomFx);
   //  Run(Create<Gx::Delay>(sf::seconds(10.f), [=] { btnBack->PerformClick(); }));
}

void StateResult::Update(const double delta)
{
    State::Update(delta);
}

Gx::RenderStates StateResult::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
    return State::Render(surface, states);
}

void StateResult::OnKeyDown(const sf::Event::KeyEvent ev)
{
    State::OnKeyDown(ev);

    if (ev.code == sf::Keyboard::Key::Enter)
    {
        if (const auto bottom = Instantiate<Gx::Sprite>("IDC_IMAGE_STATE_RESULT_BOTTOM"); bottom)
        {
            if (const auto btnBack = bottom->FindChild<Gx::Button>("IDC_BUTTON_BACK"); btnBack)
                btnBack->PerformClick();
        }
    }
}
