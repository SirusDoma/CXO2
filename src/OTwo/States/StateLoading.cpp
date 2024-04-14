#include <OTwo/States/StateLoading.hpp>

#include <OTwo/Data/UserState.hpp>
#include <OTwo/Data/ChartState.hpp>
#include <OTwo/IO/Loaders/Chart/ChartLoader.hpp>

#include <Genode/UI/Image.hpp>

#include <random>
#include <thread>
#include <iostream>

StateLoading::StateLoading(State &state) :
    State(state),
    m_cover(nullptr)
{
}

void StateLoading::Initialize()
{
    State::Initialize();

    auto &user  = Require<UserState>();
    auto &state = Require<ChartState>();
    auto &room  = user.GetCurrentRoom();

    auto metadata  = room.Chart;
    auto loader = ChartLoader();
    loader.SetCoverLoadCallback([this] (auto cover)
    {
        m_cover = cover;

        auto imageList = std::vector<Gx::Image*>();
        for (auto child : GetChildren())
        {
            if (auto image = dynamic_cast<Gx::Image*>(child); image)
                imageList.push_back(image);
        }

        auto device     = std::random_device();
        auto seeder     = std::mt19937(device());
        auto randomizer = std::uniform_int_distribution<int>(0, static_cast<int>(imageList.size()) - 1);
        int result      = randomizer(seeder);

        for (int i = 0; i < imageList.size(); i++)
            imageList[i]->SetVisible(i == result);
    });

    for (auto child : GetChildren())
    {
        if (auto image = dynamic_cast<Gx::Image*>(child); image)
            image->SetVisible(false);
    }

    auto thread = std::thread([=, &state] ()
    {
        state.SetChart(loader.LoadFromFile(metadata.Source, Gx::ResourceContext("o2ma" + metadata.ID)));
    });

    thread.detach();
}

void StateLoading::Update(double delta)
{
    State::Update(delta);

    if (m_cover)
    {
        for (auto child : GetChildren())
        {
            if (auto image = dynamic_cast<Gx::Image*>(child); image)
            {
                if (image->IsVislble() && m_texture.loadFromImage(*m_cover))
                {
                    image->SetTexture(m_texture);
                    break;
                }
            }
        }

        m_cover = nullptr;
    }

    auto &state = Require<ChartState>();
    if (state.GetChart())
    {
        // Loaded
    }
}
