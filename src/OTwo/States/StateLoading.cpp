#include <OTwo/States/StateLoading.hpp>

#include <OTwo/States/StatePlaying7K.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/GameContext.hpp>
#include <OTwo/IO/PlayingResourceContext.hpp>

#include <OTwo/StringTable/Identifiers/Cache.hpp>

#include <Genode/UI/Image.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Utilities/Randomizer.hpp>

#include <thread>

using namespace StringTable::Identifiers;

StateLoading::StateLoading(GameContext& game) :
    m_context(game)
{
}

void StateLoading::Initialize()
{
    State::Initialize();

    std::size_t index = 0;
    const int result = Gx::Randomizer::Randomize(0,  static_cast<int>(GetChildrenCount()) - 1);

    for (const auto child : GetChildren())
    {
        if (const auto image = dynamic_cast<Gx::Image*>(child); image)
        {
            image->SetVisible(index == result);
            index++;
        }
    }

    const auto chart      = m_context.GetChart();
    const auto& resources = GetResources(ResourceScope::Shared);
    auto loader           = ChartLoader(m_context);

    if (const auto image = resources.Find<sf::Image>(Resource::Cache::IDC_IMAGE_STATE_LOADING_COVER); image)
    {
        OnCoverLoaded(image);
    }
    else
    {
        loader.SetCoverLoadCallback([this] (auto cover)
        {
            if (cover)
                OnCoverLoaded(cover);
        });
    }

    auto thread = std::thread([=] ()
    {
        m_context.SetChart(loader.LoadFromFile(chart->Source, Gx::ResourceContext::Default));
        QueueEvent([this] { OnChartLoaded(m_context.GetChart()); });
    });

    thread.detach();
}

bool StateLoading::IsTrackable()
{
    return false;
}

void StateLoading::Update(const double delta)
{
    State::Update(delta);
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
    Run<Gx::Sequence>([this, chart]
        {
            auto& director = GetDirector();
            auto ctx       = PlayingResourceContext();

            ctx.SetFxEnabled(m_context.GetConfig().UseFx);
            ctx.SetMapID(m_context.GetMapID());
            ctx.SetEffectID(m_context.GetEffectID());
            ctx.SetMode(m_context.GetMode());

            director.Present<StatePlaying7K>(ctx);
        },
        Gx::Delay(sf::seconds(0.5f))
    );
}
