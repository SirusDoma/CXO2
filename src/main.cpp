#include <OTwo/O2Jam.hpp>

#include <Genode/IO/FileSystem/LocalFileSystem.hpp>
#include <Genode/System/Exception.hpp>
#include <Genode/Utilities/Debugger.hpp>

#include <boxer/boxer.h>

int main(int argc , char** argv)
{
    try
    {
        if (!Gx::Debugger::IsDebuggerAttached())
        {
            // "Fix" macOS translocation
            if (auto path = Gx::LocalFileSystem::GetApplicationDirectoryPath(); !path.empty())
                Gx::LocalFileSystem::SetWorkingDirectory(path);
        }

        auto o2jam = O2Jam("O2-JAM", sf::VideoMode(800, 600), sf::VideoMode(800, 600));
        return o2jam.Start();
    }
    catch (std::exception &ex)
    {
        if (Gx::Debugger::IsDebuggerAttached())
            throw ex;

        auto details = std::string();
        if (typeid(ex) == typeid(Gx::ResourceAccessException) || typeid(ex) == typeid(Gx::ResourceLoadException))
        {
            details += "\n\nWorking Directory:\n";
            details += Gx::LocalFileSystem::GetWorkingDirectory();

            details += "\n\nMounted Paths:\n";
            for (auto &path: Gx::LocalFileSystem::GetAssetPaths())
                details += "- " + path + "\n\n";
        }

        boxer::show(std::string(std::string(ex.what()) + details).c_str(), "Fatal Error", boxer::Style::Error);
        throw ex;
    }
}
