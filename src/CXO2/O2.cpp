#include <CXO2/O2.hpp>

#include <Genode.hpp>

#include <CXO2/Archives/OpiArchive.hpp>
#include <CXO2/Archives/OjmArchive.hpp>
#include <CXO2/Archives/EmbeddedArchive.hpp>

#include <CXO2/IO/Loaders/MetadataLoader.hpp>

#include <CXO2/IO/Loaders/Graphics/TextureLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/FontLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/ShapeLoader.hpp>
#include <CXO2/IO/Loaders/Audio/SoundBufferLoader.hpp>
#include <CXO2/IO/Loaders/Audio/SoundLoader.hpp>
#include <CXO2/IO/Loaders/Audio/MusicLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/AnimationLoader.hpp>

#include <CXO2/IO/Loaders/Adaptor/ControlListLoader.hpp>
#include <CXO2/IO/Loaders/Adaptor/O2JamSpriteLoader.hpp>

#include <CXO2/IO/Loaders/UI/CursorLoader.hpp>
#include <CXO2/IO/Loaders/UI/ImageLoader.hpp>
#include <CXO2/IO/Loaders/UI/LabelLoader.hpp>
#include <CXO2/IO/Loaders/UI/ToolTipLoader.hpp>
#include <CXO2/IO/Loaders/UI/BitmapNumberLoader.hpp>
#include <CXO2/IO/Loaders/UI/ButtonLoader.hpp>
#include <CXO2/IO/Loaders/UI/ToggleButtonLoader.hpp>
#include <CXO2/IO/Loaders/UI/RadioButtonLoader.hpp>
#include <CXO2/IO/Loaders/UI/GaugeLoader.hpp>
#include <CXO2/IO/Loaders/UI/ListLoader.hpp>
#include <CXO2/IO/Loaders/UI/DialogLoader.hpp>
#include <CXO2/IO/Loaders/UI/InputFieldLoader.hpp>
#include <CXO2/IO/Loaders/UI/ScrollBarLoader.hpp>
#include <CXO2/IO/Loaders/UI/UiContainerLoader.hpp>

#include <CXO2/IO/Loaders/Avatar/ItemLoader.hpp>
#include <CXO2/IO/Loaders/Avatar/ItemDataLoader.hpp>
#include <CXO2/IO/Loaders/Avatar/SetInfoDataLoader.hpp>
#include <CXO2/IO/Loaders/Avatar/AvatarLoader.hpp>

#include <CXO2/IO/Loaders/UI/Components/Common/MarqueeLoader.hpp>
#include <CXO2/IO/Loaders/UI/Components/Common/ChatWindowLoader.hpp>
#include <CXO2/IO/Loaders/UI/Components/Planet/ChannelButtonLoader.hpp>
#include <CXO2/IO/Loaders/UI/Components/Planet/ChannelBoardLoader.hpp>
#include <CXO2/IO/Loaders/UI/Components/Waiting/AvatarInfoLoader.hpp>
#include <CXO2/IO/Loaders/UI/Components/Playing/EqualizerLoader.hpp>

#include <CXO2/IO/Loaders/Chart/O2JamChartMetadataLoader.hpp>
#include <CXO2/IO/Loaders/Chart/O2JamChartLoader.hpp>
#include <CXO2/IO/Loaders/SceneGraph/StateLoader.hpp>
#include <CXO2/IO/Loaders/SceneGraph/StatePlaying7KLoader.hpp>

#include <CXO2/IO/TextureCacheBuilder.hpp>
#include <CXO2/Metadata/Legacy/ControlList.hpp>

#include <CXO2/UI/Common/ChatPanel.hpp>
#include <CXO2/UI/Dialogs/CreateRoomDialog.hpp>
#include <CXO2/UI/Dialogs/OptionDialog.hpp>
#include <CXO2/UI/Dialogs/SelectMusicDialog.hpp>
#include <CXO2/UI/Room/RoomButton.hpp>
#include <CXO2/UI/Room/RoomList.hpp>
#include <CXO2/UI/Room/UserList.hpp>
#include <CXO2/UI/Waiting/InstrumentSelector.hpp>
#include <CXO2/UI/Waiting/MapSelector.hpp>
#include <CXO2/UI/Playing/PlayMenu.hpp>
#include <CXO2/UI/Playing/Equalizer.hpp>

#include <CXO2/Decorators/SceneGraph/SceneDirectorDecorator.hpp>

#include <CXO2/Services/MessageService.hpp>
#include <CXO2/Services/AuthService.hpp>
#include <CXO2/Services/PlanetService.hpp>
#include <CXO2/Services/CharacterService.hpp>
#include <CXO2/Services/ChatService.hpp>
#include <CXO2/Services/ChannelService.hpp>
#include <CXO2/Services/ItemShopService.hpp>
#include <CXO2/Services/WaitingService.hpp>
#include <CXO2/Services/PlayingService.hpp>

#include <CXO2/Contexts/CommandLineContext.hpp>
#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/GameContext.hpp>
#include <CXO2/Contexts/CartContext.hpp>

#include <CXO2/Avatar/ItemFactory.hpp>
#include <CXO2/Core/JudgementStrategy.hpp>
#include <CXO2/Core/Judgements/RenderPositionJudgementStrategy.hpp>
#include <CXO2/Core/Judgements/TimeJudgementStrategy.hpp>
#include <CXO2/Core/ScoreTracker.hpp>

#include <CXO2/States/StateAvi.hpp>
#include <CXO2/States/StatePlanet.hpp>
#include <CXO2/States/StateRoom.hpp>
#include <CXO2/States/StateMusicShop.hpp>
#include <CXO2/States/StateItemShop.hpp>
#include <CXO2/States/StateMyRoom.hpp>
#include <CXO2/States/StateBulletin.hpp>
#include <CXO2/States/StatePayment.hpp>
#include <CXO2/States/StateWaiting7K.hpp>
#include <CXO2/States/StateLoading.hpp>
#include <CXO2/States/StateResult.hpp>

#include <CXO2/Config/GameConfig.hpp>
#include <CXO2/Utilities/Console.hpp>
#include <CXO2/Resources.hpp>

namespace Cx
{
    O2::O2(std::string title, const sf::VideoMode& mode, const sf::View& view, const bool fullScreen, const sf::ContextSettings& settings) :
        Gx::Application(std::move(title), mode, view, fullScreen, settings)
    {
    }

    bool O2::InInteropMode()
    {
        return InInteropMode(
            InteropMode::Interface |
            InteropMode::Playing |
            InteropMode::Avatar
        );
    }

    bool O2::InInteropMode(const InteropMode modes)
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

    void O2::Boot()
    {
        // Render Settings
        auto& window = GetMainWindow();
        if (GetWindowState() == sf::State::Fullscreen)
            Gx::Application::SetView(GetLetterBoxView(window.getView(), window.getSize()));

        // Initialize singleton providers
        auto& context = GetModule<Gx::Context>();
        context.Provide<Gx::TcpNetworkClient>([](auto&)
        {
            auto client = std::make_unique<Gx::TcpNetworkClient>();
            client->UseDefaultPrefix<std::uint16_t>();

            return client;
        }, Gx::Context::Scope::Singleton);

        context.Provide<GameConfig>([](auto&)
        {
            auto config = std::make_unique<GameConfig>();
            config->Load();

            return config;
        }, Gx::Context::Scope::Singleton);

        context.Provide<Gx::FontManager>([] (auto&)
        {
            return std::make_unique<Gx::FontManager>();
        }, Gx::Context::Scope::Singleton);

        context.Provide<Gx::ResourceManager>([](auto&)
        {
            // Register shared resource container
            auto resources = std::make_unique<Gx::ResourceManager>();
            resources->Register<Item>();
            resources->Register<ItemData>();

            return resources;
        }, Gx::Context::Scope::Singleton);

        context.Provide<Gx::AudioMixer>([](auto& ctx)
        {
            auto mixer = std::make_unique<Gx::AudioMixer>();
            auto& cfg  = ctx.template Require<GameConfig>();

            mixer->GetSoundGroup(Sound::Channel::BGM).SetVolume(cfg.MusicVolume);
            mixer->GetSoundGroup(Sound::Channel::SFX).SetVolume(cfg.EffectVolume);

            return mixer;
        }, Gx::Context::Scope::Singleton);

        context.Provide<SessionContext>([&](auto& ctx)
        {
            const auto& cmd   = ctx.template Require<CommandLineContext>();
            std::string token = cmd.GetAuthToken();

            return std::make_unique<SessionContext>(token);
        }, Gx::Context::Scope::Singleton);

        context.Provide<GameContext>(Gx::Context::Scope::Singleton);
        context.Provide<RoomContext>(Gx::Context::Scope::Singleton);
        context.Provide<CartContext>(Gx::Context::Scope::Singleton);
        context.Provide<ScoreTracker>(Gx::Context::Scope::Singleton);

        // Initialize local providers
        context.Provide<JudgementStrategy>([] (auto&)
        {
            return std::make_unique<RenderPositionJudgementStrategy>();
        });

        // Initializes application modules
        Install<MessageService>();

        // Register services
        context.Provide<AuthService, AuthOnlineService>();
        context.Provide<PlanetService, PlanetOnlineService>();
        context.Provide<CharacterService, CharacterOnlineService>();
        context.Provide<ChatService, ChatOnlineService>();
        context.Provide<ItemShopService, ItemShopOnlineService>();
        context.Provide<ChannelService, ChannelOnlineService>();
        context.Provide<WaitingService, WaitingOnlineService>();
        context.Provide<PlayingService, PlayingOnlineService>();

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

        // Reroute bold font to embedded resource
        if (auto arialBlack = fontManager.GetData("Arial Black"))
            embedded.WriteFile("Interface/Common/Font.Bold.ttf", arialBlack->first, arialBlack->second);
        else if (auto defaultData = fontManager.GetDefaultData())
            embedded.WriteFile("Interface/Common/Font.Bold.ttf", defaultData->first, defaultData->second);

        // Embedded resources
        for (const auto& [name, resource] : Cx::Resources)
            embedded.WriteFile(std::string(name), resource.data, resource.size);

        Gx::FileSystem::Mount(embedded);

        // App Icon
        if (const auto stream = embedded.Open("Icon.png"))
        {
            m_icon = sf::Image(*stream);
            window.setIcon(*m_icon);
        }

        // Do NOT throw error when master archives cannot be loaded below.
        // There could be assets that override them completely.

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

        // Force to load item providers during start-up
        context.Provide<ItemFactory>([] (const Gx::Context& ctx)
        {
            try
            {
                // Scan for ItemData.dat
                std::string itemDataFileName = []
                {
                    if (const auto files = Gx::FileSystem::Scan("itemdata*.dat"); !files.empty())
                        return files.front()->GetName();

                    return std::string("Avatar/ItemData.json");
                }();

                // Scan for SetInfoData.ojs (optional)
                std::string setInfoDataFileName = []
                {
                    if (const auto files = Gx::FileSystem::Scan("setinfodata.*"); !files.empty())
                        return files.front()->GetName();

                    if (Gx::FileSystem::Contains("Avatar/SetInfoData.json"))
                        return std::string("Avatar/SetInfoData.json");

                    return std::string();
                }();

                return std::make_unique<ItemFactory>(
                    ctx.Require<Gx::ResourceManager>(),
                    itemDataFileName,
                    setInfoDataFileName
                );
            }
            catch (Gx::ResourceAccessException)
            {
                throw Gx::ResourceAccessException("Cannot find the avatar-related image.");
            }
        }, Gx::Context::Scope::Singleton);

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

        auto director = SceneDirectorDecorator::Decorate(GetModule<Gx::SceneDirector>());
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

    void O2::OnWindowCreated(sf::RenderWindow& window)
    {
        Gx::Application::OnWindowCreated(window);

        const auto& context = GetModule<Gx::Context>();
        const auto& config  = context.Require<GameConfig>();

        if (m_icon.has_value())
            window.setIcon(*m_icon);

        window.setVerticalSyncEnabled(config.UseVsync);
        window.setFramerateLimit(0);
    }

    void O2::OnFocusChanged(const bool focus)
    {
        Gx::Application::OnFocusChanged(focus);

        const auto& context = GetModule<Gx::Context>();
        const auto& config  = context.Require<GameConfig>();
        auto& mixer         = context.Require<Gx::AudioMixer>();

        const bool ignored = GetModule<Gx::SceneDirector>().IsPresenting<StateAvi>()       ||
                             GetModule<Gx::SceneDirector>().IsPresenting<StatePlaying7K>() ||
                             GetModule<Gx::SceneDirector>().IsPresenting<StateResult>();

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

    void O2::OnInputReceived(sf::Event& ev)
    {
        Gx::Application::OnInputReceived(ev);

        if (const auto key = ev.getIf<sf::Event::KeyReleased>(); key && key->code == sf::Keyboard::Key::F12)
            Console::Instance().SetEnabled(!Console::Instance().IsEnabled());
    }

    int O2::Shutdown()
    {
        auto& director  = GetModule<Gx::SceneDirector>();
        auto& mixer     = GetModule<Gx::Context>().Require<Gx::AudioMixer>();
        auto& resources = GetModule<Gx::Context>().Require<Gx::ResourceManager>();

        director.Reset();
        mixer.Reset(false);
        resources.Clear();

        return 0;
    }

    sf::VideoMode O2::GetVideoMode() const
    {
        auto mode = Gx::Application::GetVideoMode();
        if (GetWindowState() == sf::State::Fullscreen && Gx::GetCurrentPlatform() == Gx::Platform::macOS)
        {
            mode = GetDesktopVideoMode();
            mode.size.y -= 120;
        }

        return mode;
    }

    void O2::Update(const sf::Time& delta)
    {
        Gx::Application::Update(delta);

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

    Gx::RenderStates O2::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        if (GetWindowState() == sf::State::Fullscreen)
        {
            if (m_layeredTarget->getSize() != GetMainWindow().getSize())
                SetupLayeredTarget();

            m_layeredTarget->clear(GetClearColor());
            {
                Gx::Application::Render(m_layeredAdaptor, states);
                m_layeredAdaptor.Render(Console::Instance(), states);
            }
            m_layeredTarget->display();

            const auto buffer = Gx::Sprite(m_layeredTarget->getTexture());
            surface.Render(buffer, Gx::RenderStates::Default);
        }
        else
        {
            Gx::Application::Render(surface, states);
            surface.Render(Console::Instance(), states);
        }

        return states;
    }

    void O2::SetupLayeredTarget() const
    {
        m_layeredTarget = std::make_unique<sf::RenderTexture>(GetMainWindow().getSize(), GetSettings());
        m_layeredTarget->setSmooth(true);
        m_layeredAdaptor = Gx::RenderSurfaceAdaptor(*m_layeredTarget);
    }

    sf::View O2::GetLetterBoxView(sf::View view, const sf::Vector2u& windowSize)
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

    O2::operator sf::RenderTarget&() const
    {
        if (GetWindowState() == sf::State::Fullscreen)
            return *m_layeredTarget;

        return Gx::Application::operator sf::RenderTarget&();
    }
}
