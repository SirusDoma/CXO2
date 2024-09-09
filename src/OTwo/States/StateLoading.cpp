#include <OTwo/States/StateLoading.hpp>

#include <OTwo/States/StatePlaying7K.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/GameContext.hpp>
#include <OTwo/IO/PlayingResourceContext.hpp>

#include <Genode/UI/Image.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Utilities/Randomizer.hpp>

#include <thread>

StateLoading::StateLoading(const SessionContext& session, GameContext& game, GameConfig& config) :
    m_session(session),
    m_game(game),
    m_config(config)
{
}

void StateLoading::Initialize()
{
    State::Initialize();
    
    const auto& room      = m_session.GetCurrentRoom();
    const auto& resources = GetResources(ResourceScope::Shared);

    std::size_t index = 0;
    const int result = Gx::Randomizer::Randomize(0,  static_cast<int>(GetChildren().size()) - 1);

    for (const auto child : GetChildren())
    {
        if (const auto image = dynamic_cast<Gx::Image*>(child); image)
        {
            image->SetVisible(index == result);
            index++;
        }
    }

    const auto metadata = room.ChartMetadata;
    m_game.SetConfig(m_config);
    m_game.SetMode(room.GameMode);
    m_game.SetDifficulty(room.Difficulty);
    m_game.SetSpeed(room.Speed);

    if (!m_game.GetChart() || std::to_string(m_game.GetChart()->GetMetadata().ID) != metadata.ID)
    {
        auto loader = ChartLoader(m_game);
        if (const auto image = resources.Find<sf::Image>("IDC_IMAGE_STATE_LOADING_COVER"); image)
        {
            OnCoverLoaded(image);
        }
        else
        {
            loader.SetCoverLoadCallback([this] (auto cover)
            {
                Queue([this, cover] () { OnCoverLoaded(cover); });
            });
        }

        auto thread = std::thread([=] ()
        {
            m_game.SetChart(loader.LoadFromFile(metadata.Source, Gx::ResourceContext("o2ma" + metadata.ID)));
            Queue([this] { OnChartLoaded(m_game.GetChart()); });
        });

        thread.detach();
    }
    else
    {
        if (const auto image = resources.Find<sf::Image>("IDC_IMAGE_STATE_LOADING_COVER"); image)
            OnCoverLoaded(image);

        Run(Create<Gx::Delay>(sf::seconds(0.5f), [this] { OnChartLoaded(m_game.GetChart()); }));
    }
}

void StateLoading::Update(const double delta)
{
    State::Update(delta);
}

void StateLoading::OnCoverLoaded(const sf::Image *cover)
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

void StateLoading::OnChartLoaded(const Chart *chart)
{
    const auto transition = Create<Gx::Sequence>([this, chart]
        {
            auto &director = GetDirector();
            auto &room     = m_session.GetCurrentRoom();
            auto ctx       = PlayingResourceContext();

            ctx.SetFxEnabled(m_config.UseFx);
            ctx.SetMapID(room.MapID);
            ctx.SetEffectID(room.EffectID);

            director.Present<StatePlaying7K>(ctx);
        },
        Gx::Sequence::ListOf({
            Create<Gx::Delay>(sf::seconds(0.5f))
        })
    );

    Run(transition);
}
