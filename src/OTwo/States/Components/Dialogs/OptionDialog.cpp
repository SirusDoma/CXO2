#include <OTwo/States/Components/Dialogs/OptionDialog.hpp>
#include <OTwo/States/State.hpp>

#include <Genode/Audio/Mixer.hpp>
#include <Genode/Audio/SoundGroup.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/Gauge.hpp>

#include <magic_enum.hpp>

OptionDialog::OptionDialog(const Gx::Dialog &copy) :
    Gx::UiContainer(copy),
    Gx::Node(copy),
    Gx::Dialog(copy),
    m_parent(),
    m_keyTexts(),
    m_keyDowns(),
    m_config(),
    m_keyChannel(),
    m_keyTestEnabled(false),
    m_initialized(false)
{
}

void OptionDialog::Initialize()
{
    if (m_initialized)
        return;

    // Rewire callbacks due to copy constructor
    if (const auto acceptButton = GetAcceptButton(); acceptButton)
        SetAcceptButton(*acceptButton);

    if (const auto cancelButton = GetCancelButton(); cancelButton)
        SetCancelButton(*cancelButton);

    auto& app   = Gx::Application::Instance();
    auto& mixer = app.Require<Gx::Mixer>();
    m_parent    = GetParent<::State>();

    const auto bgAllTest     = m_parent->Load<sf::Music>("IDC_DIALOG_OPTION/IDC_MUSIC_MASTER");
    const auto bgTest        = m_parent->Load<sf::Music>("IDC_DIALOG_OPTION/IDC_MUSIC_SAMPLE");
    const auto sfxTest       = m_parent->Load<sf::Sound>("IDC_DIALOG_OPTION/IDC_SOUND_SAMPLE");
    const auto sfxNavigation = m_parent->Load<sf::Sound>("IDC_DIALOG_OPTION/IDC_SOUND_NAVIGATION");

    const auto background = FindChild<Gx::Image>("IDC_IMAGE_DIALOG_OPTION");
    background->SetFrame("KeyOption");
    //SetTexCoords(background->GetFrame("KeyOption")->TexCoords);

    const auto toolTip     = FindChild<Gx::ToolTip>("IDC_TOOLTIP_INFO");
    const auto gameOption  = FindChild<Gx::UiContainer>("IDC_CONTAINER_GAME_OPTION");
    const auto musicOption = FindChild<Gx::UiContainer>("IDC_CONTAINER_MUSIC_OPTION");

    const auto keySelect = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_SELECT");
    const auto keyBar = FindChild<Gx::Image>("IDC_IMAGE_KEY_BAR");
    keySelect->AddChild(keyBar);

    const auto gfxCheckBox = gameOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_GFX");
    gfxCheckBox->SetCheckStateChangeCallback([=] (auto sender) { m_config.UseFx = sender->IsChecked(); });
    const auto cursorCheckBox  = gameOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_CURSOR");
    cursorCheckBox->SetCheckStateChangeCallback([=] (auto sender) { m_config.UseWindowCursor = sender->IsChecked(); });
    const auto keyTestCheckBox = gameOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_KEY_TEST");
    keyTestCheckBox->SetCheckStateChangeCallback([=] (auto sender)
    {
        m_keyTestEnabled = sender->IsChecked();
        m_keyChannel     = Chart::Channel::Note1;
        keySelect->SetFrame(0);
        keySelect->SetVisible(!m_keyTestEnabled);
    });
    keyTestCheckBox->SetCheckedState(false);

    m_keyChannel = Chart::Channel::Note1;
    for (unsigned int i = 1; i <= 7; i++)
    {
        auto channel = static_cast<Chart::Channel>(i + 1);
        const auto keytext = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_TEXT_" + std::to_string(i));
        keytext->SetClickCallback([=] (auto &sender, auto &ev)
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
        AddChild(keyDown);
    }

    const auto bgmCheckBox           = musicOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_BGM");
    bgmCheckBox->SetCheckStateChangeCallback([=] (auto sender) { m_config.UseBGM = sender->IsChecked(); });
    const auto masterVolumeGauge = musicOption->FindChild<Gx::Gauge>("IDC_GAUGE_MASTER_VOLUME");
    const auto musicVolumeGauge  = musicOption->FindChild<Gx::Gauge>("IDC_GAUGE_MUSIC_VOLUME");
    const auto effectVolumeGauge = musicOption->FindChild<Gx::Gauge>("IDC_GAUGE_SOUND_VOLUME");

    const auto btnMasterVolumeUp   = musicOption->FindChild<Gx::Button>("IDC_BUTTON_MASTER_UP");
    const auto btnMasterVolumeDown = musicOption->FindChild<Gx::Button>("IDC_BUTTON_MASTER_DOWN");
    btnMasterVolumeUp->SetHoldClickCallback([=, &mixer] (auto &sender, auto &ev)
    {
        masterVolumeGauge->SetValue(masterVolumeGauge->GetValue() + 1);
        musicVolumeGauge->SetValue(masterVolumeGauge->GetValue());
        effectVolumeGauge->SetValue(masterVolumeGauge->GetValue());

        m_config.MusicVolume  = static_cast<unsigned int>(masterVolumeGauge->GetValue());
        m_config.EffectVolume = static_cast<unsigned int>(masterVolumeGauge->GetValue());

        mixer.Pause("BGM");
        if (bgAllTest->getStatus() != sf::SoundSource::Playing)
        {
            mixer.Stop("BGTest");
            mixer.Stop("EFTest");
            mixer.Play(bgAllTest, "BGTest");
        }

        mixer.Play(sfxTest, "EFTest");
        mixer.GetSoundGroup("BGTest")->SetVolume(masterVolumeGauge->GetValue());
        mixer.GetSoundGroup("EFTest")->SetVolume(masterVolumeGauge->GetValue());
    });
    btnMasterVolumeDown->SetHoldClickCallback([=, &mixer] (auto &sender, auto &ev)
    {
        masterVolumeGauge->SetValue(masterVolumeGauge->GetValue() - 1);
        musicVolumeGauge->SetValue(masterVolumeGauge->GetValue());
        effectVolumeGauge->SetValue(masterVolumeGauge->GetValue());

        m_config.MusicVolume  = static_cast<unsigned int>(masterVolumeGauge->GetValue());
        m_config.EffectVolume = static_cast<unsigned int>(masterVolumeGauge->GetValue());

        mixer.Pause("BGM");
        if (bgAllTest->getStatus() != sf::SoundSource::Playing)
        {
            mixer.Stop("BGTest");
            mixer.Stop("EFTest");
            mixer.Play(bgAllTest, "BGTest");
        }

        mixer.Play(sfxTest, "EFTest");
        mixer.GetSoundGroup("BGTest")->SetVolume(masterVolumeGauge->GetValue());
        mixer.GetSoundGroup("EFTest")->SetVolume(masterVolumeGauge->GetValue());
    });

    const auto btnMusicUp   = musicOption->FindChild<Gx::Button>("IDC_BUTTON_MUSIC_UP");
    const auto btnMusicDown = musicOption->FindChild<Gx::Button>("IDC_BUTTON_MUSIC_DOWN");

    btnMusicUp->SetHoldClickCallback([=, &mixer] (auto &sender, auto &ev)
    {
        musicVolumeGauge->SetValue(musicVolumeGauge->GetValue() + 1);
        m_config.MusicVolume = static_cast<unsigned int>(musicVolumeGauge->GetValue());

        mixer.Pause("BGM");
        if (bgTest->getStatus() != sf::SoundSource::Playing)
        {
            mixer.Stop("BGTest");
            mixer.Stop("EFTest");
            mixer.Play(bgTest, "BGTest");
        }

        mixer.GetSoundGroup("BGTest")->SetVolume(musicVolumeGauge->GetValue());
    });
    btnMusicDown->SetHoldClickCallback([=, &mixer] (auto &sender, auto &ev)
    {
        musicVolumeGauge->SetValue(musicVolumeGauge->GetValue() - 1);
        m_config.MusicVolume = static_cast<unsigned int>(musicVolumeGauge->GetValue());

        mixer.Pause("BGM");
        if (bgTest->getStatus() != sf::SoundSource::Playing)
        {
            mixer.Stop("BGTest");
            mixer.Stop("EFTest");
            mixer.Play(bgTest, "BGTest");
        }

        mixer.GetSoundGroup("BGTest")->SetVolume(musicVolumeGauge->GetValue());
    });

    const auto btnSoundEffectUp   = musicOption->FindChild<Gx::Button>("IDC_BUTTON_SOUND_UP");
    const auto btnSoundEffectDown = musicOption->FindChild<Gx::Button>("IDC_BUTTON_SOUND_DOWN");

    btnSoundEffectUp->SetHoldClickCallback([=, &mixer] (auto &sender, auto &ev)
    {
        effectVolumeGauge->SetValue(effectVolumeGauge->GetValue() + 1);
        m_config.EffectVolume = static_cast<unsigned int>(effectVolumeGauge->GetValue());

        mixer.Pause("BGM");
        if (sfxTest->getStatus() != sf::SoundSource::Playing)
        {
            mixer.Stop("BGTest");
            mixer.Stop("EFTest");
        }

        mixer.Play(sfxTest, "EFTest");
        mixer.GetSoundGroup("EFTest")->SetVolume(effectVolumeGauge->GetValue());
    });
    btnSoundEffectDown->SetHoldClickCallback([=, &mixer] (auto &sender, auto &ev)
    {
        effectVolumeGauge->SetValue(effectVolumeGauge->GetValue() - 1);
        m_config.EffectVolume = static_cast<unsigned int>(effectVolumeGauge->GetValue());

        mixer.Pause("BGM");
        if (sfxTest->getStatus() != sf::SoundSource::Playing)
        {
            mixer.Stop("BGTest");
            mixer.Stop("EFTest");
        }

        mixer.Play(sfxTest, "EFTest");
        mixer.GetSoundGroup("EFTest")->SetVolume(effectVolumeGauge->GetValue());
    });

    const auto btnSave = FindChild<Gx::Button>("IDC_BUTTON_SAVE");
    btnSave->SetClickCallback([=, &mixer] (auto &sender, auto &ev)
    {
        if (!ValidateConfig())
        {
            toolTip->SetString("Invalid Keysetting.");
            toolTip->Show(this);
            return;
        }

        auto& application = Gx::Application::Instance();
        application.Require<GameConfig>().Apply(m_config);

        m_keyChannel = Chart::Channel::Note1;
        keySelect->SetFrame(0);

        if (const auto bgGroup = mixer.GetSoundGroup("BGM"); bgGroup)
        {
            bgGroup->SetVolume(m_config.MusicVolume);
            bgGroup->SetEnabled(m_config.UseBGM);
        }

        if (const auto sfxGroup = mixer.GetSoundGroup("SFX"); sfxGroup)
            sfxGroup->SetVolume(m_config.EffectVolume);

        toolTip->SetString("Setting has been saved.");
        toolTip->Show(this);
        Invalidate();
    });

    const auto btnDefault = FindChild<Gx::Button>("IDC_BUTTON_DEFAULT");
    btnDefault->SetClickCallback([=] (auto &sender, auto &ev)
    {
        m_config.Reset();
        btnSave->PerformClick();

        toolTip->SetString("Setting has reset to default.");
        toolTip->Show(this);
    });

    const auto keyTab   = FindChild<Gx::RadioButton>("IDC_BUTTON_KEY_TAB");
    const auto soundTab = FindChild<Gx::RadioButton>("IDC_BUTTON_SOUND_TAB");

    keyTab->SetCheckStateChangeCallback([=, &mixer] (auto sender)
    {
        if (!sender->IsChecked())
            return;

        const auto bgGroup = mixer.GetSoundGroup("BGTest");
        const auto efGroup = mixer.GetSoundGroup("EFTest");
        mixer.Play(sfxNavigation, "SFX");

        if ((bgGroup && bgGroup->GetStatus() == sf::SoundSource::Playing) || (efGroup && efGroup->GetStatus() == sf::SoundSource::Playing))
            mixer.Play("BGM");
        else
            mixer.Resume("BGM");

        mixer.Stop(bgGroup);
        mixer.Stop(efGroup);

        if (bgGroup)
            bgGroup->SetVolume(m_config.MusicVolume);

        if (efGroup)
            efGroup->SetVolume(m_config.EffectVolume);

        background->SetFrame("KeyOption");
        //SetTexCoords(background->GetFrame("KeyOption")->TexCoords);

        gameOption->SetEnabled(true);
        gameOption->SetVisible(true);

        musicOption->SetEnabled(false);
        musicOption->SetVisible(false);
    });
    soundTab->SetCheckStateChangeCallback([=, &mixer] (auto sender)
    {
        if (!sender->IsChecked())
            return;

        mixer.Play(sfxNavigation, "SFX");
        mixer.Pause("BGM");
        mixer.Stop("BGTest");
        mixer.Stop("EFTest");

        mixer.Play(bgAllTest, "BGTest");
        mixer.GetSoundGroup("BGTest")->SetVolume(m_config.MusicVolume);

        background->SetFrame("SoundOption");
        //SetTexCoords(background->GetFrame("SoundOption")->TexCoords);

        gameOption->SetEnabled(false);
        gameOption->SetVisible(false);

        musicOption->SetEnabled(true);
        musicOption->SetVisible(true);
    });

    m_initialized = true;
}

void OptionDialog::OnShown(Gx::Scene &scene)
{
    Dialog::OnShown(scene);
    Initialize();

    auto& app    = scene.GetApplication();
    auto& mixer  = app.Require<Gx::Mixer>();
    auto& config = app.Require<GameConfig>();
    m_config     = GameConfig(config);

    const auto background    = FindChild<Gx::Image>("IDC_IMAGE_DIALOG_OPTION");
    const auto keyTab        = FindChild<Gx::RadioButton>("IDC_BUTTON_KEY_TAB");
    const auto soundTab      = FindChild<Gx::RadioButton>("IDC_BUTTON_SOUND_TAB");
    const auto gameOption    = FindChild<Gx::UiContainer>("IDC_CONTAINER_GAME_OPTION");
    const auto musicOption   = FindChild<Gx::UiContainer>("IDC_CONTAINER_MUSIC_OPTION");
    const auto toolTip       = FindChild<Gx::ToolTip>("IDC_TOOLTIP_INFO");
    const auto keySelect     = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_SELECT");
    const auto sfxNavigation = m_parent->Load<sf::Sound>("IDC_DIALOG_OPTION/IDC_SOUND_NAVIGATION");

    background->SetFrame("KeyOption");
    //SetTexCoords(background->GetFrame("KeyOption")->TexCoords);


    keyTab->SetCheckedState(true);
    mixer.Stop(sfxNavigation);

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
    auto& app   = Gx::Application::Instance();
    auto& mixer = app.Require<Gx::Mixer>();

    const auto keyTab        = FindChild<Gx::RadioButton>("IDC_BUTTON_KEY_TAB");
    const auto sfxNavigation = m_parent->Load<sf::Sound>("IDC_DIALOG_OPTION/IDC_SOUND_NAVIGATION");

    mixer.Stop("BGTest");
    mixer.Stop("EFTest");
    if (keyTab->IsChecked())
        mixer.Resume("BGM");
    else
        mixer.Play("BGM");

    mixer.Play(sfxNavigation, "SFX");
}

void OptionDialog::Update(const double delta)
{
    UiContainer::Update(delta);
}

void OptionDialog::OnKeyDown(const sf::Event::KeyEvent ev)
{
    Dialog::OnKeyDown(ev);

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

            m_config.KeyBindings[KeyMode::Seven][m_keyChannel] = ev.code;
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
                if (m_config.KeyBindings[KeyMode::Seven][channel] == ev.code)
                    keyDown->SetVisible(true);
            }
        }
    }
}

void OptionDialog::OnKeyUp(const sf::Event::KeyEvent ev)
{
    Inputable::OnKeyUp(ev);

    if (m_keyTestEnabled)
    {
        const auto gameOption = FindChild<Gx::UiContainer>("IDC_CONTAINER_GAME_OPTION");
        const auto keySelect  = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_SELECT");

        keySelect->SetVisible(false);
        for (auto [channel, keyDown] : m_keyDowns)
        {
            if (m_config.KeyBindings[KeyMode::Seven][channel] == ev.code)
                keyDown->SetVisible(false);
        }
    }
}

void OptionDialog::OnKeyType(const sf::Event::TextEvent ev)
{
    Inputable::OnKeyType(ev);
}

bool OptionDialog::ValidateConfig()
{
    for (auto [channel, key] : m_config.KeyBindings[KeyMode::Seven])
    {
        for (auto [pairChannel, pairKey] : m_config.KeyBindings[KeyMode::Seven])
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

    const auto gfxCheckBox       = gameOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_GFX");
    const auto cursorCheckBox    = gameOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_CURSOR");
    const auto keyTestCheckBox   = gameOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_KEY_TEST");
    const auto keySelect         = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_SELECT");
    const auto bgmCheckBox       = musicOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_BGM");
    const auto masterVolumeGauge = musicOption->FindChild<Gx::Gauge>("IDC_GAUGE_MASTER_VOLUME");
    const auto musicVolumeGauge  = musicOption->FindChild<Gx::Gauge>("IDC_GAUGE_MUSIC_VOLUME");
    const auto effectVolumeGauge = musicOption->FindChild<Gx::Gauge>("IDC_GAUGE_SOUND_VOLUME");

    gfxCheckBox->SetCheckedState(m_config.UseFx);
    cursorCheckBox->SetCheckedState(m_config.UseWindowCursor);
    keyTestCheckBox->SetCheckedState(m_keyTestEnabled);
    bgmCheckBox->SetCheckedState(m_config.UseBGM);

    masterVolumeGauge->SetValue(m_config.MusicVolume == m_config.EffectVolume ? m_config.MusicVolume : masterVolumeGauge->GetValue());
    musicVolumeGauge->SetValue(m_config.MusicVolume);
    effectVolumeGauge->SetValue(m_config.EffectVolume);

    for (auto [channel, keytext] : m_keyTexts)
        keytext->SetFrame(std::string(magic_enum::enum_name(m_config.KeyBindings[KeyMode::Seven][channel])));

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