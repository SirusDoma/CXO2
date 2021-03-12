#include <stdio.h>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <Genode.hpp>

#include <Nx/IO/Archives/M30Archive.hpp>
#include <Nx/IO/Archives/OmcArchive.hpp>

#include <Nx/IO/Definitions/SpriteDefinitionLoader.hpp>
#include <Nx/IO/Definitions/MusicDefinitionLoader.hpp>

#include <Nx/Scenes/StateAvi.hpp>

int main() 
{
    // Mount Asset Paths
    Gx::FileHelper::AddAssetPath("./Assets");
    Gx::FileHelper::AddAssetPath("./Image");
    Gx::FileHelper::AddAssetPath("./Music");

    // Register resource definition loaders
    Gx::ResourceLoaderFactory::Register<Gx::Sprite>(new SpriteDefinitionLoader());
    Gx::ResourceLoaderFactory::Register<sf::Music>(new MusicDefinitionLoader());

    // Load globally used assets
    Gx::ResourceManager::Instance()->AddArchive<OmcArchive>("Music/BGM.ojm");

    sf::Texture hcursor;
    hcursor.loadFromFile(Gx::FileHelper::GetFullName("Common/CursorSingle.png"));

    Gx::Cursor cursor = Gx::Cursor(hcursor);
    Gx::Application application(new StateAvi(), sf::VideoMode(1024, 768), sf::VideoMode(800, 600));

    application.SetCursor(cursor);
    int exitCode = application.Start();

    return exitCode;
}

