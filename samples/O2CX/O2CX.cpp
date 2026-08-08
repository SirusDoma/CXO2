#include <Log.hpp>
#include <StatelessExtension.hpp>
#include <Extensions/AccuracyMeterExtension.hpp>
#include <Extensions/ChatExtension.hpp>
#include <Extensions/MusicSearchExtension.hpp>
#include <Extensions/TetrisExtension.hpp>
#include <Extensions/WelcomeExtension.hpp>

#include <CXO2/Services/PluginService.hpp>

#include <memory>
#include <vector>

#ifdef _WIN32
#define O2CX_EXPORT __declspec(dllexport)
#else
#define O2CX_EXPORT __attribute__((visibility("default")))
#endif

namespace
{
    std::vector<std::unique_ptr<O2CX::StatelessExtension>> extensions;
    O2CX::SubscriptionSet subscribers;
}

extern "C" O2CX_EXPORT int O2CX_StartUp(const Cx::AppPluginContext& app)
{
    O2CX::Log("O2CX_StartUp: attached");

    extensions.push_back(std::make_unique<O2CX::WelcomeExtension>());
    extensions.push_back(std::make_unique<O2CX::ChatExtension>());

    for (const auto& extension : extensions)
        extension->Attach(app);

    subscribers.Add(app.Events.On(Cx::StateEvents::OnExtend, [&events = app.Events] (Cx::State& sender, Cx::StateEventArgs& ev)
    {
        if (ev.Key == Cx::StateKey::Waiting7K)
            sender.AddExtension<O2CX::MusicSearchExtension>(events);
        else if (ev.Key == Cx::StateKey::Playing7K)
            sender.AddExtension<O2CX::AccuracyMeterExtension>(events);

        // Uncomment if you want to play tetris instead
        // else if (ev.Key == Cx::StateKey::Planet)
        //     sender.AddExtension<O2CX::TetrisExtension>();
    }));

    return 0;
}

extern "C" O2CX_EXPORT void O2CX_ShutDown()
{
    O2CX::Log("O2CX_ShutDown: detached");

    subscribers.Clear();
    for (const auto& extension : extensions)
        extension->Detach();

    extensions.clear();
}
