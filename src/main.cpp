#include <OTwo/O2Jam.hpp>

#include <Genode/IO/FileSystem/LocalFileSystem.hpp>
#include <Genode/System/Exception.hpp>

#include <boxer/boxer.h>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>

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

int main(int argc , char** argv)
{
    try
    {
        // Fix weird working directory in macOS
        std::string path = std::string(argv[0]);
#ifdef __APPLE__
        path = GetAppBundlePath();
#endif
        Gx::LocalFileSystem::SetWorkingDirectory(path);

        auto o2jam = O2Jam("O2-JAM", sf::VideoMode(800, 600), sf::VideoMode(800, 600));
        return o2jam.Start();
    }
    catch (std::exception &ex)
    {
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
