#include <O2/O2Jam.hpp>
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
    Gx::ResourceLoaderFactory::Register<Gx::Repeater, RepeaterLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Dialog, DialogLoader>();

    // Load global assets
    m_resources.LoadArchive<OmcArchive>("Music/BGM.ojm");
    m_resources.LoadArchive<OmcArchive>("Music/bgEffect.ojm");
    m_resources.LoadArchive<OmcArchive>("Music/Planet.ojm");

    // Load application modules here
    ShareResources(m_resources);
    UseMixer(m_mixer);
}
