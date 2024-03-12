#include <OTwo/O2Jam.hpp>

int main()
{
    auto o2jam = O2Jam(sf::VideoMode(800, 600), sf::VideoMode(800, 600));
    return o2jam.Start();
}
