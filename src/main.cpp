#include <OTwo/O2Jam.hpp>
#include <OTwo/Utilities/NamedMutex.hpp>
#include <OTwo/Utilities/SystemMessageBox.hpp>

#include <Genode/IO/FileSystem/LocalFileSystem.hpp>
#include <Genode/System/Exception.hpp>
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

            // "Fix" macOS translocation
            if (const auto path = Gx::LocalFileSystem::GetApplicationDirectoryPath(); !path.empty())
                Gx::LocalFileSystem::SetWorkingDirectory(path);
        }
        else
        {
            // Enable Live++ when debugger attached (optional)
            Gx::Debugger::Watch();
        }

        auto o2jam = O2Jam("O2-JAM", sf::VideoMode({800, 600}), sf::View({400, 300}, {800, 600}), true);
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
            details += "\n\nWorking Directory:\n";
            details += Gx::LocalFileSystem::GetWorkingDirectory() + "\n";

            details += "\n\nAsset paths:\n";
            for (auto path : Gx::LocalFileSystem::GetAssetPaths())
                details += path + "\n";
        }

        auto message = std::string(std::string(ex.what()) + details);
        if (Gx::Debugger::IsDebuggerAttached())
            message = "[" + Gx::StringHelper::GetTypeName(ex) + "]\n" + message;

        SystemMessageBox::ShowError(message, "Fatal Error");
        return -1;
    }
}
