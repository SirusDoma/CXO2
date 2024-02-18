#include <stdio.h>
#include <iostream>

#include <OTwo/O2Jam.hpp>
#include <OTwo/States/StateAvi.hpp>

int main()
{
    O2Jam o2jam(sf::VideoMode(800, 600), sf::VideoMode(800, 600));
    o2jam.Start(new StateAvi());

}

