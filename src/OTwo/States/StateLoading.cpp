#include <OTwo/States/StateLoading.hpp>

#include <OTwo/Data/UserState.hpp>
#include <OTwo/Data/ChartState.hpp>
#include <OTwo/IO/Loaders/Chart/ChartLoader.hpp>

#include <Genode/UI/Image.hpp>

#include <random>
#include <thread>
#include <iostream>
#include <OTwo/States/StatePlaying.hpp>
#include <Genode/Tasks/Sequence.hpp>

StateLoading::StateLoading(State &state) :
    State(state)
{
}

void StateLoading::Initialize()
{
    State::Initialize();

    auto &user  = Require<UserState>();
    auto &room  = user.GetCurrentRoom();
    auto &state = Require<ChartState>();
    state.SetMode(room.GameMode);
    state.SetDifficulty(room.Difficulty);

    auto device      = std::random_device();
    auto seeder      = std::mt19937(device());
    auto randomizer  = std::uniform_int_distribution<int>(0, static_cast<int>(GetChildren().size()) - 1);
    const int result = randomizer(seeder);

    std::size_t index = 0;
    for (const auto child : GetChildren())
    {
        if (const auto image = dynamic_cast<Gx::Image*>(child); image)
        {
            image->SetVisible(index == result);
            index++;
        }
    }

    const auto metadata  = room.Chart;
    auto loader          = ChartLoader();

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
            auto &director = GetDirector();
            auto &state    = Require<UserState>();
            auto &room     = state.GetCurrentRoom();
            auto ctx       = PlayingResourceContext();

            ctx.SetMapID(room.MapID);
            ctx.SetEffectID(room.EffectID);

            director.Present<StatePlaying>(ctx);
        },
        Gx::Sequence::ListOf({
            Create<Gx::Delay>(sf::seconds(1.f))
        })
    );

    Run(transition);
}
