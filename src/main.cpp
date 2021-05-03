#include <stdio.h>
#include <iostream>

#include <O2/O2Jam.hpp>
#include <O2/States/StateAvi.hpp>

int main()
{
    O2Jam o2jam(sf::VideoMode(800, 600), sf::VideoMode(800, 600));
    o2jam.Start(new StateAvi());

}

