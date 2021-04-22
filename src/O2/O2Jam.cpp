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
#include <O2/Loaders/UI/CheckBoxLoader.hpp>
#include <O2/Loaders/UI/RadioButtonLoader.hpp>
#include <O2/Loaders/UI/ProgressBarLoader.hpp>
#include <O2/Loaders/UI/ListLoader.hpp>
#include <O2/Loaders/UI/DialogLoader.hpp>
#include <O2/Loaders/UI/TextBoxLoader.hpp>
#include <O2/Loaders/UI/ScrollBarLoader.hpp>

#include <O2/Loaders/UI/Components/ChatWindowLoader.hpp>
#include <O2/Loaders/UI/Components/OptionDialogLoader.hpp>
#include <O2/Loaders/UI/Components/CreateRoomDialogLoader.hpp>
#include <O2/Loaders/UI/Components/MarqueeLoader.hpp>

#include <O2/Loaders/Character/ItemLoader.hpp>
#include <O2/Loaders/Character/ItemDataLoader.hpp>
#include <O2/Loaders/Character/AvatarLoader.hpp>

#include <O2/Config/GameConfig.hpp>

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
    Resolve<Gx::ResourceManager>([=] (auto& app) -> Gx::ResourceManager& { return m_resources; });
    Resolve<Gx::Mixer>([=] (auto& app) -> Gx::Mixer&
    {
        m_mixer = Gx::Mixer(Require<Gx::ResourceManager>());
        return m_mixer;
    });
    Resolve<ItemFactory>([=] (auto& app) -> ItemFactory&
    {
        m_itemFactory = ItemFactory(Require<Gx::ResourceManager>());
        return m_itemFactory;
    });

    // Register shared resource container
    m_resources.Register<Item>();
    m_resources.Register<ItemData>();

    // Force to load item metadata at startup
    for (auto gender : {Character::Gender::Male, Character::Gender::Female})
        Require<ItemFactory>().GetDefaultItems(gender);

    // Load global assets
    m_resources.LoadArchive<OmcArchive>("Music/BGM.ojm");
    m_resources.LoadArchive<OmcArchive>("Music/bgEffect.ojm");
    m_resources.LoadArchive<OmcArchive>("Music/Planet.ojm");

    /** Uncomment to load all items at startup
     *
    auto itemData = static_cast<ItemData*>(m_resources.LoadMetadata<ItemData>("Avatar/Itemdata.json"));
    for (auto item : itemData->Items)
        m_resources.Load<Item>("Avatar/Items/" + std::to_string(item.first) + ".json", item.second);
    */

    // Load application modules here
    ShareResources(m_resources);
}

void O2Jam::OnFocusChanged(bool focus)
{
    Application::OnFocusChanged(focus);

    if (focus)
    {
        m_mixer.Stop("SFX");
        m_mixer.Play("BGM");
    }
    else
        m_mixer.PauseAll();
}
