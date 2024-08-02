#include <OTwo/States/StateLoading.hpp>

#include <OTwo/States/StatePlaying7K.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/GameContext.hpp>
#include <OTwo/IO/Loaders/Chart/ChartLoader.hpp>

#include <Genode/UI/Image.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Utilities/Randomizer.hpp>

#include <thread>

StateLoading::StateLoading(State &&state) :
    State(std::move(state))
{
}

void StateLoading::Initialize()
{
    State::Initialize();

    auto &session = Require<SessionContext>();
    auto &room    = session.GetCurrentRoom();
    auto &state   = Require<GameContext>();

    state.SetMode(room.GameMode);
    state.SetDifficulty(room.Difficulty);

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

    const auto metadata = room.Chart;
    auto loader         = ChartLoader();

    // TODO: Load cover after select music in StateWaiting and forward it into StateLoading
    loader.SetCoverLoadCallback([this] (auto cover)
    {
        Queue([this, cover] () { OnCoverLoaded(cover); });
    });

    auto thread = std::thread([=, &state] ()
    {
        state.SetChart(loader.LoadFromFile(metadata.Source, Gx::ResourceContext("o2ma" + metadata.ID)));
        Queue([this, &state] { OnChartLoaded(state.GetChart()); });
    });

    thread.detach();
}

void StateLoading::Update(const double delta)
{
    State::Update(delta);
}

void StateLoading::OnCoverLoaded(const sf::Image *cover)
{
    for (const auto child : GetChildren())
    {
        if (const auto image = dynamic_cast<Gx::Image*>(child); image && image->IsVislble() && m_texture.loadFromImage(*cover))
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
            auto &app      = GetApplication();
            auto &director = GetDirector();
            auto &session  = Require<SessionContext>();
            auto &config   = app.GetConfig<GameConfig>();
            auto &room     = session.GetCurrentRoom();
            auto ctx       = PlayingResourceContext();

            ctx.SetFxEnabled(config.UseFx);
            ctx.SetMapID(room.MapID);
            ctx.SetEffectID(room.EffectID);

            director.Present<StatePlaying7K>(ctx);
        },
        Gx::Sequence::ListOf({
            Create<Gx::Delay>(sf::seconds(1.f))
        })
    );

    Run(transition);
}
