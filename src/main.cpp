#include <stdio.h>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <Genode.hpp>

#include <O2/IO/Archives/M30Archive.hpp>
#include <O2/IO/Archives/OmcArchive.hpp>

#include <O2/IO/Loaders/SpriteLoader.hpp>
#include <O2/IO/Loaders/SoundLoader.hpp>
#include <O2/IO/Loaders/MusicLoader.hpp>
#include <O2/IO/Loaders/AnimationLoader.hpp>

#include <O2/IO/Loaders/UI/ImageLoader.hpp>
#include <O2/IO/Loaders/UI/LabelLoader.hpp>
#include <O2/IO/Loaders/UI/NumberLoader.hpp>
#include <O2/IO/Loaders/UI/ButtonLoader.hpp>
#include <O2/IO/Loaders/UI/RadioButtonLoader.hpp>
#include <O2/IO/Loaders/UI/ProgressBarLoader.hpp>
#include <O2/IO/Loaders/UI/RepeaterLoader.hpp>
#include <O2/IO/Loaders/UI/DialogLoader.hpp>

#include <O2/States/StateAvi.hpp>

int main() 
{
    // Mount Asset Paths
    Gx::FileHelper::AddAssetPath("./Assets");
    Gx::FileHelper::AddAssetPath("./Image");
    Gx::FileHelper::AddAssetPath("./Music");

    // -- Register resource metadata loaders
    // Basic Resource
    Gx::ResourceLoaderFactory::Register<Gx::Sprite>(new SpriteLoader());
    Gx::ResourceLoaderFactory::Register<sf::Sound>(new SoundLoader());
    Gx::ResourceLoaderFactory::Register<sf::Music>(new MusicLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Animation>(new AnimationLoader());
    // UI
    Gx::ResourceLoaderFactory::Register<Gx::Image>(new ImageLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Label>(new LabelLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Number>(new NumberLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Button>(new ButtonLoader());
    Gx::ResourceLoaderFactory::Register<Gx::RadioButton>(new RadioButtonLoader());
    Gx::ResourceLoaderFactory::Register<Gx::ProgressBar>(new ProgressBarLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Repeater>(new RepeaterLoader());
    Gx::ResourceLoaderFactory::Register<Gx::Dialog>(new DialogLoader());

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

