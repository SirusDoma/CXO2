#include <CXO2/Services/PluginService.hpp>
#include <CXO2/Contexts/CommandLineContext.hpp>

#include <Genode/System/Exception.hpp>

#include <fmt/format.h>

#include <dlfcn.h>
#include <filesystem>
#include <iostream>

namespace Cx
{
    PluginService::PluginService(CommandLineContext& args, Gx::Context& context, Gx::EventDispatcher& dispatcher) :
        m_args(args),
        m_context{*this, context, dispatcher}
    {
    }

    void PluginService::Load()
    {
        for (const auto& library : m_args.GetLibraries())
        {
            const auto id = std::filesystem::path(library).stem().string();
            if (m_plugins.count(id))
                continue;

            const auto handle = dlopen(library.c_str(), RTLD_NOW | RTLD_LOCAL);
            if (!handle)
            {
                const auto error = dlerror();
                throw Gx::Exception(fmt::format("Failed to load plugin \"{}\": {}", library, error ? error : "unknown error"));
            }

            const auto startUp  = reinterpret_cast<PluginStartUp>(dlsym(handle, "O2CX_StartUp"));
            const auto shutDown = reinterpret_cast<PluginShutDown>(dlsym(handle, "O2CX_ShutDown"));
            if (!startUp || !shutDown)
            {
                const auto error = dlerror();
                throw Gx::Exception(fmt::format("Failed to load plugin \"{}\": {}", library, error ? error : "unknown error"));
            }

            auto& plugin        = m_plugins[id];
            plugin.Handle       = handle;
            plugin.StartUp      = startUp;
            plugin.ShutDown     = shutDown;
            plugin.GetLastError = reinterpret_cast<PluginGetLastError>(dlsym(handle, "O2CX_GetLastError"));

            plugin.Busy = true;

            const auto result = plugin.StartUp(m_context);
            if (result != 0)
            {
                const auto reason = plugin.GetLastError ? plugin.GetLastError() : nullptr;
                std::cerr << fmt::format("Failed to initialize plugin \"{}\" (code {}): {}", id, result, reason ? reason : "no details") << std::endl;

                plugin.ShutDown();
            }

            plugin.Busy = false;
            plugin.Enabled = result == 0;
        }
    }

    void PluginService::ShutDown()
    {
        for (auto& [id, plugin] : m_plugins)
        {
            if (plugin.Enabled)
            {
                plugin.Busy = true;
                plugin.ShutDown();
                plugin.Busy = false;
                plugin.Enabled = false;
            }
        }
    }

    std::vector<std::string> PluginService::GetPlugins() const
    {
        auto plugins = std::vector<std::string>();
        plugins.reserve(m_plugins.size());

        for (const auto& [id, plugin] : m_plugins)
            plugins.push_back(id);

        return plugins;
    }

    bool PluginService::IsEnabled(const std::string& id) const
    {
        const auto it = m_plugins.find(id);
        return it != m_plugins.end() && it->second.Enabled;
    }

    bool PluginService::SetEnabled(const std::string& id, const bool enabled)
    {
        const auto it = m_plugins.find(id);
        if (it == m_plugins.end())
            throw Gx::Exception(fmt::format("Plugin is not loaded: {}", id));

        auto& plugin = it->second;
        if (plugin.Busy)
            return false;

        if (plugin.Enabled == enabled)
            return true;

        plugin.Busy = true;

        if (enabled)
        {
            const auto result = plugin.StartUp(m_context);
            if (result != 0)
            {
                const auto reason = plugin.GetLastError ? plugin.GetLastError() : nullptr;
                std::cerr << fmt::format("Failed to start \"{}\" plugin (code {}): {}", id, result, reason ? reason : "no details") << std::endl;

                plugin.ShutDown();
                plugin.Busy = false;

                return false;
            }
        }
        else
            plugin.ShutDown();

        plugin.Busy = false;
        plugin.Enabled = enabled;

        return true;
    }
}
