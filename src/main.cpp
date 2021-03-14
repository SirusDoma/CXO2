#include <stdio.h>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <Genode.hpp>

#include <O2/IO/Archives/M30Archive.hpp>
#include <O2/IO/Archives/OmcArchive.hpp>

#include <O2/IO/Metadata/SpriteMetadataLoader.hpp>
#include <O2/IO/Metadata/AnimationMetadataLoader.hpp>
#include <O2/IO/Metadata/ButtonMetadataLoader.hpp>
#include <O2/IO/Metadata/MusicMetadataLoader.hpp>

#include <O2/Scenes/StateAvi.hpp>

int main() 
{
    // Mount Asset Paths
    Gx::FileHelper::AddAssetPath("./Assets");
    Gx::FileHelper::AddAssetPath("./Image");
    Gx::FileHelper::AddAssetPath("./Music");

    // Register resource definition loaders
    Gx::ResourceLoaderFactory::Register<Gx::Sprite>(new SpriteMetadataLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Animation>(new AnimationMetadataLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Button>(new ButtonMetadataLoader());
    Gx::ResourceLoaderFactory::Register<sf::Music>(new MusicMetadataLoader());

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

