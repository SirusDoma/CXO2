#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#ifdef __linux__
#include <unistd.h>
#include <sys/prctl.h>
#include <signal.h>
#endif

#ifndef NDEBUG
#include <livepp/API/x64/LPP_API_x64_CPP.h>
#endif

namespace Gx
{
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

    private:
        std::any m_agent;
    };

    class Debugger
    {
    public:
        static bool IsDebuggerAttached()
        {
#ifdef _WIN32
            return IsDebuggerPresent();
#endif

#ifdef __APPLE__
            int mib[4];
            struct kinfo_proc info{};
            size_t size;

            mib[0] = CTL_KERN;
            mib[1] = KERN_PROC;
            mib[2] = KERN_PROC_PID;
            mib[3] = getpid();

            size = sizeof(info);
            if (sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, nullptr, 0) == -1)
            {
                return false;
            }

            return (info.kp_proc.p_flag & P_TRACED) != 0;
#endif

#ifdef __linux__
            // Check if the process is being traced by a debugger
            return prctl(PR_GET_DUMPABLE, 0, 0, 0, 0) != 0;
#endif

            // Unsupported platform
            return false;
        }

        static void Watch()
        {
            const auto agent = lpp::LppCreateDefaultAgent(nullptr,  L"extlibs/libs/internal/livepp");

            if (!lpp::LppIsValidDefaultAgent(&agent))
                return;

            agent.EnableModule(lpp::LppGetCurrentModulePath(), lpp::LPP_MODULES_OPTION_ALL_IMPORT_MODULES, nullptr, nullptr);
            static DebuggerWatcher watcher = { agent };
        }
    };
}
