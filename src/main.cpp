#include <OTwo/O2Jam.hpp>

#include <boxer/boxer.h>

int main(int argc , char** argv)
{
    try
    {
        // Fix weird working directory in macOS
        Gx::LocalFileSystem::SetWorkingDirectory(std::string(argv[0]));

        auto o2jam = O2Jam("O2-JAM", sf::VideoMode(800, 600), sf::VideoMode(800, 600));
        return o2jam.Start();
    }
    catch (std::exception &ex)
    {
        boxer::show(ex.what(), "Fatal Error", boxer::Style::Error);
        throw ex;
    }
}
