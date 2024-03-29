#include <OTwo/O2Jam.hpp>

#include <Genode/IO/FileSystem/LocalFileSystem.hpp>
#include <Genode/System/Exception.hpp>

#include <boxer/boxer.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>

std::string GetAppBundlePath()
{
    std::string appBundlePath;
    if (CFBundleRef mainBundle = CFBundleGetMainBundle(); mainBundle)
    {
        if (CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle); mainBundleURL)
        {
            if (CFStringRef path = CFURLCopyFileSystemPath(mainBundleURL, kCFURLPOSIXPathStyle); path)
            {
                char buffer[PATH_MAX];
                CFStringGetCString(path, buffer, sizeof(buffer), kCFStringEncodingUTF8);
                appBundlePath = buffer;
                CFRelease(path);
            }
            CFRelease(mainBundleURL);
        }
        CFRelease(mainBundle);
    }
    return appBundlePath;
}
#endif

#ifdef __linux__
#include <unistd.h>
#include <sys/prctl.h>
#include <signal.h>
#endif

bool IsDebuggerAttached()
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
    if (sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, nullptr, 0) == -1) {
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

int main(int argc , char** argv)
{
    try
    {
        // Fix weird working directory in macOS
        if (!IsDebuggerAttached())
        {
            std::string path = std::string(argv[0]);
#ifdef __APPLE__
            // Handle scenario where app bundle is sandboxed by the OS
            if (auto bundlePath = GetAppBundlePath(); bundlePath.rfind("/private/var/folders/") != 0)
                path = bundlePath;
#endif
            Gx::LocalFileSystem::SetWorkingDirectory(path);
        }

        auto o2jam = O2Jam("O2-JAM", sf::VideoMode(800, 600), sf::VideoMode(800, 600));
        return o2jam.Start();
    }
    catch (std::exception &ex)
    {
        if (IsDebuggerAttached())
            throw ex;

        auto details = std::string();
        if (typeid(ex) == typeid(Gx::ResourceAccessException) || typeid(ex) == typeid(Gx::ResourceLoadException))
        {
            details += "\n\nMounted Paths:\n";
            for (auto &path: Gx::LocalFileSystem::GetAssetPaths())
                details += "- " + path + "\n\n";
        }

        boxer::show(std::string(std::string(ex.what()) + details).c_str(), "Fatal Error", boxer::Style::Error);
        throw ex;
    }
}
