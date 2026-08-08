#pragma once
#include <CXO2/Services/Service.hpp>

#include <Genode/System/Context.hpp>
#include <Genode/Events/EventDispatcher.hpp>

#include <map>
#include <string>
#include <vector>

namespace Cx
{
    class CommandLineContext;
    class PluginService;

    struct AppPluginContext
    {
        PluginService& Plugins;
        Gx::Context& Context;
        Gx::EventDispatcher& Events;
    };

    class PluginService : public virtual Service
    {
    public:
        PluginService(CommandLineContext& args, Gx::Context& context, Gx::EventDispatcher& dispatcher);

        void Load();
        void ShutDown();

        std::vector<std::string> GetPlugins() const;
        bool IsEnabled(const std::string& id) const;
        bool SetEnabled(const std::string& id, bool enabled);

    private:
        using PluginStartUp      = int(*)(const AppPluginContext&);
        using PluginShutDown     = void(*)();
        using PluginGetLastError = const char*(*)();

        struct Plugin
        {
            void*              Handle       = nullptr;
            PluginStartUp      StartUp      = nullptr;
            PluginShutDown     ShutDown     = nullptr;
            PluginGetLastError GetLastError = nullptr;
            bool               Enabled      = false;
            bool               Busy         = false;
        };

        CommandLineContext& m_args;
        AppPluginContext m_context;

        std::map<std::string, Plugin> m_plugins{};
    };
}
