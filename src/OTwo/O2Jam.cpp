#include <OTwo/O2Jam.hpp>
#include <Genode.hpp>

#include <OTwo/Archives/M30Archive.hpp>
#include <OTwo/Archives/OmcArchive.hpp>

#include <OTwo/Loaders/SpriteLoader.hpp>
#include <OTwo/Loaders/SoundLoader.hpp>
#include <OTwo/Loaders/MusicLoader.hpp>
#include <OTwo/Loaders/AnimationLoader.hpp>

#include <OTwo/Loaders/UI/ImageLoader.hpp>
#include <OTwo/Loaders/UI/LabelLoader.hpp>
#include <OTwo/Loaders/UI/ToolTipLoader.hpp>
#include <OTwo/Loaders/UI/NumberLoader.hpp>
#include <OTwo/Loaders/UI/ButtonLoader.hpp>
#include <OTwo/Loaders/UI/CheckBoxLoader.hpp>
#include <OTwo/Loaders/UI/RadioButtonLoader.hpp>
#include <OTwo/Loaders/UI/ProgressBarLoader.hpp>
#include <OTwo/Loaders/UI/ListLoader.hpp>
#include <OTwo/Loaders/UI/DialogLoader.hpp>
#include <OTwo/Loaders/UI/TextBoxLoader.hpp>
#include <OTwo/Loaders/UI/ScrollBarLoader.hpp>

#include <OTwo/Loaders/UI/Components/ChannelButtonLoader.hpp>
#include <OTwo/Loaders/UI/Components/ChannelBoardLoader.hpp>
#include <OTwo/Loaders/UI/Components/ChatWindowLoader.hpp>
#include <OTwo/Loaders/Dialogs/OptionDialogLoader.hpp>
#include <OTwo/Loaders/Dialogs/CreateRoomDialogLoader.hpp>
#include <OTwo/Loaders/UI/Components/MarqueeLoader.hpp>

#include <OTwo/Loaders/Character/ItemLoader.hpp>
#include <OTwo/Loaders/Character/ItemDataLoader.hpp>
#include <OTwo/Loaders/Character/AvatarLoader.hpp>

#include <OTwo/States/Components/Planet/ChannelButton.hpp>
#include <OTwo/States/Components/Planet/ChannelBoard.hpp>

#include <OTwo/Config/GameConfig.hpp>

void O2Jam::OnStart()
{
    auto config = GameConfig();

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
    Gx::ResourceLoaderFactory::Register<Gx::ToolTip, ToolTipLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Number, NumberLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Button, ButtonLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::CheckBox, CheckBoxLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::RadioButton, RadioButtonLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::ProgressBar, ProgressBarLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::List, ListLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Dialog, DialogLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::TextBox, TextBoxLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::ScrollBar, ScrollBarLoader>();
    // O2 Components
    Gx::ResourceLoaderFactory::Register<ChannelButton, ChannelButtonLoader>();
    Gx::ResourceLoaderFactory::Register<ChannelBoard, ChannelBoardLoader>();
    Gx::ResourceLoaderFactory::Register<ChatWindow, ChatWindowLoader>();
    Gx::ResourceLoaderFactory::Register<OptionDialog, OptionDialogLoader>();
    Gx::ResourceLoaderFactory::Register<CreateRoomDialog, CreateRoomDialogLoader>();
    Gx::ResourceLoaderFactory::Register<Marquee, MarqueeLoader>();
    // Character
    Gx::ResourceLoaderFactory::Register<Item, ItemLoader>();
    Gx::ResourceLoaderFactory::Register<ItemData, ItemDataLoader>();
    Gx::ResourceLoaderFactory::Register<Avatar, AvatarLoader>();

    // Setup configuration
    SetConfig<GameConfig>([] (auto &app)
    {
        // TODO: Load game config from file
        auto config = std::make_unique<GameConfig>();
        return config;
    });

    // Module configuration
    Provide<Gx::ResourceManager>(
        [=](auto &app) -> Gx::ResourceManager & {
            // Register shared resource container
            m_resources.Register<Item>();
            m_resources.Register<ItemData>();

            return m_resources;
        }
    );
    Provide<Gx::Mixer>(
        [=](auto &app) -> Gx::Mixer & {
            m_mixer = Gx::Mixer(Require<Gx::ResourceManager>());
            return m_mixer;
        }
    );
    Provide<ItemFactory>(
        [=](auto &app) -> ItemFactory & {
            m_itemFactory = ItemFactory(Require<Gx::ResourceManager>());
            return m_itemFactory;
        }
    );

    /** Uncomment to load all items at startup
     *
     auto itemData = static_cast<ItemData*>(m_resources.LoadMetadata<ItemData>("Avatar/Itemdata.json"));
     for (auto item : itemData->Items)
         m_resources.Load<Item>("Avatar/Items/" + std::to_string(item.first) + ".json", item.second);
    */

    // Force to load item metadata at startup
    for (auto gender : {Character::Gender::Male, Character::Gender::Female})
        Require<ItemFactory>().GetDefaultItems(gender);

    // Load global assets
    m_resources.LoadArchive<OmcArchive>("Music/BGM.ojm");
    m_resources.LoadArchive<OmcArchive>("Music/bgEffect.ojm");
    m_resources.LoadArchive<OmcArchive>("Music/Planet.ojm");

    // Load application modules here
    ShareResources(m_resources);
}

void O2Jam::OnFocusChanged(bool focus)
{
    Application::OnFocusChanged(focus);

    auto config = GetConfig<GameConfig>();
    if (focus)
    {
        m_mixer.SetVolume(config.MusicVolume);

        m_mixer.Stop("SFX");
        m_mixer.Play("BGM");
    }
    else
    {
        m_mixer.SetVolume(0.f);
        m_mixer.PauseAll();
    }
}
