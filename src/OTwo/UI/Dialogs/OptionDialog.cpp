#include <OTwo/UI/Dialogs/OptionDialog.hpp>
#include <OTwo/States/State.hpp>

#include <Genode/System/Application.hpp>

#include <Genode/Audio/Mixer.hpp>
#include <Genode/Audio/SoundGroup.hpp>

#include <Genode/UI/Cursor.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/ToggleButton.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/Gauge.hpp>
#include <Genode/UI/ToolTip.hpp>

#include <magic_enum.hpp>

OptionDialog::OptionDialog(Gx::Mixer& mixer, GameConfig& config) :
    m_parent(),
    m_mixer(mixer),
    m_appConfig(config),
    m_keyChannel(Chart::Channel::Note1)
{
}

void OptionDialog::Initialize()
{
    Gx::Dialog::Initialize();

    if (m_initialized)
        return;

    m_parent                 = GetParent<::State>();
    const auto bgAllTest     = m_parent->Instantiate<sf::Music>("bgEffect/MusicVolumn");
    const auto bgTest        = m_parent->Instantiate<sf::Music>("bgEffect/SampleSong.ogg");
    const auto sfxTest       = m_parent->Instantiate<sf::Sound>("bgEffect/38");
    const auto sfxNavigation = m_parent->Instantiate<sf::Sound>("bgEffect/01");

    bgAllTest->setLooping(true);
    bgTest->setLooping(true);

    const auto background = FindChild<Gx::Image>("IDC_IMAGE_DIALOG_OPTION");
    background->SetFrame("KeyOption");

    const auto toolTip     = FindChild<Gx::ToolTip>("IDC_TOOLTIP_INFO");
    const auto gameOption  = FindChild<Gx::UiContainer>("IDC_CONTAINER_GAME_OPTION");
    const auto musicOption = FindChild<Gx::UiContainer>("IDC_CONTAINER_MUSIC_OPTION");

    const auto keySelect = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_SELECT");
    const auto keyBar = FindChild<Gx::Image>("IDC_IMAGE_KEY_BAR");

    const auto gfxToggleButton = gameOption->FindChild<Gx::ToggleButton>("IDC_TOGGLE_GFX");
    gfxToggleButton->SetCheckStateChangeCallback([=] (auto& sender) { m_tempConfig.UseFx = sender.IsChecked(); });
    const auto cursorToggleButton  = gameOption->FindChild<Gx::ToggleButton>("IDC_TOGGLE_CURSOR");
    cursorToggleButton->SetCheckStateChangeCallback([=] (auto& sender) { m_tempConfig.UseWindowCursor = sender.IsChecked(); });
    const auto keyTestToggleButton = gameOption->FindChild<Gx::ToggleButton>("IDC_TOGGLE_KEY_TEST");
    keyTestToggleButton->SetCheckStateChangeCallback([=] (auto& sender)
    {
        m_keyTestEnabled = sender.IsChecked();
        m_keyChannel     = Chart::Channel::Note1;
        keySelect->SetFrame(0);
        keySelect->SetVisible(!m_keyTestEnabled);
    });
    keyTestToggleButton->SetCheckedState(false);

    m_keyChannel = Chart::Channel::Note1;
    for (unsigned int i = 1; i <= 7; i++)
    {
        auto channel = static_cast<Chart::Channel>(i + 1);
        const auto keytext = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_TEXT_" + std::to_string(i));
        keytext->SetClickCallback([=] (auto& sender, auto& ev)
        {
            m_keyChannel = channel;
            keySelect->SetFrame(i - 1);
        });

        m_keyTexts[channel] = keytext;
    }

    for (unsigned int i = 1; i <= 7; i++)
    {
        const auto keyDown = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_DOWN_" + std::to_string(i));
        auto channel = static_cast<Chart::Channel>(i + 1);

        keyDown->SetFrame("Note" + std::to_string(i));
        m_keyDowns[channel] = keyDown;
        // AddChild(keyDown);
    }

    const auto bgmToggleButton       = musicOption->FindChild<Gx::ToggleButton>("IDC_TOGGLE_BGM");
    bgmToggleButton->SetCheckStateChangeCallback([=] (auto& sender) { m_tempConfig.UseBGM = sender.IsChecked(); });
    const auto masterVolumeGauge = musicOption->FindChild<Gx::Gauge>("IDC_GAUGE_MASTER_VOLUME");
    const auto musicVolumeGauge  = musicOption->FindChild<Gx::Gauge>("IDC_GAUGE_MUSIC_VOLUME");
    const auto effectVolumeGauge = musicOption->FindChild<Gx::Gauge>("IDC_GAUGE_SOUND_VOLUME");

    const auto btnMasterVolumeUp   = musicOption->FindChild<Gx::Button>("IDC_BUTTON_MASTER_UP");
    const auto btnMasterVolumeDown = musicOption->FindChild<Gx::Button>("IDC_BUTTON_MASTER_DOWN");
    btnMasterVolumeUp->SetHoldClickCallback([=] (auto& sender, auto& ev)
    {
        masterVolumeGauge->SetValue(masterVolumeGauge->GetValue() + 1);
        musicVolumeGauge->SetValue(masterVolumeGauge->GetValue());
        effectVolumeGauge->SetValue(masterVolumeGauge->GetValue());

        m_tempConfig.MusicVolume  = static_cast<unsigned int>(masterVolumeGauge->GetValue());
        m_tempConfig.EffectVolume = static_cast<unsigned int>(masterVolumeGauge->GetValue());

        m_mixer.Pause("BGM");
        if (bgAllTest->getStatus() != sf::SoundSource::Status::Playing)
        {
            m_mixer.Stop("BGTest");
            m_mixer.Stop("EFTest");
            m_mixer.Play(bgAllTest, "BGTest");
        }

        m_mixer.Play(sfxTest, "EFTest");
        m_mixer.GetSoundGroup("BGTest")->SetVolume(masterVolumeGauge->GetValue());
        m_mixer.GetSoundGroup("EFTest")->SetVolume(masterVolumeGauge->GetValue());
    });
    btnMasterVolumeDown->SetHoldClickCallback([=] (auto& sender, auto& ev)
    {
        masterVolumeGauge->SetValue(masterVolumeGauge->GetValue() - 1);
        musicVolumeGauge->SetValue(masterVolumeGauge->GetValue());
        effectVolumeGauge->SetValue(masterVolumeGauge->GetValue());

        m_tempConfig.MusicVolume  = static_cast<unsigned int>(masterVolumeGauge->GetValue());
        m_tempConfig.EffectVolume = static_cast<unsigned int>(masterVolumeGauge->GetValue());

        m_mixer.Pause("BGM");
        if (bgAllTest->getStatus() != sf::SoundSource::Status::Playing)
        {
            m_mixer.Stop("BGTest");
            m_mixer.Stop("EFTest");
            m_mixer.Play(bgAllTest, "BGTest");
        }

        m_mixer.Play(sfxTest, "EFTest");
        m_mixer.GetSoundGroup("BGTest")->SetVolume(masterVolumeGauge->GetValue());
        m_mixer.GetSoundGroup("EFTest")->SetVolume(masterVolumeGauge->GetValue());
    });

    const auto btnMusicUp   = musicOption->FindChild<Gx::Button>("IDC_BUTTON_MUSIC_UP");
    const auto btnMusicDown = musicOption->FindChild<Gx::Button>("IDC_BUTTON_MUSIC_DOWN");

    btnMusicUp->SetHoldClickCallback([=] (auto& sender, auto& ev)
    {
        musicVolumeGauge->SetValue(musicVolumeGauge->GetValue() + 1);
        m_tempConfig.MusicVolume = static_cast<unsigned int>(musicVolumeGauge->GetValue());

        m_mixer.Pause("BGM");
        if (bgTest->getStatus() != sf::SoundSource::Status::Playing)
        {
            m_mixer.Stop("BGTest");
            m_mixer.Stop("EFTest");
            m_mixer.Play(bgTest, "BGTest");
        }

        m_mixer.GetSoundGroup("BGTest")->SetVolume(musicVolumeGauge->GetValue());
    });
    btnMusicDown->SetHoldClickCallback([=] (auto& sender, auto& ev)
    {
        musicVolumeGauge->SetValue(musicVolumeGauge->GetValue() - 1);
        m_tempConfig.MusicVolume = static_cast<unsigned int>(musicVolumeGauge->GetValue());

        m_mixer.Pause("BGM");
        if (bgTest->getStatus() != sf::SoundSource::Status::Playing)
        {
            m_mixer.Stop("BGTest");
            m_mixer.Stop("EFTest");
            m_mixer.Play(bgTest, "BGTest");
        }

        m_mixer.GetSoundGroup("BGTest")->SetVolume(musicVolumeGauge->GetValue());
    });

    const auto btnSoundEffectUp   = musicOption->FindChild<Gx::Button>("IDC_BUTTON_SOUND_UP");
    const auto btnSoundEffectDown = musicOption->FindChild<Gx::Button>("IDC_BUTTON_SOUND_DOWN");

    btnSoundEffectUp->SetHoldClickCallback([=] (auto& sender, auto& ev)
    {
        effectVolumeGauge->SetValue(effectVolumeGauge->GetValue() + 1);
        m_tempConfig.EffectVolume = static_cast<unsigned int>(effectVolumeGauge->GetValue());

        m_mixer.Pause("BGM");
        if (sfxTest->getStatus() != sf::SoundSource::Status::Playing)
        {
            m_mixer.Stop("BGTest");
            m_mixer.Stop("EFTest");
        }

        m_mixer.Play(sfxTest, "EFTest");
        m_mixer.GetSoundGroup("EFTest")->SetVolume(effectVolumeGauge->GetValue());
    });
    btnSoundEffectDown->SetHoldClickCallback([=] (auto& sender, auto& ev)
    {
        effectVolumeGauge->SetValue(effectVolumeGauge->GetValue() - 1);
        m_tempConfig.EffectVolume = static_cast<unsigned int>(effectVolumeGauge->GetValue());

        m_mixer.Pause("BGM");
        if (sfxTest->getStatus() != sf::SoundSource::Status::Playing)
        {
            m_mixer.Stop("BGTest");
            m_mixer.Stop("EFTest");
        }

        m_mixer.Play(sfxTest, "EFTest");
        m_mixer.GetSoundGroup("EFTest")->SetVolume(effectVolumeGauge->GetValue());
    });

    const auto btnSave = FindChild<Gx::Button>("IDC_BUTTON_SAVE");
    btnSave->SetClickCallback([=] (auto& sender, auto& ev)
    {
        if (!ValidateConfig())
        {
            toolTip->SetString("Invalid Keysetting.");
            toolTip->Show(this);
            return;
        }

        const auto& application = Gx::Application::Instance();
        m_appConfig.Apply(m_tempConfig);

        if (const auto cursor = application.GetCursor(); cursor)
        {
            cursor->SetEnabled(!m_tempConfig.UseWindowCursor);
            application.InvalidateCursor();
        }

        m_keyChannel = Chart::Channel::Note1;
        keySelect->SetFrame(0);

        if (const auto bgGroup = m_mixer.GetSoundGroup("BGM"); bgGroup)
        {
            bgGroup->SetVolume(m_tempConfig.MusicVolume);
            bgGroup->SetEnabled(m_tempConfig.UseBGM);
        }

        if (const auto sfxGroup = m_mixer.GetSoundGroup("SFX"); sfxGroup)
            sfxGroup->SetVolume(m_tempConfig.EffectVolume);

        toolTip->SetString("Setting has been saved.");
        toolTip->Show(this);
        Invalidate();
    });

    const auto btnDefault = FindChild<Gx::Button>("IDC_BUTTON_DEFAULT");
    btnDefault->SetClickCallback([=] (auto& sender, auto& ev)
    {
        m_tempConfig.Reset();
        btnSave->PerformClick();

        toolTip->SetString("Setting has reset to default.");
        toolTip->Show(this);
    });

    const auto keyTab   = FindChild<Gx::RadioButton>("IDC_BUTTON_KEY_TAB");
    const auto soundTab = FindChild<Gx::RadioButton>("IDC_BUTTON_SOUND_TAB");

    keyTab->SetCheckStateChangeCallback([=] (auto& sender)
    {
        if (!sender.IsChecked())
            return;

        const auto bgGroup = m_mixer.GetSoundGroup("BGTest");
        const auto efGroup = m_mixer.GetSoundGroup("EFTest");
        m_mixer.Play(sfxNavigation, "SFX");

        if ((bgGroup && bgGroup->GetStatus() == sf::SoundSource::Status::Playing) || (efGroup && efGroup->GetStatus() == sf::SoundSource::Status::Playing))
            m_mixer.Play("BGM");
        else
            m_mixer.Resume("BGM");

        m_mixer.Stop(bgGroup);
        m_mixer.Stop(efGroup);

        if (bgGroup)
            bgGroup->SetVolume(m_tempConfig.MusicVolume);

        if (efGroup)
            efGroup->SetVolume(m_tempConfig.EffectVolume);

        background->SetFrame("KeyOption");
        //SetTexCoords(background->GetFrame("KeyOption")->TexCoords);

        gameOption->SetEnabled(true);
        gameOption->SetVisible(true);

        musicOption->SetEnabled(false);
        musicOption->SetVisible(false);
    });
    soundTab->SetCheckStateChangeCallback([=] (auto& sender)
    {
        if (!sender.IsChecked())
            return;

        m_mixer.Play(sfxNavigation, "SFX");
        m_mixer.Pause("BGM");
        m_mixer.Stop("BGTest");
        m_mixer.Stop("EFTest");

        m_mixer.Play(bgAllTest, "BGTest");
        m_mixer.GetSoundGroup("BGTest")->SetVolume(m_tempConfig.MusicVolume);

        background->SetFrame("SoundOption");
        //SetTexCoords(background->GetFrame("SoundOption")->TexCoords);

        gameOption->SetEnabled(false);
        gameOption->SetVisible(false);

        musicOption->SetEnabled(true);
        musicOption->SetVisible(true);
    });

    m_initialized = true;
}

void OptionDialog::OnShown(Gx::Scene& scene)
{
    Dialog::OnShown(scene);
    Initialize();

    m_tempConfig             = GameConfig(m_appConfig);
    const auto background    = FindChild<Gx::Image>("IDC_IMAGE_DIALOG_OPTION");
    const auto keyTab        = FindChild<Gx::RadioButton>("IDC_BUTTON_KEY_TAB");
    const auto soundTab      = FindChild<Gx::RadioButton>("IDC_BUTTON_SOUND_TAB");
    const auto gameOption    = FindChild<Gx::UiContainer>("IDC_CONTAINER_GAME_OPTION");
    const auto musicOption   = FindChild<Gx::UiContainer>("IDC_CONTAINER_MUSIC_OPTION");
    const auto toolTip       = FindChild<Gx::ToolTip>("IDC_TOOLTIP_INFO");
    const auto keySelect     = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_SELECT");
    const auto sfxNavigation = m_parent->Instantiate<sf::Sound>("bgEffect/01");

    background->SetFrame("KeyOption");
    //SetTexCoords(background->GetFrame("KeyOption")->TexCoords);


    keyTab->SetCheckedState(true);
    m_mixer.Stop(sfxNavigation);

    soundTab->SetCheckedState(false);
    gameOption->SetEnabled(true);
    gameOption->SetVisible(true);
    musicOption->SetEnabled(false);
    musicOption->SetVisible(false);
    toolTip->Hide();

    m_keyTestEnabled = false;
    m_keyChannel = Chart::Channel::Note1;
    keySelect->SetFrame(0);
}

void OptionDialog::OnClose()
{
    const auto keyTab        = FindChild<Gx::RadioButton>("IDC_BUTTON_KEY_TAB");
    const auto sfxNavigation = m_parent->Instantiate<sf::Sound>("bgEffect/01");

    m_mixer.Stop("BGTest");
    m_mixer.Stop("EFTest");
    if (keyTab->IsChecked())
        m_mixer.Resume("BGM");
    else
        m_mixer.Play("BGM");

    m_mixer.Play(sfxNavigation, "SFX");
}

void OptionDialog::Update(const double delta)
{
    UiContainer::Update(delta);
}

void OptionDialog::OnKeyPressed(const sf::Event::KeyPressed& ev)
{
    Dialog::OnKeyPressed(ev);

    const auto gameOption = FindChild<Gx::UiContainer>("IDC_CONTAINER_GAME_OPTION");
    const auto keySelect  = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_SELECT");

    if (gameOption->IsEnabled())
    {
        if (!m_keyTestEnabled)
        {
            const auto keytext = m_keyTexts[m_keyChannel];
            const auto keyStr = std::string(magic_enum::enum_name(ev.code));

            if (!keytext || !keytext->ContainsFrame(keyStr))
                return;

            m_tempConfig.KeyBindings[KeyMode::Seven][m_keyChannel] = ev.code;
            keytext->SetFrame(keyStr);

            int index = static_cast<int>(m_keyChannel);
            if (index >= static_cast<int>(Chart::Channel::Note7))
                index = static_cast<int>(Chart::Channel::Note1);
            else
                index++;

            m_keyChannel = static_cast<Chart::Channel>(index);
            keySelect->SetFrame(index - static_cast<int>(Chart::Channel::Note1));
            keySelect->SetVisible(true);
        }
        else
        {
            keySelect->SetVisible(false);
            for (auto [channel, keyDown] : m_keyDowns)
            {
                if (m_tempConfig.KeyBindings[KeyMode::Seven][channel] == ev.code)
                    keyDown->SetVisible(true);
            }
        }
    }
}

void OptionDialog::OnKeyReleased(const sf::Event::KeyReleased& ev)
{
    Inputable::OnKeyReleased(ev);

    if (m_keyTestEnabled)
    {
        const auto gameOption = FindChild<Gx::UiContainer>("IDC_CONTAINER_GAME_OPTION");
        const auto keySelect  = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_SELECT");

        keySelect->SetVisible(false);
        for (auto [channel, keyDown] : m_keyDowns)
        {
            if (m_tempConfig.KeyBindings[KeyMode::Seven][channel] == ev.code)
                keyDown->SetVisible(false);
        }
    }
}

void OptionDialog::OnTextEntered(const sf::Event::TextEntered& ev)
{
    Inputable::OnTextEntered(ev);
}

bool OptionDialog::ValidateConfig()
{
    for (auto [channel, key] : m_tempConfig.KeyBindings[KeyMode::Seven])
    {
        for (auto [pairChannel, pairKey] : m_tempConfig.KeyBindings[KeyMode::Seven])
        {
            if (channel != pairChannel && key == pairKey)
                return false;
        }
    }

    return true;
}

void OptionDialog::Invalidate()
{
    Dialog::Invalidate();

    if (!IsShown())
        return;

    const auto gameOption  = FindChild<Gx::UiContainer>("IDC_CONTAINER_GAME_OPTION");
    const auto musicOption = FindChild<Gx::UiContainer>("IDC_CONTAINER_MUSIC_OPTION");

    const auto gfxToggleButton       = gameOption->FindChild<Gx::ToggleButton>("IDC_TOGGLE_GFX");
    const auto cursorToggleButton    = gameOption->FindChild<Gx::ToggleButton>("IDC_TOGGLE_CURSOR");
    const auto keyTestToggleButton   = gameOption->FindChild<Gx::ToggleButton>("IDC_TOGGLE_KEY_TEST");
    const auto keySelect         = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_SELECT");
    const auto bgmToggleButton       = musicOption->FindChild<Gx::ToggleButton>("IDC_TOGGLE_BGM");
    const auto masterVolumeGauge = musicOption->FindChild<Gx::Gauge>("IDC_GAUGE_MASTER_VOLUME");
    const auto musicVolumeGauge  = musicOption->FindChild<Gx::Gauge>("IDC_GAUGE_MUSIC_VOLUME");
    const auto effectVolumeGauge = musicOption->FindChild<Gx::Gauge>("IDC_GAUGE_SOUND_VOLUME");

    gfxToggleButton->SetCheckedState(m_tempConfig.UseFx);
    cursorToggleButton->SetCheckedState(m_tempConfig.UseWindowCursor);
    keyTestToggleButton->SetCheckedState(m_keyTestEnabled);
    bgmToggleButton->SetCheckedState(m_tempConfig.UseBGM);

    masterVolumeGauge->SetValue(m_tempConfig.MusicVolume == m_tempConfig.EffectVolume ? m_tempConfig.MusicVolume : masterVolumeGauge->GetValue());
    musicVolumeGauge->SetValue(m_tempConfig.MusicVolume);
    effectVolumeGauge->SetValue(m_tempConfig.EffectVolume);

    for (auto [channel, keytext] : m_keyTexts)
        keytext->SetFrame(std::string(magic_enum::enum_name(m_tempConfig.KeyBindings[KeyMode::Seven][channel])));

    if (!m_keyTestEnabled)
    {
        for (auto [channel, keyDown] : m_keyDowns)
            keyDown->SetVisible(false);
    }
    else
    {
        keySelect->SetVisible(false);
    }
}