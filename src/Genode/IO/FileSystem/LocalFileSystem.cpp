#include <Genode/IO/FileSystem/LocalFileSystem.hpp>

#include <SFML/System/FileInputStream.hpp>

#include <Genode/IO/FileSystem/FileInfo.hpp>
#include <Genode/Utilities/StringHelper.hpp>

#include <unordered_set>
#include <algorithm>
#include <fstream>
#include <filesystem>
using namespace std::filesystem;

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>

typedef OSStatus (*SecTranslocateIsTranslocatedURLFunc)(CFURLRef url, Boolean* isTranslocated);
typedef CFURLRef __nullable (*SecTranslocateCreateOriginalPathForURLFunc)(CFURLRef translocatedPath, CFErrorRef* __nullable error);
#endif

namespace Gx
{
    LocalFileSystem& LocalFileSystem::Instance()
    {
        static ResourcePtr<LocalFileSystem> instance;
        if (!instance)
            instance = ResourcePtr<LocalFileSystem>(new LocalFileSystem(), [] (auto ptr) { delete ptr; });

        return *instance.get();
    }

    std::string LocalFileSystem::GetApplicationDirectoryPath()
    {
        // Windows and Linux doesn't have to be using the actual path of the executable.
        // This because the operating system allows the user to manually specify working directory on their own.
        auto workingDir = std::filesystem::current_path().string();

#ifdef __APPLE__
        // On the other hand, macOS "security" modify the working directory out of developer / user will.
        // Context: https://lapcatsoftware.com/articles/app-translocation.html

        // TODO: This "de-translocation" may not desirable for general use cases.
        //       Consider to remove this particular handling from public repo

        // Get the current bundle
        CFBundleRef mainBundle;
        if (mainBundle = CFBundleGetMainBundle(); !mainBundle)
            return workingDir;

        // Get the URL of main bunddle
        CFURLRef mainBundleURL;
        if (mainBundleURL = CFBundleCopyBundleURL(mainBundle); !mainBundleURL)
        {
            CFRelease(mainBundle);
            return workingDir;
        }

        // Extract URL into std::string
        if (CFStringRef path = CFURLCopyFileSystemPath(mainBundleURL, kCFURLPOSIXPathStyle); path)
        {
            char buffer[PATH_MAX];
            CFStringGetCString(path, buffer, sizeof(buffer), kCFStringEncodingUTF8);

            workingDir = std::string(buffer);
            CFRelease(path);
        }

        // Release bundle ref
        CFRelease(mainBundle);

        // Load security framework
        auto handle = dlopen("/System/Library/Frameworks/Security.framework/Security", RTLD_LAZY);
        if (!handle)
        {
            CFRelease(mainBundleURL);
            return workingDir;
        }

        // Load SecTranslocateIsTranslocatedURL
        auto SecTranslocateIsTranslocatedURL = reinterpret_cast<SecTranslocateIsTranslocatedURLFunc>(dlsym(handle, "SecTranslocateIsTranslocatedURL"));
        if (SecTranslocateIsTranslocatedURL == nullptr)
        {
            CFRelease(mainBundleURL);
            dlclose(handle);
            return workingDir;
        }

        // Check whether the current working directory is translocated
        Boolean isTranslocated = false;
        if (auto status = SecTranslocateIsTranslocatedURL(mainBundleURL, &isTranslocated); !status || !isTranslocated)
        {
            CFRelease(mainBundleURL);
            dlclose(handle);
            return workingDir;
        }

        // Load SecTranslocateCreateOriginalPathForURL
        auto SecTranslocateCreateOriginalPathForURL = reinterpret_cast<SecTranslocateCreateOriginalPathForURLFunc>(dlsym(handle, "SecTranslocateCreateOriginalPathForURL"));
        if (SecTranslocateCreateOriginalPathForURL == nullptr)
        {
            CFRelease(mainBundleURL);
            dlclose(handle);
            return workingDir;
        }

        // Get Untranslocated Current Working Directory
        auto appURL = SecTranslocateCreateOriginalPathForURL(mainBundleURL, nullptr);
        if (!appURL)
        {
            CFRelease(mainBundleURL);
            CFRelease(appURL);
            dlclose(handle);
            return workingDir;
        }

        // Convert the URL into string
        if (auto appPath = CFURLCopyFileSystemPath(appURL, kCFURLPOSIXPathStyle); appPath)
        {
            char buffer[PATH_MAX];
            CFStringGetCString(appPath, buffer, sizeof(buffer), kCFStringEncodingUTF8);

            workingDir = std::string(buffer);
        }

        // Clean up
        CFRelease(mainBundleURL);
        CFRelease(appURL);
        dlclose(handle);
#endif

        return workingDir;
    }

    std::string LocalFileSystem::GetWorkingDirectory()
    {
        return std::filesystem::current_path().string();
    }

    void LocalFileSystem::SetWorkingDirectory(const std::string& inputPath)
    {
        auto workingDir = path(inputPath);

#ifdef __APPLE__
        if (workingDir.has_filename() || workingDir.has_extension())
            workingDir = workingDir.parent_path();
#endif

        if (!exists(workingDir))
            return;

        std::filesystem::current_path(workingDir);
    }

    std::vector<std::string> LocalFileSystem::GetAssetPaths()
    {
        auto paths = std::vector<std::string>();
        for (const auto& p : m_paths)
            paths.push_back(weakly_canonical(path(p)).string());

        return paths;
    }

    void LocalFileSystem::AddAssetPath(const std::string& path)
    {
        m_paths.push_back(path);
    }

    ResourcePtr<sf::InputStream> LocalFileSystem::Open(const std::string& fileName) const
    {
        const auto fileStream = new sf::FileInputStream();
        auto stream = ResourcePtr<sf::InputStream>(fileStream, [] (auto fs) { delete fs; });
        if (fileStream->open(GetFullName(fileName)))
            return stream;

        return nullptr;
    }

    std::unique_ptr<FileInfo> LocalFileSystem::GetFileInfo(const std::string& fileName) const
    {
        const auto fullName = GetFullName(fileName);
        const auto size = GetFileSize(fullName).value_or(0);

        return std::make_unique<FileInfo>(FileInfo(*this, fullName, size));
    }

    std::optional<std::size_t> LocalFileSystem::GetFileSize(const std::string& fileName) const
    {
        if (auto fileStream = sf::FileInputStream(); fileStream.open(GetFullName(fileName)))
            return fileStream.getSize();

        return std::nullopt;
    }

    bool LocalFileSystem::Contains(const std::string& fileName) const
    {
        if (fileName.empty())
            return false;

        if (const auto filePath = path(fileName.c_str()); exists(filePath))
            return true;

        return std::any_of(m_paths.begin(), m_paths.end(), [fileName] (const std::string& path)
        {
            const std::string fullPath = std::string(path).append("/").append(fileName);
            return exists(fullPath.c_str());
        });
    }

    std::string LocalFileSystem::GetFileName(const std::string& fullPath, const bool withExtension) const
    {
        const auto filePath = path(fullPath.c_str());
        if (withExtension)
            return filePath.filename().string();

        return filePath.filename().replace_extension().string();
    }

    std::string LocalFileSystem::GetFullName(const std::string& fileName, const bool withExtension) const
    {
        if (exists(fileName))
        {
            if (withExtension)
                return fileName;

            return path(fileName).replace_extension().string();
        }

        for (std::string& dir : m_paths)
        {
            if (std::string fullPath = std::string(dir).append("/").append(fileName); exists(fullPath))
            {
                if (withExtension)
                    return fullPath;

                return path(fullPath).replace_extension().string();
            }
        }

        return "";
    }

    std::optional<std::size_t> LocalFileSystem::ReadFile(const std::string& fileName, void* data, std::size_t size) const
    {
        sf::FileInputStream fs;
        if (!fs.open(GetFullName(fileName)))
            return -1;

        if (size < 0)
            size = fs.getSize().value_or(0);

        if (size <= 0)
            return std::nullopt;

        return fs.read(data, size);
    }

    void LocalFileSystem::WriteFile(const std::string& fileName, const void* data, const std::size_t size)
    {
        if (size <= 0)
            return;

        std::ofstream fs(fileName.c_str(), std::ios::out | std::ios::binary);
        fs.write(static_cast<const char*>(data), size);

        fs.close();
    }

    std::vector<std::unique_ptr<FileInfo>> LocalFileSystem::Scan(const std::string& pattern, const bool recursive) const
    {
        std::vector<std::unique_ptr<FileInfo>> files;
        std::unordered_set<std::string> scanned;

        auto paths = m_paths;
        for (const auto& dir : paths)
        {
            if (!exists(dir))
                continue;

            for (const auto& entry : directory_iterator(dir))
            {
                if (is_directory(entry) && recursive)
                {
                    paths.push_back(entry.path().string());
                    continue;
                }

                if (!is_regular_file(entry))
                    continue;

                auto fileName = weakly_canonical(entry.path()).string();
                if (auto [_, inserted] = scanned.insert(fileName); !inserted)
                    continue;

                if (StringHelper::IsGlobMatch(entry.path().filename().string(), pattern))
                    files.push_back(std::make_unique<FileInfo>(*this, fileName, GetFileSize(fileName).value_or(0)));
            }
        }

        return files;
    }
}
