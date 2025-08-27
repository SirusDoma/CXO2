#include <OTwo/O2Jam.hpp>

#include <Genode.hpp>
#include <OTwo/Utilities/Reflection.hpp>

#include <OTwo/Archives/OpiArchive.hpp>
#include <OTwo/Archives/OjmArchive.hpp>
#include <OTwo/Archives/EmbeddedArchive.hpp>

#include <OTwo/IO/Loaders/MetadataLoader.hpp>

#include <OTwo/IO/Loaders/Graphics/TextureLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/FontLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/ShapeLoader.hpp>
#include <OTwo/IO/Loaders/Audio/SoundBufferLoader.hpp>
#include <OTwo/IO/Loaders/Audio/SoundLoader.hpp>
#include <OTwo/IO/Loaders/Audio/MusicLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/AnimationLoader.hpp>

#include <OTwo/IO/Loaders/Adaptor/ControlListLoader.hpp>
#include <OTwo/IO/Loaders/Adaptor/O2JamSpriteLoader.hpp>

#include <OTwo/IO/Loaders/UI/CursorLoader.hpp>
#include <OTwo/IO/Loaders/UI/ImageLoader.hpp>
#include <OTwo/IO/Loaders/UI/LabelLoader.hpp>
#include <OTwo/IO/Loaders/UI/ToolTipLoader.hpp>
#include <OTwo/IO/Loaders/UI/BitmapNumberLoader.hpp>
#include <OTwo/IO/Loaders/UI/ButtonLoader.hpp>
#include <OTwo/IO/Loaders/UI/ToggleButtonLoader.hpp>
#include <OTwo/IO/Loaders/UI/RadioButtonLoader.hpp>
#include <OTwo/IO/Loaders/UI/GaugeLoader.hpp>
#include <OTwo/IO/Loaders/UI/ListLoader.hpp>
#include <OTwo/IO/Loaders/UI/DialogLoader.hpp>
#include <OTwo/IO/Loaders/UI/InputFieldLoader.hpp>
#include <OTwo/IO/Loaders/UI/ScrollBarLoader.hpp>
#include <OTwo/IO/Loaders/UI/UiContainerLoader.hpp>

#include <OTwo/IO/Loaders/Avatar/ItemLoader.hpp>
#include <OTwo/IO/Loaders/Avatar/ItemDataLoader.hpp>
#include <OTwo/IO/Loaders/Avatar/SetInfoDataLoader.hpp>
#include <OTwo/IO/Loaders/Avatar/AvatarLoader.hpp>

#include <OTwo/IO/Loaders/UI/Components/Common/MarqueeLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Common/ChatWindowLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Planet/ChannelButtonLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Planet/ChannelBoardLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Waiting/AvatarInfoLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Playing/EqualizerLoader.hpp>

#include <OTwo/IO/Loaders/Chart/O2JamChartMetadataLoader.hpp>
#include <OTwo/IO/Loaders/Chart/O2JamChartLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/StateLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/StatePlaying7KLoader.hpp>

#include <OTwo/IO/TextureCacheBuilder.hpp>
#include <OTwo/Metadata/Legacy/ControlList.hpp>

#include <OTwo/UI/Common/ChatPanel.hpp>
#include <OTwo/UI/Dialogs/CreateRoomDialog.hpp>
#include <OTwo/UI/Dialogs/OptionDialog.hpp>
#include <OTwo/UI/Dialogs/SelectMusicDialog.hpp>
#include <OTwo/UI/Room/RoomButton.hpp>
#include <OTwo/UI/Room/RoomList.hpp>
#include <OTwo/UI/Room/UserList.hpp>
#include <OTwo/UI/Waiting/InstrumentSelector.hpp>
#include <OTwo/UI/Waiting/MapSelector.hpp>
#include <OTwo/UI/Playing/PlayMenu.hpp>
#include <OTwo/UI/Playing/Equalizer.hpp>

#include <OTwo/Decorators/SceneGraph/SceneDirectorDecorator.hpp>

#include <OTwo/Network/NetworkAdapter.hpp>
#include <OTwo/Services/NetworkService.hpp>
#include <OTwo/Services/AuthService.hpp>
#include <OTwo/Services/PlanetService.hpp>
#include <OTwo/Services/CharacterService.hpp>
#include <OTwo/Services/MessagingService.hpp>
#include <OTwo/Services/RoomService.hpp>
#include <OTwo/Services/ItemShopService.hpp>
#include <OTwo/Services/WaitingService.hpp>
#include <OTwo/Services/PlayingService.hpp>

#include <OTwo/Contexts/CommandLineContext.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/CartContext.hpp>

#include <OTwo/Avatar/ItemFactory.hpp>
#include <OTwo/Core/JudgementStrategy.hpp>
#include <OTwo/Core/Judgements/RenderPositionJudgementStrategy.hpp>
#include <OTwo/Core/Judgements/TimeJudgementStrategy.hpp>
#include <OTwo/Core/ScoreTracker.hpp>

#include <OTwo/States/StateAvi.hpp>
#include <OTwo/States/StatePlanet.hpp>
#include <OTwo/States/StateRoom.hpp>
#include <OTwo/States/StateMusicShop.hpp>
#include <OTwo/States/StateItemShop.hpp>
#include <OTwo/States/StateMyRoom.hpp>
#include <OTwo/States/StateBulletin.hpp>
#include <OTwo/States/StatePayment.hpp>
#include <OTwo/States/StateWaiting7K.hpp>
#include <OTwo/States/StateLoading.hpp>
#include <OTwo/States/StateResult.hpp>

#include <OTwo/Config/GameConfig.hpp>
#include <OTwo/Utilities/Console.hpp>
#include <OTwo/Resources.hpp>

O2Jam::O2Jam(std::string title, const sf::VideoMode& mode, const sf::View& view, const bool fullScreen, const sf::ContextSettings& settings) :
    Gx::Application(std::move(title), mode, view, fullScreen, settings)
{
}

bool O2Jam::InInteropMode()
{
    return InInteropMode(
        InteropMode::Interface |
        InteropMode::Playing |
        InteropMode::Avatar
    );
}

bool O2Jam::InInteropMode(const InteropMode modes)
{
    static bool image   = Gx::FileSystem::Contains("ControlList_Interface.txt");
    static bool playing = Gx::FileSystem::Contains("ControlList_Playing.txt");
    static bool avatar  = Gx::FileSystem::Scan("Itemdata*.dat").size() > 0;

    if (modes & InteropMode::Interface && !image)
        return false;

    if (modes & InteropMode::Playing && !playing)
        return false;

    if (modes & InteropMode::Avatar && !avatar)
        return false;

    return true;
}

void O2Jam::Boot()
{
    // Render Settings
    const auto& window = GetMainWindow();
    if (GetWindowState() == sf::State::Fullscreen)
        Gx::Application::SetView(GetLetterBoxView(window.getView(), window.getSize()));

    // Initialize singleton providers
    auto& context = GetContext();
    context.Provide<NetworkAdapter>([](auto&)
    {
        auto adapter = std::make_unique<NetworkAdapter>();
        adapter->UsePrefixSizeType<std::uint16_t>();

        return adapter;
    }, Gx::Context::Scope::Shared);

    context.Provide<GameConfig>([](auto&)
    {
        auto config = std::make_unique<GameConfig>();
        config->Load();

        return config;
    }, Gx::Context::Scope::Shared);

    context.Provide<Gx::FontManager>([] (auto&)
    {
        return std::make_unique<Gx::FontManager>();
    }, Gx::Context::Scope::Shared);

    context.Provide<Gx::ResourceManager>([](auto&)
    {
        // Register shared resource container
        auto resources = std::make_unique<Gx::ResourceManager>();
        resources->Register<Item>();
        resources->Register<ItemData>();

        return resources;
    }, Gx::Context::Scope::Shared);

    context.Provide<Gx::AudioMixer>([](auto& ctx)
    {
        auto mixer = std::make_unique<Gx::AudioMixer>();
        auto& cfg  = ctx.template Require<GameConfig>();

        mixer->GetSoundGroup(Sound::Channel::BGM).SetVolume(cfg.MusicVolume);
        mixer->GetSoundGroup(Sound::Channel::SFX).SetVolume(cfg.EffectVolume);

        return mixer;
    }, Gx::Context::Scope::Shared);

    context.Provide<SessionContext>([&](auto& ctx)
    {
        // auto charInfo            = CharacterInfo();
        // charInfo.Name            = "Player";
        // charInfo.Level           = -1;
        // charInfo.Experience      = 11200;
        // charInfo.RankStats.Rank  = 7;
        // charInfo.RankStats.Wins  = 100;
        // charInfo.RankStats.Draws = 10;
        // charInfo.RankStats.Loses = 5;
        // charInfo.Gender          = Gender::Male;
        // charInfo.Wallet.Cash     = 15000;

        const auto& cmd   = ctx.template Require<CommandLineContext>();
        std::string token = cmd.GetAuthToken();

        auto session = std::make_unique<SessionContext>(token);
        // session->SetCharacterInfo(charInfo);
        // session->Load();

        return session;
    }, Gx::Context::Scope::Shared);

    context.Provide<CartContext>([&] (auto&)
    {
        return std::make_unique<CartContext>();
    }, Gx::Context::Scope::Shared);

    context.Provide<ScoreTracker>([] (auto&)
    {
        return std::make_unique<ScoreTracker>();
    }, Gx::Context::Scope::Shared);

    // Initialize local providers
    context.Provide<JudgementStrategy>([] (auto&)
    {
        return std::make_unique<RenderPositionJudgementStrategy>();
    });

    // Register services
    context.Provide<AuthService, AuthOnlineService>();
    context.Provide<PlanetService, PlanetOnlineService>();
    context.Provide<CharacterService, CharacterOnlineService>();
    context.Provide<MessagingService, MessagingOnlineService>();
    context.Provide<ItemShopService, ItemShopOnlineService>();
    context.Provide<RoomService, RoomOnlineService>();
    context.Provide<WaitingService, WaitingOnlineService>();
    context.Provide<PlayingService, PlayingOnlineService>();
    context.Provide<NetworkService, OnlineNetworkService>();

    // Asset Path
    Gx::LocalFileSystem::AddAssetPath("./assets");
    Gx::LocalFileSystem::AddAssetPath("./assets/Music");
    Gx::LocalFileSystem::AddAssetPath("./Image");
    Gx::LocalFileSystem::AddAssetPath("./Music");

    // -- Register resource metadata loaders
    // Core Resources
    Gx::ResourceLoaderFactory::Register<ResourceMetadata, MetadataLoader>();
    Gx::ResourceLoaderFactory::Register<ControlList, ControlListLoader>();
    Gx::ResourceLoaderFactory::Register<SpriteSheet, O2JamSpriteLoader>();
    Gx::ResourceLoaderFactory::Register<sf::Texture, TextureLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Font, FontLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Cursor, CursorLoader>();
    Gx::ResourceLoaderFactory::Register<sf::SoundBuffer, SoundBufferLoader>();
    Gx::ResourceLoaderFactory::Register<sf::Sound, SoundLoader>();
    Gx::ResourceLoaderFactory::Register<sf::Music, MusicLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::Shape, ShapeLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::Sprite, SpriteLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::Animation, AnimationLoader>();
    // UI
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::Image, ImageLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::Label, LabelLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::ToolTip, ToolTipLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::BitmapNumber, BitmapNumberLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::Button, ButtonLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::ToggleButton, ToggleButtonLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::RadioButton, RadioButtonLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::Gauge, GaugeLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::List, ListLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::Dialog, DialogLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::InputField, InputFieldLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::ScrollBar, ScrollBarLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Gx::UiContainer, UiContainerLoader>();
    // Avatar
    Gx::ResourceLoaderFactory::Register<Item, ItemLoader>();
    Gx::ResourceLoaderFactory::Register<ItemData, ItemDataLoader>();
    Gx::ResourceLoaderFactory::Register<SetInfoData, SetInfoDataLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Avatar, AvatarLoader>();
    // O2Jam Exclusive Components
    Gx::ResourceLoaderFactory::Register<Gx::Node, Marquee, MarqueeLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, ChatWindow, ChatWindowLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, ChannelButton, ChannelButtonLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, ChannelBoard, ChannelBoardLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, AvatarInfo, AvatarInfoLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Node, Equalizer, EqualizerLoader>();
    // O2Jam Derived Components
    Gx::ResourceLoaderFactory::Reuse<Gx::Node, Gx::UiContainer, ChatPanel>();
    Gx::ResourceLoaderFactory::Reuse<Gx::Node, Gx::UiContainer, UserList>();
    Gx::ResourceLoaderFactory::Reuse<Gx::Node, Gx::UiContainer, MapSelector>();
    Gx::ResourceLoaderFactory::Reuse<Gx::Node, Gx::UiContainer, InstrumentSelector>();
    Gx::ResourceLoaderFactory::Reuse<Gx::Node, Gx::List, RoomList>();
    Gx::ResourceLoaderFactory::Reuse<Gx::Node, Gx::Image, RoomButton>();
    Gx::ResourceLoaderFactory::Reuse<Gx::Node, Gx::Image, PlayMenu>();
    // O2Jam Dialogs
    Gx::ResourceLoaderFactory::Reuse<Gx::Node, Gx::Dialog, OptionDialog>();
    Gx::ResourceLoaderFactory::Reuse<Gx::Node, Gx::Dialog, CreateRoomDialog>();
    Gx::ResourceLoaderFactory::Reuse<Gx::Node, Gx::Dialog, SelectMusicDialog>();
    // O2Jam Core Resources
    Gx::ResourceLoaderFactory::Register<O2JamChartMetadata, O2JamChartMetadataLoader>();
    Gx::ResourceLoaderFactory::Register<Chart, O2JamChartLoader>();
    // SceneGraph
    Gx::ResourceLoaderFactory::Register<State, StateLoader>();
    Gx::ResourceLoaderFactory::Reuse<State, StateAvi>();
    Gx::ResourceLoaderFactory::Reuse<State, StatePlanet>();
    Gx::ResourceLoaderFactory::Reuse<State, StateRoom>();
    Gx::ResourceLoaderFactory::Reuse<State, StateMusicShop>();
    Gx::ResourceLoaderFactory::Reuse<State, StateItemShop>();
    Gx::ResourceLoaderFactory::Reuse<State, StateMyRoom>();
    Gx::ResourceLoaderFactory::Reuse<State, StateBulletin>();
    Gx::ResourceLoaderFactory::Reuse<State, StatePayment>();
    Gx::ResourceLoaderFactory::Reuse<State, StateWaiting7K>();
    Gx::ResourceLoaderFactory::Reuse<State, StateLoading>();
    Gx::ResourceLoaderFactory::Reuse<State, StateResult>();
    Gx::ResourceLoaderFactory::Register<StatePlaying7K, StatePlaying7KLoader>();

    // Load global interface assets
    auto& resources = context.Require<Gx::ResourceManager>();
    auto& image     = resources.Create<OpiArchive>("Interface");
    auto& playing   = resources.Create<OpiArchive>("Playing");
    auto& avatar    = resources.Create<OpiArchive>("Avatar");
    auto& embedded  = resources.Create<EmbeddedArchive>("Internal");

    // Reroute font to embedded resource
    auto& fontManager = context.Require<Gx::FontManager>();
    if (auto unicode = fontManager.GetData("Arial Unicode MS"))
        embedded.WriteFile("Interface/Common/Font.ttf", unicode->first, unicode->second);
    else
    {
        if (auto arial = fontManager.GetData("Arial"))
            embedded.WriteFile("Interface/Common/Font.ttf", arial->first, arial->second);
        else if (auto defaultData = fontManager.GetDefaultData())
            embedded.WriteFile("Interface/Common/Font.ttf", defaultData->first, defaultData->second);

        // Korea
        if (auto ff1_1 = fontManager.GetData("Malgun Gothic"))
            embedded.WriteFile("Interface/Common/Fallback-Font1.ttf", ff1_1->first, ff1_1->second);
        else if (auto ff1_2 = fontManager.GetData("Apple SD Gothic Neo"))
            embedded.WriteFile("Interface/Common/Fallback-Font1.ttf", ff1_2->first, ff1_2->second);
        else if (auto ff1_3 = fontManager.GetData("Baekmuk Dotum"))
            embedded.WriteFile("Interface/Common/Fallback-Font1.ttf", ff1_3->first, ff1_3->second);

        // Chinese
        if (auto ff2_1 = fontManager.GetData("Microsoft YaHei UI"))
            embedded.WriteFile("Interface/Common/Fallback-Font2.ttf", ff2_1->first, ff2_1->second);
        else if (auto ff2_2 = fontManager.GetData("PingFang SC"))
            embedded.WriteFile("Interface/Common/Fallback-Font2.ttf", ff2_2->first, ff2_2->second);
        else if (auto ff2_3 = fontManager.GetData("AR PL UMing CN"))
            embedded.WriteFile("Interface/Common/Fallback-Font2.ttf", ff2_3->first, ff2_3->second);

        // Japan
        if (auto ff3_1 = fontManager.GetData("Yu Gothic UI"))
            embedded.WriteFile("Interface/Common/Fallback-Font3.ttf", ff3_1->first, ff3_1->second);
        else if (auto ff3_2 = fontManager.GetData("Hiragino Sans"))
            embedded.WriteFile("Interface/Common/Fallback-Font3.ttf", ff3_2->first, ff3_2->second);
        else if (auto ff3_3 = fontManager.GetData("Kochi Gothic"))
            embedded.WriteFile("Interface/Common/Fallback-Font3.ttf", ff3_3->first, ff3_3->second);
    }

    // Embedded resources
    for (const auto& [name, resource] : OTwo::Resources)
        embedded.WriteFile(std::string(name), resource.data, resource.size);

    Gx::FileSystem::Mount(embedded);

    for (std::string name : { "Interface.opi", "Interface1.opi" })
    {
        if (image.LoadFromFile(name))
        {
            Gx::FileSystem::Mount(image);
            break;
        }
    }

    for (std::string name : { "Playing.opi", "Playing1.opi" })
    {
        if (playing.LoadFromFile(name))
        {
            Gx::FileSystem::Mount(playing);
            break;
        }
    }

    if (avatar.LoadFromFile("avatar.opa"))
        Gx::FileSystem::Mount(avatar);

    // Load global music assets
    auto& bgm       = resources.Create<OjmArchive>("BGM");
    auto& bgEvent   = resources.Create<OjmArchive>("Event");
    auto& bgEffect  = resources.Create<OjmArchive>("BgEffect");
    auto& bgPlanet  = resources.Create<OjmArchive>("BgPlanet");
    auto& npc       = resources.Create<OjmArchive>("O2PlanetNPC");

    if (Gx::FileSystem::Contains("BGM.ojm") && bgm.LoadFromFile("BGM.ojm"))
        Gx::FileSystem::Mount(bgm);

    if (Gx::FileSystem::Contains("Event.ojm") && bgEvent.LoadFromFile("Event.ojm"))
        Gx::FileSystem::Mount(bgEvent);

    if (Gx::FileSystem::Contains("bgEffect.ojm") && bgEffect.LoadFromFile("bgEffect.ojm"))
        Gx::FileSystem::Mount(bgEffect);

    if (Gx::FileSystem::Contains("Planet.ojm") && bgPlanet.LoadFromFile("Planet.ojm"))
        Gx::FileSystem::Mount(bgPlanet);

    if (Gx::FileSystem::Contains("O2PlanetNPC.ojm") && npc.LoadFromFile("O2PlanetNPC.ojm"))
        Gx::FileSystem::Mount(npc);

    // Cache item textures
    if (InInteropMode(InteropMode::Avatar))
    {
        auto cache = TextureCacheBuilder(image, resources);
        cache.BuildCache();
    }

    // Scan for item data
    std::string itemDataFileName = []
    {
        if (const auto files = Gx::FileSystem::Scan("itemdata*.dat"); !files.empty())
            return files.front()->GetName();

        return std::string("Avatar/ItemData.json");
    }();
    std::string setInfoDataFileName = []
    {
        if (const auto files = Gx::FileSystem::Scan("setinfodata.*"); !files.empty())
            return files.front()->GetName();

        if (Gx::FileSystem::Contains("Avatar/SetInfoData.json"))
            return std::string("Avatar/SetInfoData.json");

        return std::string();
    }();

    // Force to load item providers during start-up
    context.Provide<ItemFactory>([&, itemDataFileName, setInfoDataFileName](auto& ctx)
    {
        auto factory = std::make_unique<ItemFactory>(ctx.template Require<Gx::ResourceManager>(), itemDataFileName, setInfoDataFileName);
        return factory;
    }, Gx::Context::Scope::Shared);

    auto _ = context.Require<SessionContext>().GetInstalledMusic();
    for (auto gender : {Gender::Male, Gender::Female})
        auto __ = context.Require<ItemFactory>().GetDefaultItems(gender);

    // Set-up console
    if (Gx::FileSystem::Contains("Interface/Common/Font.Monospace.ttf"))
    {
        Console::Instance().SetFont(context.Require<Gx::ResourceManager>().AddFromFile<Gx::Font>("Interface/Common/Font.Monospace.ttf"));
        Console::Instance().SetCharacterSize(14);
        Console::Instance().SetBounds({{0, 0}, {400, 165}});
        Console::Instance().SetPosition({400, 0});
        Console::Instance().SetMaximumLines(10);
    }

    auto director = SceneDirectorDecorator::Decorate(GetSceneDirector());
    if (InInteropMode(InteropMode::Interface))
    {
        // Cache textures
        auto cache = TextureCacheBuilder(image, resources);
        cache.BuildCache();

        // Load and set cursor
        SetCursor(context.Require<Gx::ResourceManager>().AddFromFile<Gx::Cursor>("ControlList/Window_Cursor.json"));

        director.Register<StateAvi>("ControlList/State/Avi.json");
        director.Register<StatePlanet>("ControlList/State/Planet.json");
        director.Register<StateRoom>("ControlList/State/Room.json");
        director.Register<StateWaiting7K>("ControlList/State/Waiting7K.json");
        director.Register<StateMyRoom>("ControlList/State/MyRoom.json");
        director.Register<StateItemShop>("ControlList/State/ItemShop.json");
        director.Register<StateMusicShop>("ControlList/State/MusicShop.json");
    }
    else
    {
        // Load and set cursor
        SetCursor(context.Require<Gx::ResourceManager>().AddFromFile<Gx::Cursor>("Interface/Common/Window_Cursor.json"));

        director.Register<StateAvi>("Interface/State/Avi.json");
        director.Register<StatePlanet>("Interface/State/Planet.json");
        director.Register<StateRoom>("Interface/State/Room.json");
        director.Register<StateWaiting7K>("Interface/State/Waiting7K.json");
        director.Register<StateMyRoom>("Interface/State/MyRoom.json");
        director.Register<StateItemShop>("Interface/State/ItemShop.json");
        director.Register<StateMusicShop>("Interface/State/MusicShop.json");
    }

    if (Gx::FileSystem::Contains("ControlList_Playing.txt"))
    {
        auto cache = TextureCacheBuilder(playing, resources);
        cache.BuildCache();

        director.Register<StateLoading>("ControlList/State/Loading.json");
        director.Register<StatePlaying7K>("ControlList/State/Playing7K.json");
    }
    else
    {
        director.Register<StateLoading>("Interface/State/Loading.json");
        director.Register<StatePlaying7K>("Playing/State/Playing7K.json");
    }

    if (InInteropMode(InteropMode::Interface) && InInteropMode(InteropMode::Playing))
    {
        director.Register<StateResult>("ControlList/State/Result.json");
    }
    else
    {
        director.Register<StateResult>("Interface/State/Result.json");
    }

    director.Register<StateBulletin>("Interface/State/Bulletin.json");
    director.Register<StatePayment>("Interface/State/Payment.json");

    director.Present<StateAvi>();
}

void O2Jam::OnWindowCreated(sf::RenderWindow& window)
{
    Application::OnWindowCreated(window);

    const auto& context = GetContext();
    const auto& config  = context.Require<GameConfig>();

    auto stream     = sf::MemoryInputStream(app_icon.data(), app_icon.size());
    const auto icon = sf::Image(stream);

    window.setVerticalSyncEnabled(config.UseVsync);
    window.setFramerateLimit(0);
    window.setIcon(icon);
}

void O2Jam::OnFocusChanged(const bool focus)
{
    Application::OnFocusChanged(focus);

    const auto& context = GetContext();
    const auto& config  = context.Require<GameConfig>();
    auto& mixer         = context.Require<Gx::AudioMixer>();

    const bool ignored = GetSceneDirector().IsPresenting<StateAvi>()       ||
                         GetSceneDirector().IsPresenting<StatePlaying7K>() ||
                         GetSceneDirector().IsPresenting<StateResult>();

    auto& bgm = mixer.GetSoundGroup(Sound::Channel::BGM);
    auto& sfx = mixer.GetSoundGroup(Sound::Channel::SFX);
    if (focus)
    {
        bgm.SetVolume(static_cast<float>(config.MusicVolume));
        sfx.SetVolume(static_cast<float>(config.EffectVolume));

        if (ignored)
            return;

        sfx.Resume();
        bgm.Play();
    }
    else
    {
        bgm.SetVolume(0.f);
        sfx.SetVolume(0.f);

        if (ignored)
            return;

        mixer.PauseAll();
    }
}

void O2Jam::OnInputReceived(sf::Event& ev)
{
    Application::OnInputReceived(ev);

    if (const auto key = ev.getIf<sf::Event::KeyReleased>(); key && key->code == sf::Keyboard::Key::F12)
        Console::Instance().SetEnabled(!Console::Instance().IsEnabled());
}

int O2Jam::Shutdown()
{
    auto& director  = GetSceneDirector();
    auto& mixer     = GetContext().Require<Gx::AudioMixer>();
    auto& resources = GetContext().Require<Gx::ResourceManager>();

    director.Reset();
    mixer.Reset(false);
    resources.Clear();

    return 0;
}

void O2Jam::Update(const double delta)
{
    Application::Update(delta);

    if (Gx::Debugger::IsDebuggerAttached())
        GetMainWindow().setTitle(fmt::format("{} [FPS: {}]", GetTitle(), GetRenderFrequency()));

    if ((isKeyPressed(sf::Keyboard::Key::LAlt) || isKeyPressed(sf::Keyboard::Key::RAlt)) && isKeyPressed(sf::Keyboard::Key::Enter) && !m_windowStateSwitched)
    {
        m_windowStateSwitched = true;
        SetWindowState(GetWindowState() == sf::State::Fullscreen ? sf::State::Windowed : sf::State::Fullscreen);
        if (GetWindowState() == sf::State::Fullscreen)
            Gx::Application::SetView(GetLetterBoxView(Gx::Application::GetView(), GetMainWindow().getSize()));
    }
    else if (m_windowStateSwitched && !isKeyPressed(sf::Keyboard::Key::Enter))
        m_windowStateSwitched = false;

    if ((isKeyPressed(sf::Keyboard::Key::LAlt) || isKeyPressed(sf::Keyboard::Key::RAlt)) && isKeyPressed(sf::Keyboard::Key::Up) && !m_letterboxSwitched)
    {
        m_letterboxSwitched = true;
        if (GetWindowState() == sf::State::Fullscreen)
        {
            if (Gx::Application::GetView().getViewport() ==  sf::FloatRect({0.f, 0.f}, {1.f, 1.f}))
                Gx::Application::SetView(GetLetterBoxView(Gx::Application::GetView(), GetMainWindow().getSize()));
            else
                Gx::Application::SetView(GetDefaultView());
        }
    }
    else if (m_letterboxSwitched && !isKeyPressed(sf::Keyboard::Key::Up))
        m_letterboxSwitched = false;
}

Gx::RenderStates O2Jam::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
{
    if (GetWindowState() == sf::State::Fullscreen)
    {
        if (m_layeredTarget->getSize() != GetMainWindow().getSize())
            SetupLayeredTarget();

        m_layeredTarget->clear(GetClearColor());
        {
            Application::Render(m_layeredAdaptor, states);
            m_layeredAdaptor.Render(Console::Instance(), states);
        }
        m_layeredTarget->display();

        const auto buffer = Gx::Sprite(m_layeredTarget->getTexture());
        surface.Render(buffer, Gx::RenderStates::Default);
    }
    else
    {
        Application::Render(surface, states);
        surface.Render(Console::Instance(), states);
    }

    return states;
}

void O2Jam::SetupLayeredTarget() const
{
    m_layeredTarget = std::make_unique<sf::RenderTexture>(GetMainWindow().getSize(), GetSettings());
    m_layeredTarget->setSmooth(true);
    m_layeredAdaptor = Gx::RenderSurfaceAdaptor(*m_layeredTarget);
}

sf::View O2Jam::GetLetterBoxView(sf::View view, const sf::Vector2u& windowSize)
{
    const float windowRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
    const float viewRatio = view.getSize().x / static_cast<float>(view.getSize().y);
    float sizeX = 1;
    float sizeY = 1;
    float posX = 0;
    float posY = 0;

    bool horizontalSpacing = true;
    if (windowRatio < viewRatio)
        horizontalSpacing = false;

    if (horizontalSpacing) {
        sizeX = viewRatio / windowRatio;
        posX = (1 - sizeX) / 2.f;
    }

    else {
        sizeY = windowRatio / viewRatio;
        posY = (1 - sizeY) / 2.f;
    }

    view.setViewport(sf::FloatRect({posX, posY}, {sizeX, sizeY}));
    return view;
}

O2Jam::operator sf::RenderTarget&() const
{
    if (GetWindowState() == sf::State::Fullscreen)
        return *m_layeredTarget;

    return Application::operator sf::RenderTarget&();
}
