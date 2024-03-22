#include <OTwo/O2Jam.hpp>

int main(int argc , char** argv)
{
    // Fix weird working directory in macOS
    //Gx::LocalFileSystem::SetWorkingDirectory(std::string(argv[0]));

    auto o2jam = O2Jam(sf::VideoMode(800, 600), sf::VideoMode(800, 600));
    return o2jam.Start();
}
