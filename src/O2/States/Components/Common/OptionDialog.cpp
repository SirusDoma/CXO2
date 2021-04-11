#include <O2/States/Components/Common/OptionDialog.hpp>

#include <Genode/UI/Button.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/ProgressBar.hpp>

void OptionDialog::Initialize(Gx::Scene &scene)
{
    auto sfxTab = scene.Create<sf::Sound>("Metadata/Dialog/Option/Sound/Tab.json", Gx::ResourceScope::Shared);

    auto btnDefault = scene.Create<Gx::Button>("Metadata/Dialog/Option/Btn_Default.json");
    auto btnSave    = scene.Create<Gx::Button>("Metadata/Dialog/Option/Btn_Save.json");
    AddChild(btnDefault, btnSave);

    m_background = scene.Create<Gx::Image>("Metadata/Dialog/Option/Background.json");
    SetTexCoords(m_background->GetFrame("KeyOption"));

    auto gfxCheckBox     = scene.Create<Gx::CheckBox>("Metadata/Dialog/Option/3dCheckBox.json");
    auto cursorCheckBox  = scene.Create<Gx::CheckBox>("Metadata/Dialog/Option/CursorCheckBox.json");
    auto keyTestCheckBox = scene.Create<Gx::CheckBox>("Metadata/Dialog/Option/Btn_KeyTest.json");
    m_keyOptionContainer.AddChild(gfxCheckBox, cursorCheckBox, keyTestCheckBox);

    auto bgCheckBox     = scene.Create<Gx::CheckBox>("Metadata/Dialog/Option/BgCheckBox.json");
    auto allSoundBar    = scene.Create<Gx::ProgressBar>("Metadata/Dialog/Option/AllSoundBar.json");
    auto musicBar       = scene.Create<Gx::ProgressBar>("Metadata/Dialog/Option/MusicBar.json");
    auto soundEffectBar = scene.Create<Gx::ProgressBar>("Metadata/Dialog/Option/SoundEffectBar.json");

    auto btnAllSoundUp   = scene.Create<Gx::Button>("Metadata/Dialog/Option/Btn_AllSoundUp.json");
    auto btnAllSoundDown = scene.Create<Gx::Button>("Metadata/Dialog/Option/Btn_AllSoundDown.json");

    auto btnMusicUp   = scene.Create<Gx::Button>("Metadata/Dialog/Option/Btn_MusicUp.json");
    auto btnMusicDown = scene.Create<Gx::Button>("Metadata/Dialog/Option/Btn_MusicDown.json");

    auto btnSoundEffectUp   = scene.Create<Gx::Button>("Metadata/Dialog/Option/Btn_SoundEffectUp.json");
    auto btnSoundEffectDown = scene.Create<Gx::Button>("Metadata/Dialog/Option/Btn_SoundEffectDown.json");

    allSoundBar->SetValue(100.f);
    musicBar->SetValue(100.f);
    soundEffectBar->SetValue(100.f);

    m_soundOptionContainer.AddChild(bgCheckBox, allSoundBar, musicBar, soundEffectBar,
        btnAllSoundUp, btnAllSoundDown, btnMusicUp, btnMusicDown, btnSoundEffectUp, btnSoundEffectDown);

    AddChild(&m_keyOptionContainer, &m_soundOptionContainer);

    auto keyTab   = scene.Create<Gx::RadioButton>("Metadata/Dialog/Option/Btn_KeyTab.json");
    auto soundTab = scene.Create<Gx::RadioButton>("Metadata/Dialog/Option/Btn_SoundTab.json");

    keyTab->SetCheckStateChangeCallback([=] (auto sender)
    {
        if (!sender->IsChecked())
            return;

        sfxTab->play();
        SetTexCoords(m_background->GetFrame("KeyOption"));

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
        SetTexCoords(m_background->GetFrame("SoundOption"));

        m_keyOptionContainer.SetEnabled(false);
        m_keyOptionContainer.SetVisible(false);

        m_soundOptionContainer.SetEnabled(true);
        m_soundOptionContainer.SetVisible(true);
    });

    SetTexCoords(m_background->GetFrame("KeyOption"));

    m_keyOptionContainer.SetEnabled(true);
    m_keyOptionContainer.SetVisible(true);

    m_soundOptionContainer.SetEnabled(false);
    m_soundOptionContainer.SetVisible(false);

    AddChild(keyTab, soundTab);
}

