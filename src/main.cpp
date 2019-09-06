#include <stdio.h>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

#include <Genode.hpp>

#include <Nx/Scenes/StateLogo.hpp>
#include <Nx/IO/OpiArchive.hpp>

int main() 
{
    sf::Texture hcursor;
    hcursor.loadFromFile("D:/O2Jam/Assets/Global/CursorSingle.png");
    auto image = hcursor.copyToImage();

    Gx::Cursor cursor = Gx::Cursor(hcursor);
    Gx::Application application(new StateLogo(), sf::VideoMode(1024, 768), sf::VideoMode(800, 600));

    application.SetCursor(cursor);
    int exitCode = application.Start();

    return exitCode;
}

