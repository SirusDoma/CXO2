#include <Extensions/WelcomeExtension.hpp>
#include <Log.hpp>

namespace O2CX
{
    void WelcomeExtension::OnAttached(const Cx::AppPluginContext& app)
    {
        GetSubscriptions().Add(app.Events.On(Cx::StateEvents::OnInitialize, [this] (Cx::State& sender, Cx::StateEventArgs& ev)
        {
            if (ev.Key != Cx::StateKey::Room || m_welcomed)
                return;

            m_welcomed = true;
            sender.Invoke([&sender]
            {
                Log("WelcomeMod: showing welcome dialog");
                sender.ShowDialog("Welcome to O2Jam Online!\nThe O2CX plugin is active.", Cx::DialogStyle::Information);
            });
        }));
    }
}
