#pragma once

#ifndef NDEBUG
#include <windows.h>
#include <livepp/API/x64/LPP_API_x64_CPP.h>
#endif

#include <any>

class DebuggerWatcher
{
public:
    DebuggerWatcher() {}

    // ReSharper disable once CppNonExplicitConvertingConstructor
    DebuggerWatcher(const std::any& agent) : m_agent(agent) {}

    ~DebuggerWatcher()
    {
#ifndef NDEBUG
        if (m_agent.has_value())
        {
            const auto& agent = std::any_cast<lpp::LppDefaultAgent>(m_agent);
            lpp::LppDestroyDefaultAgent(const_cast<lpp::LppDefaultAgent*>(&agent));
        }
#endif
    }

    static void Watch()
    {
#ifndef NDEBUG
        const auto agent = lpp::LppCreateDefaultAgent(nullptr,  L"extlibs/libs/internal/livepp");

        if (!lpp::LppIsValidDefaultAgent(&agent))
            return;

        agent.EnableModule(lpp::LppGetCurrentModulePath(), lpp::LPP_MODULES_OPTION_ALL_IMPORT_MODULES, nullptr, nullptr);
        static DebuggerWatcher watcher = { agent };
#endif
    }

private:
    std::any m_agent;
};