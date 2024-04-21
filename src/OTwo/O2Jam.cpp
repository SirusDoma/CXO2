#include <OTwo/O2Jam.hpp>
#include <Genode.hpp>

#include <OTwo/Archives/OjmArchive.hpp>
#include <OTwo/Archives/M30Archive.hpp>
#include <OTwo/Archives/OmcArchive.hpp>

#include <OTwo/IO/Loaders/MetadataLoader.hpp>

#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/ShapeLoader.hpp>
#include <OTwo/IO/Loaders/Audio/SoundLoader.hpp>
#include <OTwo/IO/Loaders/Audio/MusicLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/AnimationLoader.hpp>

#include <OTwo/IO/Loaders/UI/ImageLoader.hpp>
#include <OTwo/IO/Loaders/UI/LabelLoader.hpp>
#include <OTwo/IO/Loaders/UI/ToolTipLoader.hpp>
#include <OTwo/IO/Loaders/UI/NumberLoader.hpp>
#include <OTwo/IO/Loaders/UI/ButtonLoader.hpp>
#include <OTwo/IO/Loaders/UI/CheckBoxLoader.hpp>
#include <OTwo/IO/Loaders/UI/RadioButtonLoader.hpp>
#include <OTwo/IO/Loaders/UI/GaugeLoader.hpp>
#include <OTwo/IO/Loaders/UI/ListLoader.hpp>
#include <OTwo/IO/Loaders/UI/DialogLoader.hpp>
#include <OTwo/IO/Loaders/UI/TextBoxLoader.hpp>
#include <OTwo/IO/Loaders/UI/ScrollBarLoader.hpp>
#include <OTwo/IO/Loaders/UI/UiContainerLoader.hpp>

#include <OTwo/IO/Loaders/Avatar/ItemLoader.hpp>
#include <OTwo/IO/Loaders/Avatar/ItemDataLoader.hpp>
#include <OTwo/IO/Loaders/Avatar/AvatarLoader.hpp>

#include <OTwo/IO/Loaders/UI/Components/Common/MarqueeLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Common/ChatPanelLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Common/ChatWindowLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Planet/ChannelButtonLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Planet/ChannelBoardLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Room/RoomContainerLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Room/RoomButtonLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Room/UserListLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Waiting/AvatarInfoLoader.hpp>

#include <OTwo/IO/Loaders/Chart/ChartMetadataLoader.hpp>
#include <OTwo/IO/Loaders/Chart/ChartLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/StateLoader.hpp>

#include <OTwo/Data/Character.hpp>
#include <OTwo/Data/UserState.hpp>

#include <OTwo/States/SceneDirectorDecorator.hpp>
#include <OTwo/States/StateTest.hpp>
#include <OTwo/States/StateAvi.hpp>
#include <OTwo/States/StatePlanet.hpp>
#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StateWaiting7K.hpp>
#include <OTwo/States/StateLoading.hpp>

#include <OTwo/Config/GameConfig.hpp>

void O2Jam::Boot()
{
    Gx::Application::Boot();

    // Asset Path
    Gx::LocalFileSystem::AddAssetPath("./assets");
    Gx::LocalFileSystem::AddAssetPath("./assets/Music");
    Gx::LocalFileSystem::AddAssetPath("./Image");
    Gx::LocalFileSystem::AddAssetPath("./Music");

    // -- Register resource metadata loaders
    // Basic Resource
    Gx::ResourceLoaderFactory::Register<ResourceMetadata, MetadataLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Sprite, SpriteLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Shape, ShapeLoader>();
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
    Gx::ResourceLoaderFactory::Register<Gx::Gauge, GaugeLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::List, ListLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Dialog, DialogLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::TextBox, TextBoxLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::ScrollBar, ScrollBarLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::UiContainer, UiContainerLoader>();
    // Avatar
    Gx::ResourceLoaderFactory::Register<Item, ItemLoader>();
    Gx::ResourceLoaderFactory::Register<ItemData, ItemDataLoader>();
    Gx::ResourceLoaderFactory::Register<Avatar, AvatarLoader>();
    // O2Jam Components
    Gx::ResourceLoaderFactory::Register<Marquee, MarqueeLoader>();
    Gx::ResourceLoaderFactory::Register<ChatPanel, ChatPanelLoader>();
    Gx::ResourceLoaderFactory::Register<ChatWindow, ChatWindowLoader>();
    Gx::ResourceLoaderFactory::Register<ChannelButton, ChannelButtonLoader>();
    Gx::ResourceLoaderFactory::Register<ChannelBoard, ChannelBoardLoader>();
    Gx::ResourceLoaderFactory::Register<RoomContainer, RoomContainerLoader>();
    Gx::ResourceLoaderFactory::Register<RoomButton, RoomButtonLoader>();
    Gx::ResourceLoaderFactory::Register<UserList, UserListLoader>();
    Gx::ResourceLoaderFactory::Register<AvatarInfo, AvatarInfoLoader>();
    // O2Jam Core Resources
    Gx::ResourceLoaderFactory::Register<ChartMetadata, ChartMetadataLoader>();
    Gx::ResourceLoaderFactory::Register<Chart, ChartLoader>();
    // SceneGraph
    Gx::ResourceLoaderFactory::Register<State, StateLoader>();


    auto config = GameConfig();

    // Render settings
    auto& window = GetRenderWindow();
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(0);

    // Setup configuration
    SetConfig<GameConfig>([] (auto &app)
    {
        // TODO: Load game config from file
        auto config = std::make_unique<GameConfig>();
        return config;
    });

    // Module configuration
    Provide<Gx::ResourceManager>([](auto &app)
    {
        // Register shared resource container
        auto resources = std::make_unique<Gx::ResourceManager>();
        resources->Register<Item>();
        resources->Register<ItemData>();

        return resources;
    });

    Provide<Gx::Mixer>([](auto &app)
    {
        auto mixer = std::make_unique<Gx::Mixer>();
        return mixer;
    });

    Provide<ItemFactory>([&](auto &app)
    {
        auto factory = std::make_unique<ItemFactory>(Require<Gx::ResourceManager>());
        return factory;
    });

    Provide<UserState>([&](auto &app)
    {
        auto state = std::make_unique<UserState>();
        auto player   = PlayerData();
        player.ID     = 1;
        player.Name   = "CXO2";
        player.Level  = -1;
        player.Gender = Gender::Male;

        state->SetCurrentPlayer(player);
        return state;
    });

    // Force to load heavy providers during start-up
    Require<UserState>().GetInstalledMusic();
    for (auto gender : {Gender::Male, Gender::Female})
        Require<ItemFactory>().GetDefaultItems(gender);

    // Load global assets
    auto& resources = Require<Gx::ResourceManager>();
    auto& bgm       = resources.Create<OjmArchive>("BGM");
    auto& bgEvent   = resources.Create<OjmArchive>("Event");
    auto& bgEffect  = resources.Create<OjmArchive>("BgEffect");
    auto& bgPlanet  = resources.Create<OjmArchive>("BgPlanet");

    if (bgm.LoadFromFile("BGM.ojm"))
        Gx::FileSystem::Mount(bgm);

    if (bgEvent.LoadFromFile("Event.ojm"))
        Gx::FileSystem::Mount(bgEvent);

    if (bgEffect.LoadFromFile("bgEffect.ojm"))
        Gx::FileSystem::Mount(bgEffect);

    if (bgPlanet.LoadFromFile("Planet.ojm"))
        Gx::FileSystem::Mount(bgPlanet);

    auto director = SceneDirectorDecorator::Decorate(GetSceneDirector());
    director.Register<StateTest>("Interface/State/Test.json");
    director.Register<StateAvi>("Interface/State/Avi.json");
    director.Register<StatePlanet>("Interface/State/Planet.json");
    director.Register<StateRoom>("Interface/State/Room.json");
    director.Register<StateWaiting7K>("Interface/State/Waiting.json");
    director.Register<StateLoading>("Interface/State/Loading.json");

    director.Present<StateTest>();
}

void O2Jam::Shutdown()
{
    Application::Shutdown();

    auto& resources = Require<Gx::ResourceManager>();
    auto& mixer    = Require<Gx::Mixer>();
    auto& director = GetSceneDirector();

    mixer.Clear();
    director.Unload();
    resources.Clear();
}

void O2Jam::OnFocusChanged(bool focus)
{
    Application::OnFocusChanged(focus);

    auto& director = GetSceneDirector();
    auto config    = GetConfig<GameConfig>();
    auto& mixer    = Require<Gx::Mixer>();

    if (focus)
    {
        mixer.SetVolume(static_cast<float>(config.MusicVolume));
        if (director.IsPresenting<StateAvi>())
            return;

        mixer.Stop("SFX");
        mixer.Play("BGM");
    }
    else
    {
        mixer.SetVolume(0.f);
        if (director.IsPresenting<StateAvi>())
            return;

        mixer.PauseAll();
    }
}
