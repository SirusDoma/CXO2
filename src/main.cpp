#include <OTwo/O2Jam.hpp>
#include <OTwo/Contexts/CommandLineContext.hpp>
#include <OTwo/Utilities/NamedMutex.hpp>
#include <OTwo/Utilities/SystemMessageBox.hpp>

#include <Genode/IO/FileSystem/LocalFileSystem.hpp>
#include <Genode/System/Exception.hpp>
#include <Genode/IO/IOException.hpp>
#include <Genode/Utilities/Debugger.hpp>
#include <Genode/Utilities/StringHelper.hpp>
#include <Genode/System/Platform.hpp>

#include <iostream>
#include <exception>
#include <typeinfo>
#include <stdexcept>

int main(int argc , char** argv)
{
    try
    {
        const auto mutex = NamedMutex("OnceMutex");
        if (!Gx::Debugger::IsDebuggerAttached())
        {
            // Check for multiple O2Jam instances running (including the original client)
            // Disabled when debugger is attached for developer & modder convenient
            if (!mutex.IsLocked())
            {
                SystemMessageBox::ShowInformation("O2JAM is already running.", "[INFO]");
                return 1;
            }

            // TODO: No longer working in recent MacOS. It probably not worth it to chase this
            // if (const auto path = Gx::LocalFileSystem::GetApplicationDirectoryPath(); !path.empty())
            //     Gx::LocalFileSystem::SetWorkingDirectory(path);

            // MacOS: This could be happening if the game launched in a certain way
            if (Gx::GetCurrentPlatform() == Gx::Platform::macOS)
            {
                if (argc >= 1)
                    Gx::LocalFileSystem::SetWorkingDirectory(std::string(argv[0]));
            }
        }

        // TODO: Remove validation for offline client
        const auto ctx = CommandLineContext(argc, argv);
        if (ctx.GetArgumentCount() < 6 ||
            ctx.GetAuthToken().empty() ||
            ctx.GetFtpUrl().empty() ||
            ctx.GetGame().empty() ||
            ctx.GetGatewayInfo().empty())
        {
            SystemMessageBox::ShowInformation("Please try to run O2jam.exe", "[INFO]");
            return 0;
        }

        auto o2jam = O2Jam("O2-JAM", sf::VideoMode({800, 600}), sf::View({400, 300}, {800, 600}), true);
        o2jam.GetContext().Provide<CommandLineContext>([ctx] (const auto&)
        {
            return std::make_unique<CommandLineContext>(ctx);
        }, Gx::Context::Scope::Singleton);

        return o2jam.Start();
    }
    catch (std::exception &ex)
    {
        if (Gx::Debugger::IsDebuggerAttached())
        {
            std::cerr << ex.what() << std::endl;
            throw;
        }

        auto details = std::string();
        if (typeid(ex) == typeid(Gx::ResourceAccessException) || typeid(ex) == typeid(Gx::ResourceLoadException))
        {
            if (const auto accessEx = dynamic_cast<Gx::ResourceAccessException*>(&ex))
                details += "\n(" + accessEx->GetResourceID() + ")";
            else if (const auto loadEx = dynamic_cast<Gx::ResourceLoadException*>(&ex))
                details += "\n(" + loadEx->GetResourceID() + ")";
        }

        auto message = std::string(std::string(ex.what()) + details);
        if (Gx::Debugger::IsDebuggerAttached())
            message = "[" + Gx::StringHelper::GetTypeName(ex) + "]\n" + message;

        SystemMessageBox::ShowError(message, "Fatal Error");
        return -1;
    }
}
