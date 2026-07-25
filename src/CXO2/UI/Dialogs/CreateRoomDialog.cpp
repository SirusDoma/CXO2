#include <CXO2/UI/Dialogs/CreateRoomDialog.hpp>
#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/States/State.hpp>

#include <CXO2/Models/Game.hpp>

#include <CXO2/Constants/Identifiers/Sound.hpp>
#include <CXO2/Constants/Identifiers/Room.hpp>
#include <CXO2/Constants/Messages/Room.hpp>
#include <CXO2/Utilities/StringFormatter.hpp>

#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/InputField.hpp>
#include <Genode/UI/ToggleButton.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/ToolTip.hpp>

#include <fmt/format.h>

namespace Cx
{
    using namespace Constants::Identifiers;

    CreateRoomDialog::CreateRoomDialog(SessionContext& session, Gx::AudioMixer& mixer) :
        m_session(session),
        m_mixer(mixer)
    {
    }

    void CreateRoomDialog::Initialize()
    {
        Dialog::Initialize();

        if (m_initialized)
            return;

        const auto parent   = dynamic_cast<Cx::State*>(GetPresentableParent());
        const auto sfxClick = parent->Instantiate<sf::Sound>(Sound::Effects::EF_10);

        const auto titleInput    = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_TITLE);
        const auto passwordInput = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_PASSWORD);

        const auto jamModeButton    = FindChild<Gx::RadioButton>(Resource::Room::CreateRoom::IDC_RADIO_JAM_MODE);
        const auto versusModeButton = FindChild<Gx::RadioButton>(Resource::Room::CreateRoom::IDC_RADIO_VERSUS_MODE);
        const auto singleModeButton = FindChild<Gx::RadioButton>(Resource::Room::CreateRoom::IDC_RADIO_SINGLE_MODE);

        const auto jamAnimation    = jamModeButton->FindChild<Gx::Animation>(Resource::Room::CreateRoom::IDC_ANIMATION_JAM);
        const auto versusAnimation = versusModeButton->FindChild<Gx::Animation>(Resource::Room::CreateRoom::IDC_ANIMATION_VERSUS);
        const auto singleAnimation = singleModeButton->FindChild<Gx::Animation>(Resource::Room::CreateRoom::IDC_ANIMATION_SINGLE);

        const auto levelLimitToggleButton = FindChild<Gx::ToggleButton>(Resource::Room::CreateRoom::IDC_TOGGLE_LEVEL_LIMIT);
        const auto minLevelLimitInput     = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_MIN_LEVEL_LIMIT);
        const auto maxLevelLimitInput     = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_MAX_LEVEL_LIMIT);

        titleInput->SetMaximumTextLength(21);

        passwordInput->SetMasked(true);
        passwordInput->SetMaximumTextLength(12);

        levelLimitToggleButton->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnLevelLimitToggleCheckChanged(sender, ev); });

        minLevelLimitInput->SetNumericModeEnabled(true);
        maxLevelLimitInput->SetNumericModeEnabled(true);

        jamModeButton->SetClickCallback([this] (auto& sender, auto& ev) { OnJamModeButtonClicked(sender, ev); });
        jamModeButton->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnJamModeButtonCheckChanged(sender, ev); });
        jamAnimation->SetAnimationCallback([=] (const Gx::Animation& sender)
        {
            jamAnimation->SetVisible(sender.GetState() == Gx::Animation::AnimationState::Initial || sender.GetState() == Gx::Animation::AnimationState::Playing);
        });

        versusModeButton->SetClickCallback([this] (auto& sender, auto& ev) { OnVersusModeButtonClicked(sender, ev); });
        versusModeButton->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnVersusModeButtonCheckChanged(sender, ev); });
        versusAnimation->SetAnimationCallback([=] (const Gx::Animation& sender)
        {
            versusAnimation->SetVisible(sender.GetState() == Gx::Animation::AnimationState::Initial || sender.GetState() == Gx::Animation::AnimationState::Playing);
        });

        singleModeButton->SetClickCallback([this] (auto& sender, auto& ev) { OnSingleModeButtonClicked(sender, ev); });
        singleModeButton->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnSingleModeButtonCheckChanged(sender, ev); });
        singleAnimation->SetAnimationCallback([=] (const Gx::Animation& sender)
        {
            singleAnimation->SetVisible(sender.GetState() == Gx::Animation::AnimationState::Initial || sender.GetState() == Gx::Animation::AnimationState::Playing);
        });

        m_initialized = true;
    }

    void CreateRoomDialog::OnJamModeButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto parent   = dynamic_cast<Cx::State*>(GetPresentableParent());
        const auto sfxClick = parent->Instantiate<sf::Sound>(Sound::Effects::EF_10);

        m_mixer.Play(*sfxClick, Sound::Channel::SFX);
    }

    void CreateRoomDialog::OnVersusModeButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto parent   = dynamic_cast<Cx::State*>(GetPresentableParent());
        const auto sfxClick = parent->Instantiate<sf::Sound>(Sound::Effects::EF_10);

        m_mixer.Play(*sfxClick, Sound::Channel::SFX);
    }

    void CreateRoomDialog::OnSingleModeButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto parent   = dynamic_cast<Cx::State*>(GetPresentableParent());
        const auto sfxClick = parent->Instantiate<sf::Sound>(Sound::Effects::EF_10);

        m_mixer.Play(*sfxClick, Sound::Channel::SFX);
    }

    void CreateRoomDialog::OnJamModeButtonCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev)
    {
        if (!sender.IsChecked())
            return;

        const auto jamModeButton    = FindChild<Gx::RadioButton>(Resource::Room::CreateRoom::IDC_RADIO_JAM_MODE);
        const auto versusModeButton = FindChild<Gx::RadioButton>(Resource::Room::CreateRoom::IDC_RADIO_VERSUS_MODE);
        const auto jamAnimation     = jamModeButton->FindChild<Gx::Animation>(Resource::Room::CreateRoom::IDC_ANIMATION_JAM);
        const auto toolTip          = FindChild<Gx::ToolTip>(Resource::Room::CreateRoom::IDC_TOOLTIP_INFO);

        if (jamAnimation->GetState() != Gx::Animation::AnimationState::Playing)
        {
            jamAnimation->Reset();
            jamAnimation->SetVisible(true);
        }

        toolTip->SetString(Constants::Messages::Room::CreateRoom::JAM_MODE_UNAVAILABLE);
        toolTip->Show(*this);

        jamModeButton->SetCheckedState(false);
        versusModeButton->SetCheckedState(true);
    }

    void CreateRoomDialog::OnVersusModeButtonCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev)
    {
        const auto versusModeButton = FindChild<Gx::RadioButton>(Resource::Room::CreateRoom::IDC_RADIO_VERSUS_MODE);
        const auto versusAnimation  = versusModeButton->FindChild<Gx::Animation>(Resource::Room::CreateRoom::IDC_ANIMATION_VERSUS);

        versusAnimation->Reset();
        versusAnimation->SetRepeatCount(sender.IsChecked() ? 3 : 1);
    }

    void CreateRoomDialog::OnSingleModeButtonCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev)
    {
        const auto singleModeButton       = FindChild<Gx::RadioButton>(Resource::Room::CreateRoom::IDC_RADIO_SINGLE_MODE);
        const auto singleAnimation        = singleModeButton->FindChild<Gx::Animation>(Resource::Room::CreateRoom::IDC_ANIMATION_SINGLE);
        const auto levelLimitToggleButton = FindChild<Gx::ToggleButton>(Resource::Room::CreateRoom::IDC_TOGGLE_LEVEL_LIMIT);
        const auto passwordInput          = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_PASSWORD);

        singleAnimation->Reset();
        singleAnimation->SetRepeatCount(sender.IsChecked() ? 3 : 1);

        levelLimitToggleButton->SetEnabled(!sender.IsChecked());
        if (sender.IsChecked())
            levelLimitToggleButton->SetCheckedState(false);

        passwordInput->SetEnabled(!sender.IsChecked());
        if (!passwordInput->IsEnabled())
            passwordInput->SetString("");
    }

    void CreateRoomDialog::OnLevelLimitToggleCheckChanged(Gx::ToggleButton& sender, Gx::Control::Event& ev)
    {
        const auto minLevelLimitInput = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_MIN_LEVEL_LIMIT);
        const auto maxLevelLimitInput = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_MAX_LEVEL_LIMIT);

        minLevelLimitInput->SetEnabled(sender.IsChecked());
        maxLevelLimitInput->SetEnabled(sender.IsChecked());

        if (!sender.IsChecked())
        {
            minLevelLimitInput->SetString("");
            maxLevelLimitInput->SetString("");
        }
        else if (GetRoomMode() == GameMode::Single)
        {
            sender.SetCheckedState(false);
            
            const auto parent = dynamic_cast<Cx::State*>(GetPresentableParent());
            parent->ShowDialog(Constants::Messages::Room::CreateRoom::LEVEL_LIMIT_IN_SINGLE, DialogStyle::Information);
        }
    }

    void CreateRoomDialog::OnPresented(Parent& parent, const Gx::PresentationContext& context)
    {
        Dialog::OnPresented(parent, context);

        Initialize();

        const auto titleInput    = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_TITLE);
        const auto passwordInput = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_PASSWORD);

        const auto jamModeButton    = FindChild<Gx::RadioButton>(Resource::Room::CreateRoom::IDC_RADIO_JAM_MODE);
        const auto singleModeButton = FindChild<Gx::RadioButton>(Resource::Room::CreateRoom::IDC_RADIO_SINGLE_MODE);
        const auto versusModeButton = FindChild<Gx::RadioButton>(Resource::Room::CreateRoom::IDC_RADIO_VERSUS_MODE);

        const auto jamAnimation    = jamModeButton->FindChild<Gx::Animation>(Resource::Room::CreateRoom::IDC_ANIMATION_JAM);
        const auto singleAnimation = singleModeButton->FindChild<Gx::Animation>(Resource::Room::CreateRoom::IDC_ANIMATION_SINGLE);
        const auto versusAnimation = versusModeButton->FindChild<Gx::Animation>(Resource::Room::CreateRoom::IDC_ANIMATION_VERSUS);

        const auto levelLimitToggleButton  = FindChild<Gx::ToggleButton>(Resource::Room::CreateRoom::IDC_TOGGLE_LEVEL_LIMIT);
        const auto minLevelLimitInput      = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_MIN_LEVEL_LIMIT);
        const auto maxLevelLimitInput      = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_MAX_LEVEL_LIMIT);

        const auto toolTip = FindChild<Gx::ToolTip>(Resource::Room::CreateRoom::IDC_TOOLTIP_INFO);

        titleInput->SetString(fmt::format(U"{}{}", m_session.GetName(), Constants::Messages::Room::CreateRoom::DEFAULT_TITLE_SUFFIX));
        titleInput->SelectAll();
        passwordInput->SetString("");

        jamModeButton->SetCheckedState(false);
        singleModeButton->SetCheckedState(false);
        versusModeButton->SetCheckedState(true);

        jamAnimation->Stop();
        singleAnimation->Stop();
        versusAnimation->SetRepeatCount(1);
        versusAnimation->Reset();

        levelLimitToggleButton->SetCheckedState(false);
        minLevelLimitInput->SetEnabled(false);
        maxLevelLimitInput->SetEnabled(false);
        minLevelLimitInput->SetString("");
        maxLevelLimitInput->SetString("");

        toolTip->Hide();
    }

    void CreateRoomDialog::OnAccepted()
    {
        const auto titleInput = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_TITLE);
        auto passwordInput    = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_PASSWORD);

        const auto levelLimitToggleButton   = FindChild<Gx::ToggleButton>(Resource::Room::CreateRoom::IDC_TOGGLE_LEVEL_LIMIT);
        const auto minLevelLimitInput = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_MIN_LEVEL_LIMIT);
        const auto maxLevelLimitInput = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_MAX_LEVEL_LIMIT);

        const auto toolTip = FindChild<Gx::ToolTip>(Resource::Room::CreateRoom::IDC_TOOLTIP_INFO);

        if (titleInput->GetString().isEmpty())
        {
            toolTip->SetString(Constants::Messages::Room::CreateRoom::TITLE_REQUIRED);
            toolTip->Show(*this);
            return;
        }

        if (levelLimitToggleButton->IsChecked())
        {
            if (minLevelLimitInput->GetString().isEmpty() || maxLevelLimitInput->GetString().isEmpty())
            {
                toolTip->SetString(Constants::Messages::Room::CreateRoom::LEVEL_LIMIT_REQUIRED);
                toolTip->Show(*this);

                return;
            }

            toolTip->SetString("");
            const unsigned int min = std::stoi(minLevelLimitInput->GetString().toAnsiString());
            const unsigned int max = std::stoi(maxLevelLimitInput->GetString().toAnsiString());

            if (min > 100 || max > 100)
                toolTip->SetString(Constants::Messages::Room::CreateRoom::LEVEL_OUT_OF_RANGE);
            else if (min > max)
                toolTip->SetString(Constants::Messages::Room::CreateRoom::HIGH_LEVEL_TOO_LOW);

            if (!toolTip->GetString().isEmpty())
            {
                toolTip->Show(*this);
                return;
            }
        }

        Dialog::OnAccepted();
    }

    GameMode CreateRoomDialog::GetRoomMode() const
    {
        const auto jamModeButton    = FindChild<Gx::RadioButton>(Resource::Room::CreateRoom::IDC_RADIO_JAM_MODE);
        const auto singleModeButton = FindChild<Gx::RadioButton>(Resource::Room::CreateRoom::IDC_RADIO_SINGLE_MODE);
        auto versusModeButton       = FindChild<Gx::RadioButton>(Resource::Room::CreateRoom::IDC_RADIO_VERSUS_MODE);

        if (jamModeButton && jamModeButton->IsChecked())
            return GameMode::Jam;

        if (singleModeButton && singleModeButton->IsChecked())
            return GameMode::Single;

        return GameMode::Versus;
    }

    sf::String CreateRoomDialog::GetRoomName() const
    {
        if (const auto titleInput = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_TITLE); titleInput)
            return titleInput->GetString();

        return {};
    }

    std::string CreateRoomDialog::GetRoomPassword() const
    {
        if (const auto passwordInput = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_PASSWORD); passwordInput)
            return passwordInput->GetString().toAnsiString();

        return {};
    }

    unsigned int CreateRoomDialog::GetMinLevelLimit() const
    {
        const auto levelLimitToggleButton = FindChild<Gx::ToggleButton>(Resource::Room::CreateRoom::IDC_TOGGLE_LEVEL_LIMIT);
        const auto minLevelLimitInput     = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_MIN_LEVEL_LIMIT);

        if (minLevelLimitInput && levelLimitToggleButton && levelLimitToggleButton->IsChecked())
            return std::stoi(minLevelLimitInput->GetString().toAnsiString());

        return 0;
    }

    unsigned int CreateRoomDialog::GetMaxLevelLimit() const
    {
        const auto levelLimitToggleButton = FindChild<Gx::ToggleButton>(Resource::Room::CreateRoom::IDC_TOGGLE_LEVEL_LIMIT);
        const auto maxLevelLimitInput     = FindChild<Gx::InputField>(Resource::Room::CreateRoom::IDC_EDIT_MAX_LEVEL_LIMIT);

        if (maxLevelLimitInput && levelLimitToggleButton && levelLimitToggleButton->IsChecked())
            return std::stoi(maxLevelLimitInput->GetString().toAnsiString());

        return 0;
    }
}
