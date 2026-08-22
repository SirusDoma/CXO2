#include <CXO2/UI/Components/Dialogs/OptionDialog.hpp>


#include <CXO2/States/State.hpp>

#include <CXO2/Constants/Identifiers/Sound.hpp>
#include <CXO2/Constants/Identifiers/Option.hpp>
#include <CXO2/Constants/Messages/Option.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/Audio/SoundGroup.hpp>

#include <Genode/Graphics/Cursor.hpp>
#include <CXO2/UI/Button.hpp>
#include <CXO2/UI/ToggleButton.hpp>
#include <CXO2/UI/RadioButton.hpp>
#include <CXO2/UI/Gauge.hpp>
#include <CXO2/UI/ToolTip.hpp>

#include <fmt/format.h>
#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    using namespace Constants::Identifiers;

    OptionDialog::OptionDialog(Gx::AudioMixer& mixer, GameConfig& config) :
        m_parent(),
        m_mixer(mixer),
        m_appConfig(config),
        m_keyChannel(Chart::Channel::Note1)
    {
    }

    void OptionDialog::Initialize()
    {
        Cx::Dialog::Initialize();

        if (m_initialized)
            return;

        m_parent                 = dynamic_cast<Cx::State*>(GetPresentableParent());
        const auto bgAllTest     = m_parent->Instantiate<sf::Music>(Sound::Effects::MUSIC_VOLUME);
        const auto bgTest        = m_parent->Instantiate<sf::Music>(Sound::Effects::SAMPLE_SONG);
        const auto sfxTest       = m_parent->Instantiate<sf::Sound>(Sound::Effects::EF_38);
        const auto sfxNavigation = m_parent->Instantiate<sf::Sound>(Sound::Effects::EF_01);

        bgAllTest->setLooping(true);
        bgTest->setLooping(true);

        const auto keyBackground = FindChild<Image>(Resource::Option::IDC_IMAGE_DIALOG_OPTION_KEY);
        const auto sndBackground = FindChild<Image>(Resource::Option::IDC_IMAGE_DIALOG_OPTION_SOUND);

        keyBackground->SetVisible(true);
        sndBackground->SetVisible(false);

        const auto gameOption  = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_GAME_OPTION);
        const auto musicOption = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_MUSIC_OPTION);

        const auto keySelect = gameOption->FindChild<Image>(Resource::Option::IDC_IMAGE_KEY_SELECT);
        const auto keyBar    = keySelect->FindChild<Image>(Resource::Option::IDC_IMAGE_KEY_BAR);

        const auto gfxToggleButton = gameOption->FindChild<Cx::ToggleButton>(Resource::Option::IDC_TOGGLE_GFX);
        gfxToggleButton->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnGfxToggleCheckChanged(sender, ev); });
        const auto cursorToggleButton  = gameOption->FindChild<Cx::ToggleButton>(Resource::Option::IDC_TOGGLE_CURSOR);
        cursorToggleButton->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnCursorToggleCheckChanged(sender, ev); });
        const auto keyTestToggleButton = gameOption->FindChild<Cx::ToggleButton>(Resource::Option::IDC_TOGGLE_KEY_TEST);
        keyTestToggleButton->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnKeyTestToggleCheckChanged(sender, ev); });
        keyTestToggleButton->SetCheckedState(false);

        m_keyChannel = Chart::Channel::Note1;
        for (std::size_t i = 0; i < 7; i++)
        {
            auto channel = static_cast<Chart::Channel>(i + 2);
            const auto keytext = gameOption->FindChild<Image>(Resource::Option::IDC_IMAGE_KEY_TEXT[i]);
            m_keyTextChannels[keytext] = { channel, i };
            keytext->SetClickCallback([this] (auto& sender, auto& ev) { OnKeyTextClicked(sender, ev); });

            m_keyTexts[channel] = keytext;
        }

        for (std::size_t i = 0; i < 7; i++)
        {
            const auto keyDown = gameOption->FindChild<Image>(Resource::Option::IDC_IMAGE_KEY_DOWN[i]);
            auto channel = static_cast<Chart::Channel>(i + 2);

            keyDown->SetFrame(fmt::format("Note{}", i + 1));
            m_keyDowns[channel] = keyDown;
        }

        const auto bgmToggleButton = musicOption->FindChild<Cx::ToggleButton>(Resource::Option::IDC_TOGGLE_BGM);
        bgmToggleButton->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnBgmToggleCheckChanged(sender, ev); });

        const auto btnMasterVolumeUp   = musicOption->FindChild<Cx::Button>(Resource::Option::IDC_BUTTON_MASTER_UP);
        const auto btnMasterVolumeDown = musicOption->FindChild<Cx::Button>(Resource::Option::IDC_BUTTON_MASTER_DOWN);
        btnMasterVolumeUp->SetHoldClickCallback([this] (auto& sender, auto& ev) { OnMasterVolumeUpButtonHeld(sender, ev); });
        btnMasterVolumeDown->SetHoldClickCallback([this] (auto& sender, auto& ev) { OnMasterVolumeDownButtonHeld(sender, ev); });

        const auto btnMusicUp   = musicOption->FindChild<Cx::Button>(Resource::Option::IDC_BUTTON_MUSIC_UP);
        const auto btnMusicDown = musicOption->FindChild<Cx::Button>(Resource::Option::IDC_BUTTON_MUSIC_DOWN);

        btnMusicUp->SetHoldClickCallback([this] (auto& sender, auto& ev) { OnMusicVolumeUpButtonHeld(sender, ev); });
        btnMusicDown->SetHoldClickCallback([this] (auto& sender, auto& ev) { OnMusicVolumeDownButtonHeld(sender, ev); });

        const auto btnSoundEffectUp   = musicOption->FindChild<Cx::Button>(Resource::Option::IDC_BUTTON_SOUND_UP);
        const auto btnSoundEffectDown = musicOption->FindChild<Cx::Button>(Resource::Option::IDC_BUTTON_SOUND_DOWN);

        btnSoundEffectUp->SetHoldClickCallback([this] (auto& sender, auto& ev) { OnEffectVolumeUpButtonHeld(sender, ev); });
        btnSoundEffectDown->SetHoldClickCallback([this] (auto& sender, auto& ev) { OnEffectVolumeDownButtonHeld(sender, ev); });

        const auto btnSave = FindChild<Cx::Button>(Resource::Option::IDC_BUTTON_SAVE);
        btnSave->SetClickCallback([this] (auto& sender, auto& ev) { OnSaveButtonClicked(sender, ev); });

        const auto btnDefault = FindChild<Cx::Button>(Resource::Option::IDC_BUTTON_DEFAULT);
        btnDefault->SetClickCallback([this] (auto& sender, auto& ev) { OnDefaultButtonClicked(sender, ev); });

        const auto keyTab   = FindChild<Cx::RadioButton>(Resource::Option::IDC_BUTTON_KEY_TAB);
        const auto soundTab = FindChild<Cx::RadioButton>(Resource::Option::IDC_BUTTON_SOUND_TAB);

        keyTab->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnKeyTabCheckChanged(sender, ev); });
        soundTab->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnSoundTabCheckChanged(sender, ev); });

        m_initialized = true;
    }

    void OptionDialog::OnGfxToggleCheckChanged(Cx::ToggleButton& sender, Control::Event& ev)
    {
        m_tempConfig.UseFx = sender.IsChecked();
    }

    void OptionDialog::OnCursorToggleCheckChanged(Cx::ToggleButton& sender, Control::Event& ev)
    {
        m_tempConfig.UseWindowCursor = sender.IsChecked();
    }

    void OptionDialog::OnKeyTestToggleCheckChanged(Cx::ToggleButton& sender, Control::Event& ev)
    {
        const auto gameOption = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_GAME_OPTION);
        const auto keySelect  = gameOption->FindChild<Image>(Resource::Option::IDC_IMAGE_KEY_SELECT);

        m_keyTestEnabled = sender.IsChecked();
        m_keyChannel     = Chart::Channel::Note1;
        keySelect->SetFrame(0);
        keySelect->SetVisible(!m_keyTestEnabled);
    }

    void OptionDialog::OnBgmToggleCheckChanged(Cx::ToggleButton& sender, Control::Event& ev)
    {
        m_tempConfig.UseBGM = sender.IsChecked();
    }

    void OptionDialog::OnKeyTextClicked(Control& sender, Control::Event& ev)
    {
        const auto gameOption = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_GAME_OPTION);
        const auto keySelect  = gameOption->FindChild<Image>(Resource::Option::IDC_IMAGE_KEY_SELECT);

        const auto [channel, i] = m_keyTextChannels.at(&sender);
        m_keyChannel = channel;
        keySelect->SetFrame(i);
    }

    void OptionDialog::AdjustMasterVolume(const int offset)
    {
        const auto bgAllTest = m_parent->Instantiate<sf::Music>(Sound::Effects::MUSIC_VOLUME);
        const auto sfxTest   = m_parent->Instantiate<sf::Sound>(Sound::Effects::EF_38);

        const auto musicOption       = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_MUSIC_OPTION);
        const auto masterVolumeGauge = musicOption->FindChild<Gauge>(Resource::Option::IDC_GAUGE_MASTER_VOLUME);
        const auto musicVolumeGauge  = musicOption->FindChild<Gauge>(Resource::Option::IDC_GAUGE_MUSIC_VOLUME);
        const auto effectVolumeGauge = musicOption->FindChild<Gauge>(Resource::Option::IDC_GAUGE_SOUND_VOLUME);

        masterVolumeGauge->SetValue(masterVolumeGauge->GetValue() + offset);
        musicVolumeGauge->SetValue(masterVolumeGauge->GetValue());
        effectVolumeGauge->SetValue(masterVolumeGauge->GetValue());

        m_tempConfig.MusicVolume  = static_cast<unsigned int>(masterVolumeGauge->GetValue());
        m_tempConfig.EffectVolume = static_cast<unsigned int>(masterVolumeGauge->GetValue());

        m_mixer.Pause(Sound::Channel::BGM);
        if (bgAllTest->getStatus() != sf::SoundSource::Status::Playing)
        {
            m_mixer.Stop(Sound::Channel::BGTest);
            m_mixer.Stop(Sound::Channel::EFTest);
            m_mixer.Play(*bgAllTest, Sound::Channel::BGTest);
        }

        m_mixer.Play(*sfxTest, Sound::Channel::EFTest);
        m_mixer.GetSoundGroup(Sound::Channel::BGTest).SetVolume(masterVolumeGauge->GetValue());
        m_mixer.GetSoundGroup(Sound::Channel::EFTest).SetVolume(masterVolumeGauge->GetValue());
    }

    void OptionDialog::OnMasterVolumeUpButtonHeld(Control& sender, Control::Event& ev)
    {
        AdjustMasterVolume(1);
    }

    void OptionDialog::OnMasterVolumeDownButtonHeld(Control& sender, Control::Event& ev)
    {
        AdjustMasterVolume(-1);
    }

    void OptionDialog::AdjustMusicVolume(const int offset)
    {
        const auto bgTest = m_parent->Instantiate<sf::Music>(Sound::Effects::SAMPLE_SONG);

        const auto musicOption      = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_MUSIC_OPTION);
        const auto musicVolumeGauge = musicOption->FindChild<Gauge>(Resource::Option::IDC_GAUGE_MUSIC_VOLUME);

        musicVolumeGauge->SetValue(musicVolumeGauge->GetValue() + offset);
        m_tempConfig.MusicVolume = static_cast<unsigned int>(musicVolumeGauge->GetValue());

        m_mixer.Pause(Sound::Channel::BGM);
        if (bgTest->getStatus() != sf::SoundSource::Status::Playing)
        {
            m_mixer.Stop(Sound::Channel::BGTest);
            m_mixer.Stop(Sound::Channel::EFTest);
            m_mixer.Play(*bgTest, Sound::Channel::BGTest);
        }

        m_mixer.GetSoundGroup(Sound::Channel::BGTest).SetVolume(musicVolumeGauge->GetValue());
    }

    void OptionDialog::OnMusicVolumeUpButtonHeld(Control& sender, Control::Event& ev)
    {
        AdjustMusicVolume(1);
    }

    void OptionDialog::OnMusicVolumeDownButtonHeld(Control& sender, Control::Event& ev)
    {
        AdjustMusicVolume(-1);
    }

    void OptionDialog::AdjustEffectVolume(const int offset)
    {
        const auto sfxTest = m_parent->Instantiate<sf::Sound>(Sound::Effects::EF_38);

        const auto musicOption       = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_MUSIC_OPTION);
        const auto effectVolumeGauge = musicOption->FindChild<Gauge>(Resource::Option::IDC_GAUGE_SOUND_VOLUME);

        effectVolumeGauge->SetValue(effectVolumeGauge->GetValue() + offset);
        m_tempConfig.EffectVolume = static_cast<unsigned int>(effectVolumeGauge->GetValue());

        m_mixer.Pause(Sound::Channel::BGM);
        if (sfxTest->getStatus() != sf::SoundSource::Status::Playing)
        {
            m_mixer.Stop(Sound::Channel::BGTest);
            m_mixer.Stop(Sound::Channel::EFTest);
        }

        m_mixer.Play(*sfxTest, Sound::Channel::EFTest);
        m_mixer.GetSoundGroup(Sound::Channel::EFTest).SetVolume(effectVolumeGauge->GetValue());
    }

    void OptionDialog::OnEffectVolumeUpButtonHeld(Control& sender, Control::Event& ev)
    {
        AdjustEffectVolume(1);
    }

    void OptionDialog::OnEffectVolumeDownButtonHeld(Control& sender, Control::Event& ev)
    {
        AdjustEffectVolume(-1);
    }

    void OptionDialog::OnSaveButtonClicked(Control& sender, Control::Event& ev)
    {
        const auto toolTip    = FindChild<Cx::ToolTip>(Resource::Option::IDC_TOOLTIP_INFO);
        const auto gameOption = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_GAME_OPTION);
        const auto keySelect  = gameOption->FindChild<Image>(Resource::Option::IDC_IMAGE_KEY_SELECT);

        const auto keyTab   = FindChild<Cx::RadioButton>(Resource::Option::IDC_BUTTON_KEY_TAB);
        const auto soundTab = FindChild<Cx::RadioButton>(Resource::Option::IDC_BUTTON_SOUND_TAB);

        if (!ValidateConfig())
            return;

        const auto& application = Gx::Application::Instance();
        m_appConfig.Apply(m_tempConfig);

        if (const auto cursor = application.GetCursor(); cursor)
        {
            cursor->SetEnabled(!m_tempConfig.UseWindowCursor);
            application.InvalidateCursor();
        }

        m_keyChannel = Chart::Channel::Note1;
        keySelect->SetFrame(0);

        auto& bgGroup = m_mixer.GetSoundGroup(Sound::Channel::BGM);
        bgGroup.SetVolume(m_tempConfig.MusicVolume);
        bgGroup.SetEnabled(m_tempConfig.UseBGM);

        auto& sfxGroup = m_mixer.GetSoundGroup(Sound::Channel::SFX);
        sfxGroup.SetVolume(m_tempConfig.EffectVolume);

        if (keyTab->IsChecked())
            toolTip->SetString(Constants::Messages::Option::KeySetting::SAVED);
        else if (soundTab->IsChecked())
            toolTip->SetString(Constants::Messages::Option::SoundSetting::SAVED);

        toolTip->Show(*this);
        Invalidate();
    }

    void OptionDialog::OnDefaultButtonClicked(Control& sender, Control::Event& ev)
    {
        const auto btnSave = FindChild<Cx::Button>(Resource::Option::IDC_BUTTON_SAVE);
        const auto toolTip = FindChild<Cx::ToolTip>(Resource::Option::IDC_TOOLTIP_INFO);

        const auto keyTab   = FindChild<Cx::RadioButton>(Resource::Option::IDC_BUTTON_KEY_TAB);
        const auto soundTab = FindChild<Cx::RadioButton>(Resource::Option::IDC_BUTTON_SOUND_TAB);

        m_tempConfig.Reset();
        btnSave->PerformClick();

        if (keyTab->IsChecked())
            toolTip->SetString(Constants::Messages::Option::KeySetting::RESET);
        else if (soundTab->IsChecked())
            toolTip->SetString(Constants::Messages::Option::SoundSetting::RESET);

        toolTip->Show(*this);
    }

    void OptionDialog::OnKeyTabCheckChanged(Cx::RadioButton& sender, Control::Event& ev)
    {
        if (!sender.IsChecked())
            return;

        const auto keyBackground = FindChild<Image>(Resource::Option::IDC_IMAGE_DIALOG_OPTION_KEY);
        const auto sndBackground = FindChild<Image>(Resource::Option::IDC_IMAGE_DIALOG_OPTION_SOUND);
        const auto gameOption    = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_GAME_OPTION);
        const auto musicOption   = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_MUSIC_OPTION);
        const auto sfxNavigation = m_parent->Instantiate<sf::Sound>(Sound::Effects::EF_01);

        auto& bgGroup = m_mixer.GetSoundGroup(Sound::Channel::BGTest);
        auto& efGroup = m_mixer.GetSoundGroup(Sound::Channel::EFTest);

        for (auto& source : m_mixer.GetSoundGroup(Sound::Channel::BGM))
        {
            if (source.getStatus() == sf::SoundSource::Status::Paused)
            {
                m_mixer.Play(Sound::Channel::BGM);
                break;
            }
        }

        m_mixer.Play(*sfxNavigation, Sound::Channel::SFX);

        bgGroup.Stop();
        bgGroup.Reset();

        efGroup.Stop();
        efGroup.Reset();

        bgGroup.SetVolume(m_tempConfig.MusicVolume);
        efGroup.SetVolume(m_tempConfig.EffectVolume);

        keyBackground->SetVisible(true);
        sndBackground->SetVisible(false);

        gameOption->SetEnabled(true);
        gameOption->SetVisible(true);

        musicOption->SetEnabled(false);
        musicOption->SetVisible(false);
    }

    void OptionDialog::OnSoundTabCheckChanged(Cx::RadioButton& sender, Control::Event& ev)
    {
        if (!sender.IsChecked())
            return;

        const auto keyBackground = FindChild<Image>(Resource::Option::IDC_IMAGE_DIALOG_OPTION_KEY);
        const auto sndBackground = FindChild<Image>(Resource::Option::IDC_IMAGE_DIALOG_OPTION_SOUND);
        const auto gameOption    = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_GAME_OPTION);
        const auto musicOption   = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_MUSIC_OPTION);
        const auto bgAllTest     = m_parent->Instantiate<sf::Music>(Sound::Effects::MUSIC_VOLUME);
        const auto sfxNavigation = m_parent->Instantiate<sf::Sound>(Sound::Effects::EF_01);

        m_mixer.Play(*sfxNavigation, Sound::Channel::SFX);
        m_mixer.Pause(Sound::Channel::BGM);
        m_mixer.Stop(Sound::Channel::BGTest);
        m_mixer.Stop(Sound::Channel::EFTest);

        m_mixer.Play(*bgAllTest, Sound::Channel::BGTest);
        m_mixer.GetSoundGroup(Sound::Channel::BGTest).SetVolume(m_tempConfig.MusicVolume);

        keyBackground->SetVisible(false);
        sndBackground->SetVisible(true);

        gameOption->SetEnabled(false);
        gameOption->SetVisible(false);

        musicOption->SetEnabled(true);
        musicOption->SetVisible(true);
    }

    void OptionDialog::OnPresented(Parent& parent, const Gx::PresentationContext& context)
    {
        Dialog::OnPresented(parent, context);

        m_parent = dynamic_cast<Cx::State*>(&parent);
        Initialize();

        m_tempConfig             = GameConfig(m_appConfig);
        const auto keyBackground = FindChild<Image>(Resource::Option::IDC_IMAGE_DIALOG_OPTION_KEY);
        const auto sndBackground = FindChild<Image>(Resource::Option::IDC_IMAGE_DIALOG_OPTION_SOUND);
        const auto keyTab        = FindChild<Cx::RadioButton>(Resource::Option::IDC_BUTTON_KEY_TAB);
        const auto soundTab      = FindChild<Cx::RadioButton>(Resource::Option::IDC_BUTTON_SOUND_TAB);
        const auto gameOption    = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_GAME_OPTION);
        const auto musicOption   = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_MUSIC_OPTION);
        const auto toolTip       = FindChild<Cx::ToolTip>(Resource::Option::IDC_TOOLTIP_INFO);
        const auto keySelect     = gameOption->FindChild<Image>(Resource::Option::IDC_IMAGE_KEY_SELECT);
        const auto sfxNavigation = m_parent->Instantiate<sf::Sound>(Sound::Effects::EF_01);

        keyBackground->SetVisible(true);
        sndBackground->SetVisible(false);

        keyTab->SetCheckedState(true);
        sfxNavigation->stop();

        soundTab->SetCheckedState(false);
        gameOption->SetEnabled(true);
        gameOption->SetVisible(true);
        musicOption->SetEnabled(false);
        musicOption->SetVisible(false);
        toolTip->Hide();

        m_keyTestEnabled = false;
        m_keyChannel = Chart::Channel::Note1;
        keySelect->SetFrame(0);

        Invalidate();
    }

    void OptionDialog::OnDismissed(Parent& parent)
    {
        Dialog::OnDismissed(parent);

        const auto keyTab        = FindChild<Cx::RadioButton>(Resource::Option::IDC_BUTTON_KEY_TAB);
        const auto sfxNavigation = m_parent->Instantiate<sf::Sound>(Sound::Effects::EF_01);

        m_mixer.Stop(Sound::Channel::BGTest);
        m_mixer.Stop(Sound::Channel::EFTest);
        if (keyTab->IsChecked())
            m_mixer.Resume(Sound::Channel::BGM);
        else
            m_mixer.Play(Sound::Channel::BGM);

        m_mixer.Play(*sfxNavigation, Sound::Channel::SFX);
    }

    void OptionDialog::Update(const sf::Time& delta)
    {
        UiContainer::Update(delta);
    }

    void OptionDialog::OnKeyPressed(const sf::Event::KeyPressed& ev)
    {
        Dialog::OnKeyPressed(ev);

        const auto gameOption = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_GAME_OPTION);
        const auto keySelect  = gameOption->FindChild<Image>(Resource::Option::IDC_IMAGE_KEY_SELECT);

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
            const auto gameOption = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_GAME_OPTION);
            const auto keySelect  = gameOption->FindChild<Image>(Resource::Option::IDC_IMAGE_KEY_SELECT);

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
        const auto toolTip = FindChild<Cx::ToolTip>(Resource::Option::IDC_TOOLTIP_INFO);
        for (auto [channel, key] : m_tempConfig.KeyBindings[KeyMode::Seven])
        {
            if (key == sf::Keyboard::Key::Unknown)
            {
                toolTip->SetString(Constants::Messages::Option::KeySetting::INCOMPLETE);
                toolTip->Show(*this);

                return false;
            }

            for (auto [pairChannel, pairKey] : m_tempConfig.KeyBindings[KeyMode::Seven])
            {
                if (channel != pairChannel && key == pairKey)
                {
                    toolTip->SetString(Constants::Messages::Option::KeySetting::DUPLICATED);
                    toolTip->Show(*this);

                    return false;
                }
            }
        }

        return true;
    }

    void OptionDialog::Invalidate()
    {
        Dialog::Invalidate();

        if (!IsShown())
            return;

        const auto gameOption  = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_GAME_OPTION);
        const auto musicOption = FindChild<Cx::UiContainer>(Resource::Option::IDC_CONTAINER_MUSIC_OPTION);

        const auto gfxToggleButton     = gameOption->FindChild<Cx::ToggleButton>(Resource::Option::IDC_TOGGLE_GFX);
        const auto cursorToggleButton  = gameOption->FindChild<Cx::ToggleButton>(Resource::Option::IDC_TOGGLE_CURSOR);
        const auto keyTestToggleButton = gameOption->FindChild<Cx::ToggleButton>(Resource::Option::IDC_TOGGLE_KEY_TEST);
        const auto keySelect           = gameOption->FindChild<Image>(Resource::Option::IDC_IMAGE_KEY_SELECT);
        const auto bgmToggleButton     = musicOption->FindChild<Cx::ToggleButton>(Resource::Option::IDC_TOGGLE_BGM);
        const auto masterVolumeGauge   = musicOption->FindChild<Gauge>(Resource::Option::IDC_GAUGE_MASTER_VOLUME);
        const auto musicVolumeGauge    = musicOption->FindChild<Gauge>(Resource::Option::IDC_GAUGE_MUSIC_VOLUME);
        const auto effectVolumeGauge   = musicOption->FindChild<Gauge>(Resource::Option::IDC_GAUGE_SOUND_VOLUME);

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
}
