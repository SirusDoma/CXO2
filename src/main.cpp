#include <stdio.h>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <Genode.hpp>

#include <O2/IO/Archives/M30Archive.hpp>
#include <O2/IO/Archives/OmcArchive.hpp>

#include <O2/IO/Metadata/SpriteMetadataLoader.hpp>
#include <O2/IO/Metadata/MusicMetadataLoader.hpp>
#include <O2/IO/Metadata/AnimationMetadataLoader.hpp>
#include <O2/IO/Metadata/LabelMetadataLoader.hpp>
#include <O2/IO/Metadata/ButtonMetadataLoader.hpp>
#include <O2/IO/Metadata/RadioButtonMetadataLoader.hpp>
#include <O2/IO/Metadata/DialogMetadataLoader.hpp>

#include <O2/States/StateAvi.hpp>
#include <O2/States/StatePlanet.hpp>

int main() 
{
    // Mount Asset Paths
    Gx::FileHelper::AddAssetPath("./Assets");
    Gx::FileHelper::AddAssetPath("./Image");
    Gx::FileHelper::AddAssetPath("./Music");

    // Register resource metadata loaders
    Gx::ResourceLoaderFactory::Register<Gx::Sprite>(new SpriteMetadataLoader());
    Gx::ResourceLoaderFactory::Register<sf::Music>(new MusicMetadataLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Animation>(new AnimationMetadataLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Label>(new LabelMetadataLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Button>(new ButtonMetadataLoader());
    Gx::ResourceLoaderFactory::Register<Gx::RadioButton>(new RadioButtonMetadataLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Dialog>(new DialogMetadataLoader());

    // Load globally used assets
    Gx::ResourceManager::Instance()->AddArchive<OmcArchive>("Music/BGM.ojm");
    Gx::ResourceManager::Instance()->AddArchive<OmcArchive>("Music/bgEffect.ojm");
    Gx::ResourceManager::Instance()->AddArchive<OmcArchive>("Music/Planet.ojm");

    sf::Texture hcursor;
    hcursor.loadFromFile(Gx::FileHelper::GetFullName("Common/CursorSingle.png"));
    Gx::Cursor cursor = Gx::Cursor(hcursor);

    Gx::Application application(new StateAvi(), sf::VideoMode(1024, 768), sf::VideoMode(800, 600));

    application.SetCursor(cursor);
    int exitCode = application.Start();

    return exitCode;
}

