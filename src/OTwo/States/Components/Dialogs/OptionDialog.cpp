#include <OTwo/States/Components/Dialogs/OptionDialog.hpp>
#include <OTwo/States/State.hpp>

#include <Genode/Audio/Mixer.hpp>
#include <Genode/Audio/SoundGroup.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/ProgressBar.hpp>

#include <magic_enum.hpp>

OptionDialog::OptionDialog(const Gx::Dialog &copy) :
    Gx::Dialog(copy),
    Gx::UiContainer(copy),
    Gx::Node(copy),
    m_initialized(false)
{
}

void OptionDialog::Initialize()
{
    if (m_initialized)
        return;

    // Rewire callbacks due to copy constructor
    if (auto acceptButton = GetAcceptButton(); acceptButton)
        SetAcceptButton(*acceptButton);

    if (auto cancelButton = GetCancelButton(); cancelButton)
        SetCancelButton(*cancelButton);

    auto& app   = Gx::Application::Instance();
    auto& mixer = app.Require<Gx::Mixer>();
    m_parent    = GetParent<::State>();

    auto bgAllTest     = m_parent->Load<sf::Music>("IDC_DIALOG_OPTION/IDC_MUSIC_MASTER");
    auto bgTest        = m_parent->Load<sf::Music>("IDC_DIALOG_OPTION/IDC_MUSIC_SAMPLE");
    auto sfxTest       = m_parent->Load<sf::Sound>("IDC_DIALOG_OPTION/IDC_SOUND_SAMPLE");
    auto sfxNavigation = m_parent->Load<sf::Sound>("IDC_DIALOG_OPTION/IDC_SOUND_NAVIGATION");

    auto background = FindChild<Gx::Image>("IDC_IMAGE_DIALOG_OPTION");
    background->SetFrame("KeyOption");
    //SetTexCoords(background->GetFrame("KeyOption")->TexCoords);

    auto toolTip     = FindChild<Gx::ToolTip>("IDC_TOOLTIP_INFO");
    auto gameOption  = FindChild<Gx::UiContainer>("IDC_CONTAINER_GAME_OPTION");
    auto musicOption = FindChild<Gx::UiContainer>("IDC_CONTAINER_MUSIC_OPTION");

    auto keySelect = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_SELECT");
    auto keybar = FindChild<Gx::Image>("IDC_IMAGE_KEY_BAR");
    keySelect->AddChild(keybar);

    auto gfxCheckBox = gameOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_GFX");
    gfxCheckBox->SetCheckStateChangeCallback([=] (auto sender) { m_config.Use3D = sender->IsChecked(); });
    auto cursorCheckBox  = gameOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_CURSOR");
    cursorCheckBox->SetCheckStateChangeCallback([=] (auto sender) { m_config.UseWindowCursor = sender->IsChecked(); });
    auto keyTestCheckBox = gameOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_KEY_TEST");
    keyTestCheckBox->SetCheckStateChangeCallback([=] (auto sender)
    {
        m_keyTestEnabled = sender->IsChecked();
        m_keyChannel     = EventChannel::Note1;
        keySelect->SetFrame(0);
        keySelect->SetVisible(!m_keyTestEnabled);
    });

    m_keyChannel = EventChannel::Note1;
    for (unsigned int i = 1; i <= 7; i++)
    {
        auto channel = static_cast<EventChannel>(i + 1);
        auto keytext = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_TEXT_" + std::to_string(i));
        keytext->SetClickCallback([=] (auto &sender, auto &ev)
        {
            m_keyChannel = channel;
            keySelect->SetFrame(i - 1);
        });

        m_keyTexts[channel] = keytext;
    }

    for (unsigned int i = 1; i <= 7; i++)
    {
        auto keyDown = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_DOWN_" + std::to_string(i));
        auto channel = static_cast<EventChannel>(i + 1);

        keyDown->SetFrame("Note" + std::to_string(i));
        m_keyDowns[channel] = keyDown;
        AddChild(keyDown);
    }

    auto bgmCheckBox     = musicOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_BGM");
    bgmCheckBox->SetCheckStateChangeCallback([=] (auto sender) { m_config.UseBGM = sender->IsChecked(); });
    auto masterVolumeBar = musicOption->FindChild<Gx::ProgressBar>("IDC_PROGRESSBAR_MASTER_VOLUME");
    auto musicVolumeBar  = musicOption->FindChild<Gx::ProgressBar>("IDC_PROGRESSBAR_MUSIC_VOLUME");
    auto effectVolumeBar = musicOption->FindChild<Gx::ProgressBar>("IDC_PROGRESSBAR_SOUND_VOLUME");

    auto btnMasterVolumeUp   = musicOption->FindChild<Gx::Button>("IDC_BUTTON_MASTER_UP");
    auto btnMasterVolumeDown = musicOption->FindChild<Gx::Button>("IDC_BUTTON_MASTER_DOWN");
    btnMasterVolumeUp->SetHoldClickCallback([=, &mixer] (auto &sender, auto &ev)
    {
        masterVolumeBar->SetValue(masterVolumeBar->GetValue() + 1);
        musicVolumeBar->SetValue(masterVolumeBar->GetValue());
        effectVolumeBar->SetValue(masterVolumeBar->GetValue());

        m_config.MusicVolume  = static_cast<unsigned int>(masterVolumeBar->GetValue());
        m_config.EffectVolume = static_cast<unsigned int>(masterVolumeBar->GetValue());

        mixer.Pause("BGM");
        if (bgAllTest->getStatus() != sf::SoundSource::Playing)
        {
            mixer.Stop("BGTest");
            mixer.Stop("EFTest");
            mixer.Play(bgAllTest, "BGTest");
        }

        mixer.Play(sfxTest, "EFTest");
        mixer.GetSoundGroup("BGTest")->SetVolume(masterVolumeBar->GetValue());
        mixer.GetSoundGroup("EFTest")->SetVolume(masterVolumeBar->GetValue());
    });
    btnMasterVolumeDown->SetHoldClickCallback([=, &mixer] (auto &sender, auto &ev)
    {
        masterVolumeBar->SetValue(masterVolumeBar->GetValue() - 1);
        musicVolumeBar->SetValue(masterVolumeBar->GetValue());
        effectVolumeBar->SetValue(masterVolumeBar->GetValue());

        m_config.MusicVolume  = static_cast<unsigned int>(masterVolumeBar->GetValue());
        m_config.EffectVolume = static_cast<unsigned int>(masterVolumeBar->GetValue());

        mixer.Pause("BGM");
        if (bgAllTest->getStatus() != sf::SoundSource::Playing)
        {
            mixer.Stop("BGTest");
            mixer.Stop("EFTest");
            mixer.Play(bgAllTest, "BGTest");
        }

        mixer.Play(sfxTest, "EFTest");
        mixer.GetSoundGroup("BGTest")->SetVolume(masterVolumeBar->GetValue());
        mixer.GetSoundGroup("EFTest")->SetVolume(masterVolumeBar->GetValue());
    });

    auto btnMusicUp   = musicOption->FindChild<Gx::Button>("IDC_BUTTON_MUSIC_UP");
    auto btnMusicDown = musicOption->FindChild<Gx::Button>("IDC_BUTTON_MUSIC_DOWN");

    btnMusicUp->SetHoldClickCallback([=, &mixer] (auto &sender, auto &ev)
    {
        musicVolumeBar->SetValue(musicVolumeBar->GetValue() + 1);
        m_config.MusicVolume = static_cast<unsigned int>(musicVolumeBar->GetValue());

        mixer.Pause("BGM");
        if (bgTest->getStatus() != sf::SoundSource::Playing)
        {
            mixer.Stop("BGTest");
            mixer.Stop("EFTest");
            mixer.Play(bgTest, "BGTest");
        }

        mixer.GetSoundGroup("BGTest")->SetVolume(musicVolumeBar->GetValue());
    });
    btnMusicDown->SetHoldClickCallback([=, &mixer] (auto &sender, auto &ev)
    {
        musicVolumeBar->SetValue(musicVolumeBar->GetValue() - 1);
        m_config.MusicVolume = static_cast<unsigned int>(musicVolumeBar->GetValue());

        mixer.Pause("BGM");
        if (bgTest->getStatus() != sf::SoundSource::Playing)
        {
            mixer.Stop("BGTest");
            mixer.Stop("EFTest");
            mixer.Play(bgTest, "BGTest");
        }

        mixer.GetSoundGroup("BGTest")->SetVolume(musicVolumeBar->GetValue());
    });

    auto btnSoundEffectUp   = musicOption->FindChild<Gx::Button>("IDC_BUTTON_SOUND_UP");
    auto btnSoundEffectDown = musicOption->FindChild<Gx::Button>("IDC_BUTTON_SOUND_DOWN");

    btnSoundEffectUp->SetHoldClickCallback([=, &mixer] (auto &sender, auto &ev)
    {
        effectVolumeBar->SetValue(effectVolumeBar->GetValue() + 1);
        m_config.EffectVolume = static_cast<unsigned int>(effectVolumeBar->GetValue());

        mixer.Pause("BGM");
        if (sfxTest->getStatus() != sf::SoundSource::Playing)
        {
            mixer.Stop("BGTest");
            mixer.Stop("EFTest");
        }

        mixer.Play(sfxTest, "EFTest");
        mixer.GetSoundGroup("EFTest")->SetVolume(effectVolumeBar->GetValue());
    });
    btnSoundEffectDown->SetHoldClickCallback([=, &mixer] (auto &sender, auto &ev)
    {
        effectVolumeBar->SetValue(effectVolumeBar->GetValue() - 1);
        m_config.EffectVolume = static_cast<unsigned int>(effectVolumeBar->GetValue());

        mixer.Pause("BGM");
        if (sfxTest->getStatus() != sf::SoundSource::Playing)
        {
            mixer.Stop("BGTest");
            mixer.Stop("EFTest");
        }

        mixer.Play(sfxTest, "EFTest");
        mixer.GetSoundGroup("EFTest")->SetVolume(effectVolumeBar->GetValue());
    });

    auto btnSave = FindChild<Gx::Button>("IDC_BUTTON_SAVE");
    btnSave->SetClickCallback([=, &mixer] (auto &sender, auto &ev)
    {
        if (!ValidateConfig())
        {
            toolTip->SetString("Invalid Keysetting.");
            toolTip->Show(this);
            return;
        }

        auto& app = Gx::Application::Instance();
        app.SetConfig(m_config);

        auto keyTestCheckBox = gameOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_KEY_TEST");
        keyTestCheckBox->SetCheckedState(false);

        m_keyChannel = EventChannel::Note1;
        keySelect->SetFrame(0);

        if (auto bgGroup = mixer.GetSoundGroup("BGM"); bgGroup)
        {
            bgGroup->SetVolume(m_config.MusicVolume);
            bgGroup->SetEnabled(m_config.UseBGM);
        }

        if (auto sfxGroup = mixer.GetSoundGroup("SFX"); sfxGroup)
            sfxGroup->SetVolume(m_config.EffectVolume);

        toolTip->SetString("Setting has been saved.");
        toolTip->Show(this);
        Invalidate();
    });

    auto btnDefault = FindChild<Gx::Button>("IDC_BUTTON_DEFAULT");
    btnDefault->SetClickCallback([=, &mixer] (auto &sender, auto &ev)
    {
        m_config.Reset();
        btnSave->PerformClick();

        toolTip->SetString("Setting has reset to default.");
        toolTip->Show(this);
    });

    auto keyTab   = FindChild<Gx::RadioButton>("IDC_BUTTON_KEY_TAB");
    auto soundTab = FindChild<Gx::RadioButton>("IDC_BUTTON_SOUND_TAB");

    keyTab->SetCheckStateChangeCallback([=, &mixer] (auto sender)
    {
        if (!sender->IsChecked())
            return;

        auto bgGroup = mixer.GetSoundGroup("BGTest");
        auto efGroup = mixer.GetSoundGroup("EFTest");
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

    auto& app   = scene.GetApplication();
    auto& mixer = app.Require<Gx::Mixer>();
    auto config = app.GetConfig<GameConfig>();
    m_config    = GameConfig(config);

    auto background    = FindChild<Gx::Image>("IDC_IMAGE_DIALOG_OPTION");
    auto keyTab        = FindChild<Gx::RadioButton>("IDC_BUTTON_KEY_TAB");
    auto soundTab      = FindChild<Gx::RadioButton>("IDC_BUTTON_SOUND_TAB");
    auto gameOption    = FindChild<Gx::UiContainer>("IDC_CONTAINER_GAME_OPTION");
    auto musicOption   = FindChild<Gx::UiContainer>("IDC_CONTAINER_MUSIC_OPTION");
    auto toolTip       = FindChild<Gx::ToolTip>("IDC_TOOLTIP_INFO");
    auto keySelect     = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_SELECT");
    auto sfxNavigation = m_parent->Load<sf::Sound>("IDC_DIALOG_OPTION/IDC_SOUND_NAVIGATION");

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
    m_keyChannel = EventChannel::Note1;
    keySelect->SetFrame(0);
}

void OptionDialog::OnClose()
{
    auto& app   = Gx::Application::Instance();
    auto& mixer = app.Require<Gx::Mixer>();

    auto keyTab        = FindChild<Gx::RadioButton>("IDC_BUTTON_KEY_TAB");
    auto sfxNavigation = m_parent->Load<sf::Sound>("IDC_DIALOG_OPTION/IDC_SOUND_NAVIGATION");

    mixer.Stop("BGTest");
    mixer.Stop("EFTest");
    if (keyTab->IsChecked())
        mixer.Resume("BGM");
    else
        mixer.Play("BGM");

    mixer.Play(sfxNavigation, "SFX");
}

void OptionDialog::Update(double delta)
{
    UiContainer::Update(delta);
}

void OptionDialog::OnKeyDown(sf::Event::KeyEvent ev)
{
    Dialog::OnKeyDown(ev);

    auto gameOption = FindChild<Gx::UiContainer>("IDC_CONTAINER_GAME_OPTION");
    auto keySelect  = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_SELECT");

    if (gameOption->IsEnabled())
    {
        if (!m_keyTestEnabled)
        {
            auto keytext = m_keyTexts[m_keyChannel];
            auto keyStr = std::string(magic_enum::enum_name(ev.code));

            if (!keytext || !keytext->ContainsFrame(keyStr))
                return;

            m_config.SevenKeyBinding[m_keyChannel] = ev.code;
            keytext->SetFrame(keyStr);

            int index = static_cast<int>(m_keyChannel);
            if (index >= static_cast<int>(EventChannel::Note7))
                index = static_cast<int>(EventChannel::Note1);
            else
                index++;

            m_keyChannel = static_cast<EventChannel>(index);
            keySelect->SetFrame(index - static_cast<int>(EventChannel::Note1));
            keySelect->SetVisible(true);
        }
        else
        {
            keySelect->SetVisible(false);
            for (auto [channel, keyDown] : m_keyDowns)
            {
                if (m_config.SevenKeyBinding[channel] == ev.code)
                    keyDown->SetVisible(true);
            }
        }
    }
}

void OptionDialog::OnKeyUp(sf::Event::KeyEvent ev)
{
    Inputable::OnKeyUp(ev);

    if (m_keyTestEnabled)
    {
        auto gameOption = FindChild<Gx::UiContainer>("IDC_CONTAINER_GAME_OPTION");
        auto keySelect  = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_SELECT");

        keySelect->SetVisible(false);
        for (auto [channel, keyDown] : m_keyDowns)
        {
            if (m_config.SevenKeyBinding[channel] == ev.code)
                keyDown->SetVisible(false);
        }
    }
}

void OptionDialog::OnKeyType(sf::Event::TextEvent ev)
{
    Inputable::OnKeyType(ev);
}

bool OptionDialog::ValidateConfig()
{
    for (auto [channel, key] : m_config.SevenKeyBinding)
    {
        for (auto [pairChannel, pairKey] : m_config.SevenKeyBinding)
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

    auto gameOption  = FindChild<Gx::UiContainer>("IDC_CONTAINER_GAME_OPTION");
    auto musicOption = FindChild<Gx::UiContainer>("IDC_CONTAINER_MUSIC_OPTION");

    auto gfxCheckBox     = gameOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_GFX");
    auto cursorCheckBox  = gameOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_CURSOR");
    auto keyTestCheckBox = gameOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_KEY_TEST");
    auto keySelect       = gameOption->FindChild<Gx::Image>("IDC_IMAGE_KEY_SELECT");
    auto bgmCheckBox     = musicOption->FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_BGM");
    auto masterVolumeBar = musicOption->FindChild<Gx::ProgressBar>("IDC_PROGRESSBAR_MASTER_VOLUME");
    auto musicVolumeBar  = musicOption->FindChild<Gx::ProgressBar>("IDC_PROGRESSBAR_MUSIC_VOLUME");
    auto effectVolumeBar = musicOption->FindChild<Gx::ProgressBar>("IDC_PROGRESSBAR_SOUND_VOLUME");

    gfxCheckBox->SetCheckedState(m_config.Use3D);
    cursorCheckBox->SetCheckedState(m_config.UseWindowCursor);
    keyTestCheckBox->SetCheckedState(m_keyTestEnabled);
    bgmCheckBox->SetCheckedState(m_config.UseBGM);

    masterVolumeBar->SetValue(m_config.MusicVolume == m_config.EffectVolume ? m_config.MusicVolume : masterVolumeBar->GetValue());
    musicVolumeBar->SetValue(m_config.MusicVolume);
    effectVolumeBar->SetValue(m_config.EffectVolume);

    for (auto [channel, keytext] : m_keyTexts)
        keytext->SetFrame(std::string(magic_enum::enum_name(m_config.SevenKeyBinding[channel])));

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