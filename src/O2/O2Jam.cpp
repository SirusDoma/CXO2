#include <O2/O2Jam.hpp>
#include <Genode.hpp>

#include <O2/Archives/M30Archive.hpp>
#include <O2/Archives/OmcArchive.hpp>

#include <O2/Loaders/SpriteLoader.hpp>
#include <O2/Loaders/SoundLoader.hpp>
#include <O2/Loaders/MusicLoader.hpp>
#include <O2/Loaders/AnimationLoader.hpp>

#include <O2/Loaders/UI/ImageLoader.hpp>
#include <O2/Loaders/UI/LabelLoader.hpp>
#include <O2/Loaders/UI/NumberLoader.hpp>
#include <O2/Loaders/UI/ButtonLoader.hpp>
#include <O2/Loaders/UI/RadioButtonLoader.hpp>
#include <O2/Loaders/UI/ProgressBarLoader.hpp>
#include <O2/Loaders/UI/ListLoader.hpp>
#include <O2/Loaders/UI/DialogLoader.hpp>
#include <O2/Loaders/UI/TextBoxLoader.hpp>
#include <O2/Loaders/UI/ScrollBarLoader.hpp>

void O2Jam::OnStart()
{
    // Render settings
    auto& window = GetRenderWindow();
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(0);

    // Asset Path
    Gx::FileHelper::AddAssetPath("./Assets");
    Gx::FileHelper::AddAssetPath("./Image");
    Gx::FileHelper::AddAssetPath("./Music");

    // -- Register resource metadata loaders
    // Basic Resource
    Gx::ResourceLoaderFactory::Register<Gx::Sprite, SpriteLoader>();
    Gx::ResourceLoaderFactory::Register<sf::Sound, SoundLoader>();
    Gx::ResourceLoaderFactory::Register<sf::Music, MusicLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Animation, AnimationLoader>();
    // UI
    Gx::ResourceLoaderFactory::Register<Gx::Image, ImageLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Label, LabelLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Number, NumberLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Button, ButtonLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::RadioButton, RadioButtonLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::ProgressBar, ProgressBarLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::List, ListLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Dialog, DialogLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::TextBox, TextBoxLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::ScrollBar, ScrollBarLoader>();

    // Load global assets
    m_resources.LoadArchive<OmcArchive>("Music/BGM.ojm");
    m_resources.LoadArchive<OmcArchive>("Music/bgEffect.ojm");
    m_resources.LoadArchive<OmcArchive>("Music/Planet.ojm");

    // Load application modules here
    ShareResources(m_resources);
    UseMixer(m_mixer);
}
