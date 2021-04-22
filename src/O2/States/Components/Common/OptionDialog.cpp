#include <O2/States/Components/Common/OptionDialog.hpp>

#include <Genode/Audio/Mixer.hpp>
#include <Genode/Audio/SoundGroup.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/ProgressBar.hpp>

#include <magic_enum.hpp>
#include <iostream>

void OptionDialog::Initialize(Gx::Scene &scene)
{
    auto& app = scene.GetApplication();
    m_mixer   = &app.Require<Gx::Mixer>();

    m_bgAllTest     = m_mixer->Create<sf::Music>("Interface/Metadata/Dialog/Option/SampleSong.json");
    m_bgTest        = m_mixer->Create<sf::Music>("Interface/Metadata/Dialog/Option/MusicVolume.json");
    m_efTest        = m_mixer->Create<sf::Sound>("Interface/Metadata/Dialog/Option/SampleEffect.json");
    m_sfxNavigation = m_mixer->Create<sf::Sound>("Interface/Metadata/Dialog/Option/Sound/Tab.json");

    auto btnSave = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_Save.json");
    btnSave->SetClickCallback([=, &scene] (auto &sender, auto &ev)
    {
        if (!ValidateConfig())
        {
            // TODO: Tool Tip
            return;
        }

        auto& app = scene.GetApplication();
        app.SetConfig(m_config);

        m_keyTestCheckBox->SetCheckedState(false);
        m_keyChannel = EventChannel::Note1;
        m_keySelect->SetFrame(0);

        if (auto bgGroup = m_mixer->GetSoundGroup("BGM"); bgGroup)
        {
            bgGroup->SetVolume(m_config.MusicVolume);
            bgGroup->SetEnabled(m_config.UseBGM);
        }

        if (auto sfxGroup = m_mixer->GetSoundGroup("SFX"); sfxGroup)
            sfxGroup->SetVolume(m_config.EffectVolume);

        // TODO: Add tooltip
        Invalidate();
    });

    auto btnDefault = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_Default.json");
    btnDefault->SetClickCallback([=] (auto &sender, auto &ev)
    {
        m_config.Reset();
        btnSave->PerformClick();

        // TODO: Add tooltip
    });

    AddChild(btnDefault, btnSave);

    m_background = scene.Create<Gx::Image>("Interface/Metadata/Dialog/Option/Background.json");
    SetTexCoords(m_background->GetFrame("KeyOption")->TexCoords);

    m_gfxCheckBox     = scene.Create<Gx::CheckBox>("Interface/Metadata/Dialog/Option/3dCheckBox.json");
    m_gfxCheckBox->SetCheckStateChangeCallback([=] (auto sender) { m_config.Use3D = sender->IsChecked(); });
    m_cursorCheckBox  = scene.Create<Gx::CheckBox>("Interface/Metadata/Dialog/Option/CursorCheckBox.json");
    m_cursorCheckBox->SetCheckStateChangeCallback([=] (auto sender) { m_config.UseWindowCursor = sender->IsChecked(); });
    m_keyTestCheckBox = scene.Create<Gx::CheckBox>("Interface/Metadata/Dialog/Option/Btn_KeyTest.json");
    m_keyTestCheckBox->SetCheckStateChangeCallback([=] (auto sender)
    {
        m_keyTestEnabled = sender->IsChecked();
        m_keyChannel     = EventChannel::Note1;
        m_keySelect->SetFrame(0);
        m_keySelect->SetVisible(!m_keyTestEnabled);
    });
    m_keyOptionContainer.AddChild(m_gfxCheckBox, m_cursorCheckBox, m_keyTestCheckBox);

    m_keySelect = scene.Create<Gx::Image>("Interface/Metadata/Dialog/Option/KeySelect.json");
    m_keyOptionContainer.AddChild(m_keySelect);

    auto keybar = scene.Create<Gx::Image>("Interface/Metadata/Dialog/Option/KeyBar.json");
    m_keySelect->AddChild(keybar);

    m_keyChannel = EventChannel::Note1;
    for (unsigned int i = 1; i <= 7; i++)
    {
        auto channel = static_cast<EventChannel>(i + 1);
        auto keytext = scene.Create<Gx::Image>("Interface/Metadata/Dialog/Option/KeyText" + std::to_string(i) + ".json");
        keytext->SetClickCallback([=] (auto &sender, auto &ev)
        {
            m_keyChannel = channel;
            m_keySelect->SetFrame(i - 1);
        });
        m_keyTexts[channel] = keytext;

        m_keyOptionContainer.AddChild(keytext);
    }

    for (unsigned int i = 1; i <= 7; i++)
    {
        auto keyDown = scene.Create<Gx::Image>("Interface/Metadata/Dialog/Option/KeyDown.json");
        auto channel = static_cast<EventChannel>(i + 1);

        keyDown->SetFrame("Note" + std::to_string(i));
        m_keyDowns[channel] = keyDown;

        m_keyOptionContainer.AddChild(keyDown);
    }

    m_bgmCheckBox     = scene.Create<Gx::CheckBox>("Interface/Metadata/Dialog/Option/BgmCheckBox.json");
    m_bgmCheckBox->SetCheckStateChangeCallback([=] (auto sender) { m_config.UseBGM = sender->IsChecked(); });
    m_masterVolumeBar = scene.Create<Gx::ProgressBar>("Interface/Metadata/Dialog/Option/MasterVolumeBar.json");
    m_musicVolumeBar  = scene.Create<Gx::ProgressBar>("Interface/Metadata/Dialog/Option/MusicBar.json");
    m_effectVolumeBar = scene.Create<Gx::ProgressBar>("Interface/Metadata/Dialog/Option/SoundEffectBar.json");

    auto btnMasterVolumeUp   = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_MasterVolumeUp.json");
    auto btnMasterVolumeDown = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_MasterVolumeDown.json");
    btnMasterVolumeUp->SetHoldClickCallback([=] (auto &sender, auto &ev)
    {
        m_masterVolumeBar->SetValue(m_masterVolumeBar->GetValue() + 1);
        m_musicVolumeBar->SetValue(m_masterVolumeBar->GetValue());
        m_effectVolumeBar->SetValue(m_masterVolumeBar->GetValue());

        m_config.MusicVolume  = static_cast<unsigned int>(m_masterVolumeBar->GetValue());
        m_config.EffectVolume = static_cast<unsigned int>(m_masterVolumeBar->GetValue());

        m_mixer->Pause("BGM");
        if (m_bgAllTest->getStatus() != sf::SoundSource::Playing)
        {
            m_mixer->Stop("BGTest");
            m_mixer->Stop("EFTest");
            m_mixer->Play(m_bgAllTest, "BGTest");
        }

        m_mixer->Play(m_efTest, "EFTest");
        m_mixer->GetSoundGroup("BGTest")->SetVolume(m_masterVolumeBar->GetValue());
        m_mixer->GetSoundGroup("EFTest")->SetVolume(m_masterVolumeBar->GetValue());
    });
    btnMasterVolumeDown->SetHoldClickCallback([=] (auto &sender, auto &ev)
    {
        m_masterVolumeBar->SetValue(m_masterVolumeBar->GetValue() - 1);
        m_musicVolumeBar->SetValue(m_masterVolumeBar->GetValue());
        m_effectVolumeBar->SetValue(m_masterVolumeBar->GetValue());

        m_config.MusicVolume  = static_cast<unsigned int>(m_masterVolumeBar->GetValue());
        m_config.EffectVolume = static_cast<unsigned int>(m_masterVolumeBar->GetValue());

        m_mixer->Pause("BGM");
        if (m_bgAllTest->getStatus() != sf::SoundSource::Playing)
        {
            m_mixer->Stop("BGTest");
            m_mixer->Stop("EFTest");
            m_mixer->Play(m_bgAllTest, "BGTest");
        }

        m_mixer->Play(m_efTest, "EFTest");
        m_mixer->GetSoundGroup("BGTest")->SetVolume(m_masterVolumeBar->GetValue());
        m_mixer->GetSoundGroup("EFTest")->SetVolume(m_masterVolumeBar->GetValue());
    });

    auto btnMusicUp   = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_MusicUp.json");
    auto btnMusicDown = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_MusicDown.json");

    btnMusicUp->SetHoldClickCallback([=] (auto &sender, auto &ev)
    {
        m_musicVolumeBar->SetValue(m_musicVolumeBar->GetValue() + 1);
        m_config.MusicVolume = static_cast<unsigned int>(m_musicVolumeBar->GetValue());

        m_mixer->Pause("BGM");
        if (m_bgTest->getStatus() != sf::SoundSource::Playing)
        {
            m_mixer->Stop("BGTest");
            m_mixer->Stop("EFTest");
            m_mixer->Play(m_bgTest, "BGTest");
        }

        m_mixer->GetSoundGroup("BGTest")->SetVolume(m_musicVolumeBar->GetValue());
    });
    btnMusicDown->SetHoldClickCallback([=] (auto &sender, auto &ev)
    {
        m_musicVolumeBar->SetValue(m_musicVolumeBar->GetValue() - 1);
        m_config.MusicVolume = static_cast<unsigned int>(m_musicVolumeBar->GetValue());

        m_mixer->Pause("BGM");
        if (m_bgTest->getStatus() != sf::SoundSource::Playing)
        {
            m_mixer->Stop("BGTest");
            m_mixer->Stop("EFTest");
            m_mixer->Play(m_bgTest, "BGTest");
        }

        m_mixer->GetSoundGroup("BGTest")->SetVolume(m_musicVolumeBar->GetValue());
    });

    auto btnSoundEffectUp   = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_SoundEffectUp.json");
    auto btnSoundEffectDown = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_SoundEffectDown.json");

    btnSoundEffectUp->SetHoldClickCallback([=] (auto &sender, auto &ev)
    {
        m_effectVolumeBar->SetValue(m_effectVolumeBar->GetValue() + 1);
        m_config.EffectVolume = static_cast<unsigned int>(m_effectVolumeBar->GetValue());

        m_mixer->Pause("BGM");
        if (m_efTest->getStatus() != sf::SoundSource::Playing)
        {
            m_mixer->Stop("BGTest");
            m_mixer->Stop("EFTest");
        }

        m_mixer->Play(m_efTest, "EFTest");
        m_mixer->GetSoundGroup("EFTest")->SetVolume(m_effectVolumeBar->GetValue());
    });
    btnSoundEffectDown->SetHoldClickCallback([=] (auto &sender, auto &ev)
    {
        m_effectVolumeBar->SetValue(m_effectVolumeBar->GetValue() - 1);
        m_config.EffectVolume = static_cast<unsigned int>(m_effectVolumeBar->GetValue());

        m_mixer->Pause("BGM");
        if (m_efTest->getStatus() != sf::SoundSource::Playing)
        {
            m_mixer->Stop("BGTest");
            m_mixer->Stop("EFTest");
        }

        m_mixer->Play(m_efTest, "EFTest");
        m_mixer->GetSoundGroup("EFTest")->SetVolume(m_effectVolumeBar->GetValue());
    });

    m_soundOptionContainer.AddChild(m_bgmCheckBox, m_masterVolumeBar, m_musicVolumeBar, m_effectVolumeBar,
        btnMasterVolumeUp, btnMasterVolumeDown, btnMusicUp, btnMusicDown, btnSoundEffectUp, btnSoundEffectDown);

    AddChild(&m_keyOptionContainer, &m_soundOptionContainer);

    m_keyTab   = scene.Create<Gx::RadioButton>("Interface/Metadata/Dialog/Option/Btn_KeyTab.json");
    m_soundTab = scene.Create<Gx::RadioButton>("Interface/Metadata/Dialog/Option/Btn_SoundTab.json");

    m_keyTab->SetCheckStateChangeCallback([=] (auto sender)
    {
        if (!sender->IsChecked())
            return;

        m_mixer->Play(m_sfxNavigation, "SFX");
        if (m_bgAllTest->getStatus() == sf::SoundSource::Playing || m_bgTest->getStatus() == sf::SoundSource::Playing || m_efTest->getStatus() == sf::SoundSource::Playing)
            m_mixer->Play("BGM");
        else
            m_mixer->Resume("BGM");

        m_mixer->Stop("BGTest");
        m_mixer->Stop("EFTest");

        if (auto bgGroup = m_mixer->GetSoundGroup("BGM"); bgGroup)
            bgGroup->SetVolume(m_config.MusicVolume);

        if (auto sfxGroup = m_mixer->GetSoundGroup("SFX"); sfxGroup)
            sfxGroup->SetVolume(m_config.EffectVolume);

        SetTexCoords(m_background->GetFrame("KeyOption")->TexCoords);

        m_keyOptionContainer.SetEnabled(true);
        m_keyOptionContainer.SetVisible(true);

        m_soundOptionContainer.SetEnabled(false);
        m_soundOptionContainer.SetVisible(false);
    });
    m_soundTab->SetCheckStateChangeCallback([=] (auto sender)
    {
        if (!sender->IsChecked())
            return;

        m_mixer->Play(m_sfxNavigation, "SFX");
        m_mixer->Pause("BGM");
        m_mixer->Stop("BGTest");
        m_mixer->Stop("EFTest");

        m_mixer->Play(m_bgAllTest, "BGTest");
        m_mixer->GetSoundGroup("BGTest")->SetVolume(m_config.MusicVolume);

        SetTexCoords(m_background->GetFrame("SoundOption")->TexCoords);

        m_keyOptionContainer.SetEnabled(false);
        m_keyOptionContainer.SetVisible(false);

        m_soundOptionContainer.SetEnabled(true);
        m_soundOptionContainer.SetVisible(true);
    });
    AddChild(m_keyTab, m_soundTab);
}

void OptionDialog::OnShown(Gx::Scene &scene)
{
    Dialog::OnShown(scene);

    auto& app   = scene.GetApplication();
    auto config = app.GetConfig<GameConfig>();
    m_config    = GameConfig(config);

    SetTexCoords(m_background->GetFrame("KeyOption")->TexCoords);
    m_keyTab->SetCheckedState(true);
    m_soundTab->SetCheckedState(false);
    m_keyOptionContainer.SetEnabled(true);
    m_keyOptionContainer.SetVisible(true);
    m_soundOptionContainer.SetEnabled(false);
    m_soundOptionContainer.SetVisible(false);

    m_keyTestEnabled = false;
    m_keyChannel = EventChannel::Note1;
    m_keySelect->SetFrame(0);
}

void OptionDialog::OnClose()
{
    m_mixer->Stop("BGTest");
    m_mixer->Stop("EFTest");
    if (m_keyTab->IsChecked())
        m_mixer->Resume("BGM");
    else
        m_mixer->Play("BGM");

    m_mixer->Play(m_sfxNavigation, "SFX");
}

void OptionDialog::Update(double delta)
{
    UiContainer::Update(delta);
}

void OptionDialog::OnKeyDown(sf::Event::KeyEvent ev)
{
    Dialog::OnKeyDown(ev);

    if (m_keyOptionContainer.IsEnabled())
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
            m_keySelect->SetFrame(index - static_cast<int>(EventChannel::Note1));
            m_keySelect->SetVisible(true);
        }
        else
        {
            m_keySelect->SetVisible(false);
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
        m_keySelect->SetVisible(false);
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

    m_gfxCheckBox->SetCheckedState(m_config.Use3D);
    m_cursorCheckBox->SetCheckedState(m_config.UseWindowCursor);
    m_keyTestCheckBox->SetCheckedState(m_keyTestEnabled);
    m_bgmCheckBox->SetCheckedState(m_config.UseBGM);

    m_masterVolumeBar->SetValue(m_config.MusicVolume == m_config.EffectVolume ? m_config.MusicVolume : m_masterVolumeBar->GetValue());
    m_musicVolumeBar->SetValue(m_config.MusicVolume);
    m_effectVolumeBar->SetValue(m_config.EffectVolume);

    for (auto [channel, keytext] : m_keyTexts)
        keytext->SetFrame(std::string(magic_enum::enum_name(m_config.SevenKeyBinding[channel])));

    if (!m_keyTestEnabled)
    {
        for (auto [channel, keyDown] : m_keyDowns)
            keyDown->SetVisible(false);
    }
    else
    {
        m_keySelect->SetVisible(false);
    }
}
