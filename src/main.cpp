#include <stdio.h>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <Genode.hpp>

#include <O2/IO/Archives/M30Archive.hpp>
#include <O2/IO/Archives/OmcArchive.hpp>

#include <O2/IO/Loaders/SpriteMetadataLoader.hpp>
#include <O2/IO/Loaders/MusicMetadataLoader.hpp>
#include <O2/IO/Loaders/AnimationMetadataLoader.hpp>

#include <O2/IO/Loaders/UI/ImageMetadataLoader.hpp>
#include <O2/IO/Loaders/UI/LabelMetadataLoader.hpp>
#include <O2/IO/Loaders/UI/NumberMetadataLoader.hpp>
#include <O2/IO/Loaders/UI/ButtonMetadataLoader.hpp>
#include <O2/IO/Loaders/UI/RadioButtonMetadataLoader.hpp>
#include <O2/IO/Loaders/UI/ProgressBarMetadataLoader.hpp>
#include <O2/IO/Loaders/UI/RepeaterMetadataLoader.hpp>
#include <O2/IO/Loaders/UI/DialogMetadataLoader.hpp>

#include <O2/States/StateAvi.hpp>

int main() 
{
    // Mount Asset Paths
    Gx::FileHelper::AddAssetPath("./Assets");
    Gx::FileHelper::AddAssetPath("./Image");
    Gx::FileHelper::AddAssetPath("./Music");

    // -- Register resource metadata loaders
    // Basic Resource
    Gx::ResourceLoaderFactory::Register<Gx::Sprite>(new SpriteMetadataLoader());
    Gx::ResourceLoaderFactory::Register<sf::Music>(new MusicMetadataLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Animation>(new AnimationMetadataLoader());
    // UI
    Gx::ResourceLoaderFactory::Register<Gx::Image>(new ImageMetadataLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Label>(new LabelMetadataLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Number>(new NumberMetadataLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Button>(new ButtonMetadataLoader());
    Gx::ResourceLoaderFactory::Register<Gx::RadioButton>(new RadioButtonMetadataLoader());
    Gx::ResourceLoaderFactory::Register<Gx::ProgressBar>(new ProgressBarMetadataLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Repeater>(new RepeaterMetadataLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Dialog>(new DialogMetadataLoader());

    // Load globally used assets
    Gx::ResourceManager::Instance()->AddArchive<OmcArchive>("Music/BGM.ojm");
    Gx::ResourceManager::Instance()->AddArchive<OmcArchive>("Music/bgEffect.ojm");
    Gx::ResourceManager::Instance()->AddArchive<OmcArchive>("Music/Planet.ojm");

    sf::Texture hcursor;
    hcursor.loadFromFile(Gx::FileHelper::GetFullName("Common/CursorSingle.png"));
    Gx::Cursor cursor = Gx::Cursor(hcursor);

    Gx::Application application(new StateAvi(), sf::VideoMode(800, 600), sf::VideoMode(800, 600));

    application.SetCursor(cursor);
    int exitCode = application.Start();

    return exitCode;
}

