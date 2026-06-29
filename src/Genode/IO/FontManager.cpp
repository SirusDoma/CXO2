#include <Genode/IO/FontManager.hpp>

#include <SFML/System/MemoryInputStream.hpp>

#include <algorithm>
#include <filesystem>
#include <mutex>
#include <string_view>
#include <limits.h>

#if defined(_WIN32)
    #define NOMINMAX
    #include <windows.h>
    #include <wingdi.h>
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #include <CoreText/CoreText.h>
    #include <CoreFoundation/CoreFoundation.h>
#else
    // Linux/Unix: search typical font directories
    #include <unistd.h>
    #ifdef USE_FONTCONFIG
        #include <fontconfig/fontconfig.h>
    #endif
#endif

// Note: I/O uses Gx::FileSystem to avoid direct filesystem coupling
#include <Genode/IO/FileSystem/FileSystem.hpp>
#include <Genode/IO/Archive.hpp>

namespace Gx
{
    namespace
    {
        ResourcePtr<std::vector<std::uint8_t>> WrapBytes(std::vector<std::uint8_t> bytes)
        {
            return ResourcePtr<std::vector<std::uint8_t>>(
                new std::vector<std::uint8_t>(std::move(bytes)),
                [](std::vector<std::uint8_t>* p) { delete p; });
        }

        std::string NormalizeKey(const std::string_view nameOrPath)
        {
            std::string key{nameOrPath};
            const std::filesystem::path p{key};
            if (p.has_filename())
                key = p.filename().string();

            std::transform(key.begin(), key.end(), key.begin(), [](const unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
            return key;
        }

        std::optional<std::vector<std::uint8_t>> ReadAllBytes(const std::string& filePath)
        {
            if (!FileSystem::Contains(filePath))
                return std::nullopt;

            const auto info = FileSystem::GetFileInfo(filePath);
            if (!info)
                return std::nullopt;

            const auto sizeOpt = FileSystem::GetFileSize(filePath);
            if (!sizeOpt.has_value())
                return std::nullopt;

            std::vector<std::uint8_t> data;
            data.resize(sizeOpt.value());
            const auto read = FileSystem::ReadFile(filePath, data.data(), data.size());
            if (!read.has_value())
                return std::nullopt;

            data.resize(read.value());
            return data.empty() ? std::nullopt : std::optional{std::move(data)};
        }

        std::optional<std::string> ResolveFontPath(const std::string& nameOrPath)
        {
            if (FileSystem::Contains(nameOrPath))
                return nameOrPath;

            std::vector<std::string> candidateNames;
            if (nameOrPath.find_last_of('.') != std::string::npos)
            {
                candidateNames.push_back(nameOrPath);
            }
            else
            {
                static const char* exts[] = {".ttf", ".otf", ".ttc", ".dfont"};
                for (const char* ext : exts)
                    candidateNames.emplace_back(nameOrPath + std::string(ext));
            }

    #if defined(_WIN32)
            for (const auto& cand : candidateNames)
            {
                if (FileSystem::Contains(cand))
                    return cand;
            }

            HKEY hKey;
            if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
            {
                char valueName[512];
                BYTE data[1024];
                DWORD valueIndex = 0;
                DWORD valueNameSize, dataSize, type;
                while (true)
                {
                    valueNameSize = sizeof(valueName);
                    dataSize = sizeof(data);
                    const auto status = RegEnumValueA(hKey, valueIndex++, valueName, &valueNameSize, nullptr, &type, data, &dataSize);
                    if (status != ERROR_SUCCESS) break;
                    if (type != REG_SZ) continue;
                    std::string entryName(valueName, valueNameSize);
                    std::string fileName(reinterpret_cast<char*>(data));

                    const bool matchesFamily = entryName.find(nameOrPath) != std::string::npos;
                    const bool matchesFile = std::find(candidateNames.begin(), candidateNames.end(), fileName) != candidateNames.end();
                    if (!matchesFamily && !matchesFile)
                        continue;

                    std::string fullPath = fileName;
                    if (fullPath.find(':') == std::string::npos && fullPath.rfind("\\\\", 0) != 0)
                    {
                        char winDir[MAX_PATH] = {0};
                        if (GetWindowsDirectoryA(winDir, MAX_PATH))
                            fullPath = std::string(winDir) + "\\\\Fonts\\\\" + fileName;
                    }

                    if (FileSystem::Contains(fullPath))
                    {
                        RegCloseKey(hKey);
                        return fullPath;
                    }
                }
                RegCloseKey(hKey);
            }
            return std::nullopt;
    #elif defined(__APPLE__)
            CFStringRef cfName = CFStringCreateWithCString(kCFAllocatorDefault, nameOrPath.c_str(), kCFStringEncodingUTF8);
            if (cfName)
            {
                CTFontDescriptorRef desc = CTFontDescriptorCreateWithNameAndSize(cfName, 12.0);
                if (desc)
                {
                    CTFontRef font = CTFontCreateWithFontDescriptor(desc, 12.0, nullptr);
                    if (font)
                    {
                        CFURLRef url = static_cast<CFURLRef>(CTFontCopyAttribute(font, kCTFontURLAttribute));
                        if (url)
                        {
                            char path[PATH_MAX];
                            if (CFURLGetFileSystemRepresentation(url, true, reinterpret_cast<UInt8*>(path), sizeof(path)))
                            {
                                std::string full(path);
                                CFRelease(url);
                                CFRelease(font);
                                CFRelease(desc);
                                CFRelease(cfName);
                                if (FileSystem::Contains(full))
                                    return full;

                                for (const auto& cand : candidateNames)
                                {
                                    if (FileSystem::Contains(cand))
                                        return cand;
                                }
                                return std::nullopt;
                            }
                            CFRelease(url);
                        }
                        CFRelease(font);
                    }
                    CFRelease(desc);
                }
                CFRelease(cfName);
            }
            for (const auto& cand : candidateNames)
            {
                if (FileSystem::Contains(cand))
                    return cand;
            }
            return std::nullopt;
    #else
            #ifdef USE_FONTCONFIG
            if (FcInit())
            {
                FcPattern* pat = FcNameParse(reinterpret_cast<const FcChar8*>(nameOrPath.c_str()));
                if (pat)
                {
                    FcConfigSubstitute(nullptr, pat, FcMatchPattern);
                    FcDefaultSubstitute(pat);
                    FcResult result;
                    FcPattern* font = FcFontMatch(nullptr, pat, &result);
                    if (font)
                    {
                        FcChar8* file = nullptr;
                        if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch && file)
                        {
                            std::string full(reinterpret_cast<const char*>(file));
                            FcPatternDestroy(font);
                            FcPatternDestroy(pat);
                            if (FileSystem::Contains(full))
                                return full;
                        }
                        FcPatternDestroy(font);
                    }
                    FcPatternDestroy(pat);
                }
            }
            #endif
            for (const auto& cand : candidateNames)
            {
                if (FileSystem::Contains(cand))
                    return cand;
            }
            return std::nullopt;
    #endif
        }

        std::optional<std::string> ResolveDefaultFontPath()
        {
    #if defined(_WIN32)
            std::string faceName;
            NONCLIENTMETRICSW ncm{};
            ncm.cbSize = sizeof(NONCLIENTMETRICSW);
            if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0))
            {
                wchar_t const* wname = ncm.lfMessageFont.lfFaceName;
                int len = WideCharToMultiByte(CP_UTF8, 0, wname, -1, nullptr, 0, nullptr, nullptr);
                if (len > 1)
                {
                    faceName.resize(static_cast<size_t>(len - 1));
                    WideCharToMultiByte(CP_UTF8, 0, wname, -1, faceName.data(), len, nullptr, nullptr);
                }
            }
            if (faceName.empty())
            {
                LOGFONTW lf{};
                if (SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0))
                {
                    int len = WideCharToMultiByte(CP_UTF8, 0, lf.lfFaceName, -1, nullptr, 0, nullptr, nullptr);
                    if (len > 1)
                    {
                        faceName.resize(static_cast<size_t>(len - 1));
                        WideCharToMultiByte(CP_UTF8, 0, lf.lfFaceName, -1, faceName.data(), len, nullptr, nullptr);
                    }
                }
            }

            if (!faceName.empty())
            {
                if (auto p = ResolveFontPath(faceName))
                    return p;
            }

            if (auto p = ResolveFontPath("Arial"))
                return p;

            if (auto p = ResolveFontPath("Segoe UI"))
                return p;

            return std::nullopt;
    #elif defined(__APPLE__)
            CTFontRef sysFont = CTFontCreateUIFontForLanguage(kCTFontUIFontSystem, 0.0, nullptr);
            if (sysFont)
            {
                CFURLRef url = static_cast<CFURLRef>(CTFontCopyAttribute(sysFont, kCTFontURLAttribute));
                if (url)
                {
                    char path[PATH_MAX];
                    if (CFURLGetFileSystemRepresentation(url, true, reinterpret_cast<UInt8*>(path), sizeof(path)))
                    {
                        std::string full(path);
                        CFRelease(url);
                        CFRelease(sysFont);
                        if (FileSystem::Contains(full))
                            return full;
                    }
                    CFRelease(url);
                }
                CFRelease(sysFont);
            }

            if (auto p = ResolveFontPath("San Francisco"))
                return p;

            if (auto p = ResolveFontPath("Helvetica Neue"))
                return p;

            if (auto p = ResolveFontPath("Helvetica"))
                return p;

            return std::nullopt;
    #else
            #ifdef USE_FONTCONFIG
            if (FcInit())
            {
                FcPattern* pat = FcPatternBuild(nullptr,
                                                FC_FAMILY, FcTypeString, "sans",
                                                FC_WEIGHT, FcTypeInteger, FC_WEIGHT_NORMAL,
                                                FC_SLANT,  FcTypeInteger, FC_SLANT_ROMAN,
                                                nullptr);
                if (pat)
                {
                    FcConfigSubstitute(nullptr, pat, FcMatchPattern);
                    FcDefaultSubstitute(pat);
                    FcResult result;
                    FcPattern* font = FcFontMatch(nullptr, pat, &result);
                    if (font)
                    {
                        FcChar8* file = nullptr;
                        if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch && file)
                        {
                            std::string full(reinterpret_cast<const char*>(file));
                            FcPatternDestroy(font);
                            FcPatternDestroy(pat);
                            if (FileSystem::Contains(full))
                                return full;
                        }
                        FcPatternDestroy(font);
                    }
                    FcPatternDestroy(pat);
                }
            }
            #endif

            if (auto p = ResolveFontPath("DejaVu Sans"))
                return p;

            if (auto p = ResolveFontPath("Liberation Sans"))
                return p;

            return std::nullopt;
    #endif
        }
    }

    std::unique_ptr<sf::Font> FontManager::Create(const std::string& nameOrPath)
    {
        const auto key = NormalizeKey(nameOrPath);

        {
            std::scoped_lock lock(m_mutex);
            if (const auto* bytes = m_cache.Find(key))
            {
                sf::MemoryInputStream stream(bytes->data(), bytes->size());
                return std::make_unique<sf::Font>(stream);
            }
        }

        const auto path = ResolveFontPath(nameOrPath);
        if (!path)
            return nullptr;

        auto bytes = ReadAllBytes(*path);
        if (!bytes)
            return nullptr;

        std::scoped_lock lock(m_mutex);
        const auto& ref = m_cache.Store(key, WrapBytes(std::move(*bytes)));
        sf::MemoryInputStream stream(ref.data(), ref.size());
        return std::make_unique<sf::Font>(stream);
    }

    std::unique_ptr<sf::Font> FontManager::CreateDefault()
    {
        const auto path = ResolveDefaultFontPath();
        if (!path)
            return nullptr;

        return Create(*path);
    }

    std::optional<std::pair<const void*, std::size_t>> FontManager::GetData(const std::string& key)
    {
        const auto font = Create(key);
        if (!font)
            return std::nullopt;

        const auto norm = NormalizeKey(key);
        std::scoped_lock lock(m_mutex);
        if (const auto* bytes = m_cache.Find(norm))
            return std::pair<const void*, std::size_t>{bytes->data(), bytes->size()};

        return std::nullopt;
    }

    std::optional<std::pair<const void*, std::size_t>> FontManager::GetDefaultData()
    {
        const auto path = ResolveDefaultFontPath();
        if (!path)
            return std::nullopt;

        const std::string key = NormalizeKey(std::filesystem::path(*path).filename().string());

        if (auto cached = GetData(key))
            return cached;

        const auto font = Create(*path);
        if (!font)
            return std::nullopt;

        return GetData(key);
    }

    bool FontManager::Store(const std::string& key, std::vector<std::uint8_t> bytes)
    {
        if (bytes.empty())
            return false;

        const auto norm = NormalizeKey(key);
        std::scoped_lock lock(m_mutex);
        if (m_cache.Find(norm))
            return false;

        m_cache.Store(norm, WrapBytes(std::move(bytes)));
        return true;
    }

    void FontManager::Clear()
    {
        std::scoped_lock lock(m_mutex);
        m_cache.Clear();
    }
}
