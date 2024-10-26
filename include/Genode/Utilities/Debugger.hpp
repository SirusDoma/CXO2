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

namespace Gx
{
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
    };
}
