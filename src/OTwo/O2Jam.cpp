#include <OTwo/O2Jam.hpp>
#include <Genode.hpp>

#include <OTwo/Archives/OjmArchive.hpp>

#include <OTwo/IO/Loaders/MetadataLoader.hpp>

#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/ShapeLoader.hpp>
#include <OTwo/IO/Loaders/Audio/SoundLoader.hpp>
#include <OTwo/IO/Loaders/Audio/MusicLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/AnimationLoader.hpp>

#include <OTwo/IO/Loaders/UI/CursorLoader.hpp>
#include <OTwo/IO/Loaders/UI/ImageLoader.hpp>
#include <OTwo/IO/Loaders/UI/LabelLoader.hpp>
#include <OTwo/IO/Loaders/UI/ToolTipLoader.hpp>
#include <OTwo/IO/Loaders/UI/BitmapNumberLoader.hpp>
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
#include <OTwo/IO/Loaders/UI/Components/Common/ChatWindowLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Planet/ChannelButtonLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Planet/ChannelBoardLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Waiting/AvatarInfoLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Playing/EqualizerLoader.hpp>

#include <OTwo/IO/Loaders/Chart/ChartMetadataLoader.hpp>
#include <OTwo/IO/Loaders/Chart/ChartLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/StateLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/StatePlaying7KLoader.hpp>

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

#include <OTwo/Decorators/SceneGraph/SceneDirectorDecorator.hpp>
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
#include <OTwo/States/StateMyRoom.hpp>
#include <OTwo/States/StateItemShop.hpp>
#include <OTwo/States/StateWaiting7K.hpp>
#include <OTwo/States/StateLoading.hpp>
#include <OTwo/States/StateResult.hpp>

#include <OTwo/Config/GameConfig.hpp>
#include <OTwo/Utilities/Console.hpp>

void O2Jam::Boot()
{
    // Render Settings
    auto& window = GetMainWindow();
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(0);
    if (GetWindowState() == sf::State::Fullscreen)
        Gx::Application::SetView(GetLetterBoxView(window.getView(), window.getSize()));

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
    Gx::ResourceLoaderFactory::Register<Gx::Cursor, CursorLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Image, ImageLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::Label, LabelLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::ToolTip, ToolTipLoader>();
    Gx::ResourceLoaderFactory::Register<Gx::BitmapNumber, BitmapNumberLoader>();
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
    // O2Jam Exclusive Components
    Gx::ResourceLoaderFactory::Register<Marquee, MarqueeLoader>();
    Gx::ResourceLoaderFactory::Register<ChatWindow, ChatWindowLoader>();
    Gx::ResourceLoaderFactory::Register<ChannelButton, ChannelButtonLoader>();
    Gx::ResourceLoaderFactory::Register<ChannelBoard, ChannelBoardLoader>();
    Gx::ResourceLoaderFactory::Register<AvatarInfo, AvatarInfoLoader>();
    Gx::ResourceLoaderFactory::Register<Equalizer, EqualizerLoader>();
    // O2Jam Derived Components
    Gx::ResourceLoaderFactory::RegisterDerived<Gx::UiContainer, ChatPanel>();
    Gx::ResourceLoaderFactory::RegisterDerived<Gx::UiContainer, UserList>();
    Gx::ResourceLoaderFactory::RegisterDerived<Gx::UiContainer, MapSelector>();
    Gx::ResourceLoaderFactory::RegisterDerived<Gx::UiContainer, InstrumentSelector>();
    Gx::ResourceLoaderFactory::RegisterDerived<Gx::List, RoomList>();
    Gx::ResourceLoaderFactory::RegisterDerived<Gx::Image, RoomButton>();
    Gx::ResourceLoaderFactory::RegisterDerived<Gx::Image, PlayMenu>();
    // O2Jam Dialogs
    Gx::ResourceLoaderFactory::RegisterDerived<Gx::Dialog, OptionDialog>();
    Gx::ResourceLoaderFactory::RegisterDerived<Gx::Dialog, CreateRoomDialog>();
    Gx::ResourceLoaderFactory::RegisterDerived<Gx::Dialog, SelectMusicDialog>();
    // O2Jam Core Resources
    Gx::ResourceLoaderFactory::Register<ChartMetadata, ChartMetadataLoader>();
    Gx::ResourceLoaderFactory::Register<Chart, ChartLoader>();
    // SceneGraph
    Gx::ResourceLoaderFactory::Register<State, StateLoader>();
    Gx::ResourceLoaderFactory::RegisterDerived<State, StateAvi>();
    Gx::ResourceLoaderFactory::RegisterDerived<State, StatePlanet>();
    Gx::ResourceLoaderFactory::RegisterDerived<State, StateRoom>();
    Gx::ResourceLoaderFactory::RegisterDerived<State, StateMyRoom>();
    Gx::ResourceLoaderFactory::RegisterDerived<State, StateItemShop>();
    Gx::ResourceLoaderFactory::RegisterDerived<State, StateWaiting7K>();
    Gx::ResourceLoaderFactory::RegisterDerived<State, StateLoading>();
    Gx::ResourceLoaderFactory::RegisterDerived<State, StateResult>();
    Gx::ResourceLoaderFactory::Register<StatePlaying7K, StatePlaying7KLoader>();

    // Initialize singleton providers
    auto& context = GetContext();
    context.Provide<GameConfig>([] (auto& ctx)
    {
        // TODO: Load game config from file
        auto config = std::make_unique<GameConfig>();
        return config;
    }, Gx::Context::Scope::Singleton);

    context.Provide<Gx::ResourceManager>([](auto& ctx)
    {
        // Register shared resource container
        auto resources = std::make_unique<Gx::ResourceManager>();
        resources->Register<Item>();
        resources->Register<ItemData>();

        return resources;
    }, Gx::Context::Scope::Singleton);

    context.Provide<Gx::Mixer>([](auto& ctx)
    {
        auto mixer = std::make_unique<Gx::Mixer>();
        return mixer;
    }, Gx::Context::Scope::Singleton);

    context.Provide<ItemFactory>([&](auto& ctx)
    {
        auto factory = std::make_unique<ItemFactory>(ctx.template Require<Gx::ResourceManager>());
        return factory;
    }, Gx::Context::Scope::Singleton);

    context.Provide<SessionContext>([&](auto& ctx)
    {
        auto player    = Player();
        player.ID      = 1;
        player.Name    = "CXO2";
        player.Level   = -1;
        player.Rank    = 7;
        player.Exp     = 11200;
        player.NextExp = 345500;
        player.Wins    = 100;
        player.Draws   = 10;
        player.Loses   = 5;
        player.Gender  = Gender::Male;
        player.Cash    = 15000;
        player.Inventory = { 582,  1534, 342,  115,  255,
                             312,  112,  811,  722,  821,
                             1195, 1104, 1042, 1055, 1461,
                             1481, 44,   1493, 1269, 1084,
                             1494 };

        auto session  = std::make_unique<SessionContext>(player);
        return session;
    }, Gx::Context::Scope::Singleton);

    context.Provide<CartContext>([&] (auto& ctx)
    {
        return std::make_unique<CartContext>();
    }, Gx::Context::Scope::Singleton);

    context.Provide<ScoreTracker>([] (auto& ctx)
    {
        return std::make_unique<ScoreTracker>();
    }, Gx::Context::Scope::Singleton);

    // Initialize local providers
    context.Provide<JudgementStrategy>([] (auto& ctx)
    {
        return std::make_unique<RenderPositionJudgementStrategy>();
    });

    // Set-up console
    Console::Instance().SetFont(context.Require<Gx::ResourceManager>().AddFromFile<Gx::Font>("Interface/Common/Font.Monospace.ttf"));
    Console::Instance().SetCharacterSize(14);
    Console::Instance().SetBounds({{0, 0}, {400, 165}});
    Console::Instance().SetPosition({400, 0});
    Console::Instance().SetMaximumLines(10);

    // Load and set cursor
    SetCursor(context.Require<Gx::ResourceManager>().AddFromFile<Gx::Cursor>("Interface/Common/Window_Cursor.json"));

    // Force to load heavy providers during start-up
    auto _ = context.Require<SessionContext>().GetInstalledMusic();
    for (auto gender : {Gender::Male, Gender::Female})
        auto __ = context.Require<ItemFactory>().GetDefaultItems(gender);

    // Load global assets
    auto& resources = context.Require<Gx::ResourceManager>();
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
    director.Register<StateAvi>("Interface/State/Avi.json");
    director.Register<StatePlanet>("Interface/State/Planet.json");
    director.Register<StateRoom>("Interface/State/Room.json");
    director.Register<StateMyRoom>("Interface/State/MyRoom.json");
    director.Register<StateItemShop>("Interface/State/ItemShop.json");
    director.Register<StateWaiting7K>("Interface/State/Waiting7K.json");
    director.Register<StateLoading>("Interface/State/Loading.json");
    director.Register<StatePlaying7K>("Playing/State/Playing7K.json");
    director.Register<StateResult>("Interface/State/Result.json");

    director.Present<StateAvi>();
}

void O2Jam::OnFocusChanged(const bool focus)
{
    Application::OnFocusChanged(focus);

    const auto& context = GetContext();
    const auto& config  = context.Require<GameConfig>();
    auto& mixer         = context.Require<Gx::Mixer>();

    const bool ignored = GetSceneDirector().IsPresenting<StateAvi>()       ||
                         GetSceneDirector().IsPresenting<StatePlaying7K>() ||
                         GetSceneDirector().IsPresenting<StateResult>();

    const auto bgm = mixer.GetSoundGroup("BGM");
    const auto sfx = mixer.GetSoundGroup("SFX");
    if (focus)
    {
        bgm->SetVolume(static_cast<float>(config.MusicVolume));
        sfx->SetVolume(static_cast<float>(config.EffectVolume));

        if (ignored)
            return;

        mixer.Resume(sfx);
        mixer.Play(bgm);
    }
    else
    {
        bgm->SetVolume(0.f);
        sfx->SetVolume(0.f);

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
    Application::Shutdown();

    auto& director  = GetSceneDirector();
    auto& mixer     = GetContext().Require<Gx::Mixer>();
    auto& resources = GetContext().Require<Gx::ResourceManager>();

    director.Unload();
    mixer.Clear();
    resources.Clear();

    return 0;
}

void O2Jam::Update(const double delta)
{
    Application::Update(delta);

    if ((isKeyPressed(sf::Keyboard::Key::LAlt) || isKeyPressed(sf::Keyboard::Key::RAlt)) && isKeyPressed(sf::Keyboard::Key::Enter) && !m_windowStateSwitched)
    {
        m_windowStateSwitched = true;
        SetWindowState(GetWindowState() == sf::State::Fullscreen ? sf::State::Windowed : sf::State::Fullscreen);
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
                Gx::Application::SetView(GetInitialView());
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
        auto& app = Application::operator Gx::RenderSurface&();
        app.Render(buffer, Gx::RenderStates::Default);
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
