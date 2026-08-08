#pragma once

#include <StatelessExtension.hpp>

namespace O2CX
{
    class WelcomeExtension : public StatelessExtension
    {
    protected:
        void OnAttached(const Cx::AppPluginContext& app) override;

    private:
        bool m_welcomed{false};
    };
}
