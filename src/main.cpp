#include <CXO2/O2.hpp>
#include <CXO2/Contexts/CommandLineContext.hpp>
#include <CXO2/Utilities/NamedMutex.hpp>
#include <CXO2/Utilities/SystemMessageBox.hpp>

#include <Genode/System/Platform.hpp>
#include <Genode/System/Exception.hpp>
#include <Genode/IO/LocalFileSystem.hpp>
#include <Genode/IO/IOException.hpp>
#include <Genode/Utilities/Debugger.hpp>
#include <Genode/Utilities/StringHelper.hpp>

#include <iostream>
#include <exception>
#include <typeinfo>
#include <stdexcept>

int main(int argc , char** argv)
{
    try
    {
        const auto mutex = Cx::NamedMutex("OnceMutex");
        if (!Gx::Debugger::IsDebuggerAttached())
        {
            // Check for multiple O2Jam instances running (including the original client)
            // Disabled when debugger is attached for developer & modder convenient
            if (!mutex.IsLocked())
            {
                Cx::SystemMessageBox::ShowInformation("O2JAM is already running.", "[INFO]");
                return 1;
            }

            // MacOS: This could be happening if the game launched in a certain way
            if (Gx::GetCurrentPlatform() == Gx::Platform::macOS)
            {
                if (argc >= 1 && Gx::LocalFileSystem::Instance().Contains(argv[0]))
                    Gx::LocalFileSystem::SetWorkingDirectory(std::string(argv[0]));
            }
        }

        // The command line must either be fully valid (online mode) or absent (offline mode)
        const auto ctx = Cx::CommandLineContext(argc, argv);
        if (ctx.GetArgumentCount() > 0 &&
            (ctx.GetArgumentCount() < 6 ||
             ctx.GetAuthToken().empty() ||
             ctx.GetFtpUrl().empty() ||
             ctx.GetGame().empty() ||
             ctx.GetGatewayInfo().empty()))
        {
            Cx::SystemMessageBox::ShowInformation("Please try to run O2jam.exe", "[INFO]");
            return 0;
        }

        sf::ContextSettings settings{};
        settings.presentation = sf::ContextSettings::Presentation::Auto;

        auto renderer = sf::Renderer::OpenGL;
        if (sf::isRendererAvailable(sf::Renderer::Direct3D11))
        {
            renderer = sf::Renderer::Direct3D11;
            settings.presentation = sf::ContextSettings::Presentation::LowLatency;
        }
        else if (sf::isRendererAvailable(sf::Renderer::Metal))
        {
            renderer = sf::Renderer::Metal;
            settings.presentation = sf::ContextSettings::Presentation::LowLatency;
        }

        sf::setRenderer(renderer);
        if (sf::getRenderer() != renderer)
        {
            Cx::SystemMessageBox::ShowError("Failed to initialize the graphic renderer.", "Fatal Error");
            return -1;
        }

        auto cxo2 = Cx::O2("O2-JAM", sf::VideoMode({800, 600}), sf::View({400, 300}, {800, 600}), true, settings);
        cxo2.GetModule<Gx::Context>().Provide<Cx::CommandLineContext>([ctx] (const auto&)
        {
            return std::make_unique<Cx::CommandLineContext>(ctx);
        }, Gx::Context::Scope::Singleton);

        return cxo2.Start();
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
            if (const auto accessEx = dynamic_cast<Gx::ResourceAccessException*>(&ex); accessEx && !accessEx->GetResourceID().empty())
                details += "\n(" + accessEx->GetResourceID() + ")";
            else if (const auto loadEx = dynamic_cast<Gx::ResourceLoadException*>(&ex); loadEx && !loadEx->GetResourceID().empty())
                details += "\n(" + loadEx->GetResourceID() + ")";
        }

        auto message = std::string(std::string(ex.what()) + details);
        if (Gx::Debugger::IsDebuggerAttached())
            message = "[" + Gx::StringHelper::GetTypeName(ex) + "]\n" + message;

        Cx::SystemMessageBox::ShowError(message, "Fatal Error");
        return -1;
    }
}
