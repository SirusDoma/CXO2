#include <O2/States/Components/Common/OptionDialog.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/ProgressBar.hpp>

#include <magic_enum.hpp>
#include <iostream>

void OptionDialog::Initialize(Gx::Scene &scene)
{
    auto app    = &scene.GetApplication();
    auto config = app->GetConfig<GameConfig>();
    m_config    = GameConfig(config);

    auto sfxTab     = scene.Create<sf::Sound>("Interface/Metadata/Dialog/Option/Sound/Tab.json", Gx::ResourceScope::Shared);
    auto btnDefault = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_Default.json");
    auto btnSave    = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_Save.json");
    AddChild(btnDefault, btnSave);

    m_background = scene.Create<Gx::Image>("Interface/Metadata/Dialog/Option/Background.json");
    SetTexCoords(m_background->GetFrame("KeyOption")->TexCoords);

    auto gfxCheckBox     = scene.Create<Gx::CheckBox>("Interface/Metadata/Dialog/Option/3dCheckBox.json");
    gfxCheckBox->SetCheckedState(config.Use3D);
    auto cursorCheckBox  = scene.Create<Gx::CheckBox>("Interface/Metadata/Dialog/Option/CursorCheckBox.json");
    cursorCheckBox->SetCheckedState(config.UseWindowCursor);
    auto keyTestCheckBox = scene.Create<Gx::CheckBox>("Interface/Metadata/Dialog/Option/Btn_KeyTest.json");
    keyTestCheckBox->SetCheckStateChangeCallback([=] (auto sender) { m_keyTestEnabled = sender->IsChecked(); });
    m_keyOptionContainer.AddChild(gfxCheckBox, cursorCheckBox, keyTestCheckBox);

    m_keySelect = scene.Create<Gx::Image>("Interface/Metadata/Dialog/Option/KeySelect.json");
    m_keyOptionContainer.AddChild(m_keySelect);

    auto keybar = scene.Create<Gx::Image>("Interface/Metadata/Dialog/Option/KeyBar.json");
    m_keySelect->AddChild(keybar);

    m_keyChannel = EventChannel::Note1;
    for (unsigned int i = 1; i <= 7; i++)
    {
        auto channel = static_cast<EventChannel>(i + 1);
        auto keytext = scene.Create<Gx::Image>("Interface/Metadata/Dialog/Option/KeyText" + std::to_string(i) + ".json");
        keytext->SetFrame(std::string(magic_enum::enum_name(config.SevenKeyBinding[channel])));
        keytext->SetClickCallback([=] (auto &sender, auto &ev)
        {
            m_keyChannel = channel;
            m_keySelect->SetFrame(i - 1);
        });

        m_keyTexts[channel] = keytext;
        m_keyOptionContainer.AddChild(keytext);
    }

    auto bgCheckBox = scene.Create<Gx::CheckBox>("Interface/Metadata/Dialog/Option/BgCheckBox.json");
    bgCheckBox->SetCheckedState(config.UseBGM);

    auto allSoundBar    = scene.Create<Gx::ProgressBar>("Interface/Metadata/Dialog/Option/AllSoundBar.json");
    auto musicBar       = scene.Create<Gx::ProgressBar>("Interface/Metadata/Dialog/Option/MusicBar.json");
    auto soundEffectBar = scene.Create<Gx::ProgressBar>("Interface/Metadata/Dialog/Option/SoundEffectBar.json");

    auto btnAllSoundUp   = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_AllSoundUp.json");
    auto btnAllSoundDown = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_AllSoundDown.json");

    auto btnMusicUp   = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_MusicUp.json");
    auto btnMusicDown = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_MusicDown.json");

    auto btnSoundEffectUp   = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_SoundEffectUp.json");
    auto btnSoundEffectDown = scene.Create<Gx::Button>("Interface/Metadata/Dialog/Option/Btn_SoundEffectDown.json");

    allSoundBar->SetValue(100.f);
    musicBar->SetValue(100.f);
    soundEffectBar->SetValue(100.f);

    m_soundOptionContainer.AddChild(bgCheckBox, allSoundBar, musicBar, soundEffectBar,
        btnAllSoundUp, btnAllSoundDown, btnMusicUp, btnMusicDown, btnSoundEffectUp, btnSoundEffectDown);

    AddChild(&m_keyOptionContainer, &m_soundOptionContainer);

    auto keyTab   = scene.Create<Gx::RadioButton>("Interface/Metadata/Dialog/Option/Btn_KeyTab.json");
    auto soundTab = scene.Create<Gx::RadioButton>("Interface/Metadata/Dialog/Option/Btn_SoundTab.json");

    keyTab->SetCheckStateChangeCallback([=] (auto sender)
    {
        if (!sender->IsChecked())
            return;

        sfxTab->play();
        SetTexCoords(m_background->GetFrame("KeyOption")->TexCoords);

        m_keyOptionContainer.SetEnabled(true);
        m_keyOptionContainer.SetVisible(true);

        m_soundOptionContainer.SetEnabled(false);
        m_soundOptionContainer.SetVisible(false);
    });
    soundTab->SetCheckStateChangeCallback([=] (auto sender)
    {
        if (!sender->IsChecked())
            return;

        sfxTab->play();
        SetTexCoords(m_background->GetFrame("SoundOption")->TexCoords);

        m_keyOptionContainer.SetEnabled(false);
        m_keyOptionContainer.SetVisible(false);

        m_soundOptionContainer.SetEnabled(true);
        m_soundOptionContainer.SetVisible(true);
    });

    SetTexCoords(m_background->GetFrame("KeyOption")->TexCoords);

    m_keyOptionContainer.SetEnabled(true);
    m_keyOptionContainer.SetVisible(true);

    m_soundOptionContainer.SetEnabled(false);
    m_soundOptionContainer.SetVisible(false);

    AddChild(keyTab, soundTab);
}

void OptionDialog::Update(double delta)
{
    UiContainer::Update(delta);
}

void OptionDialog::OnKeyDown(sf::Event::KeyEvent ev)
{
    Inputable::OnKeyDown(ev);

    if (m_keyOptionContainer.IsEnabled() && !m_keyTestEnabled)
    {
        auto keytext = m_keyTexts[m_keyChannel];
        auto keyStr  = std::string(magic_enum::enum_name(ev.code));

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
        m_keySelect->SetFrame(index - 2);
    }
}

void OptionDialog::OnKeyType(sf::Event::TextEvent ev)
{
    Inputable::OnKeyType(ev);
}

