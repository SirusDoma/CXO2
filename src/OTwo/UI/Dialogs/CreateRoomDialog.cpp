#include <OTwo/UI/Dialogs/CreateRoomDialog.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/States/State.hpp>

#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/TextBox.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/ToolTip.hpp>

CreateRoomDialog::CreateRoomDialog(const Gx::Dialog &copy) :
    Gx::Node(copy),
    Gx::Dialog(copy),
    m_initialized(false)
{
}

void CreateRoomDialog::Initialize()
{
    if (m_initialized)
        return;

    const auto parent = GetParent<::State>();
    auto& mixer  = parent->Require<Gx::Mixer>();
    auto session = parent->Require<SessionContext>();

    const auto sfxClick = parent->Instantiate<sf::Sound>("IDC_DIALOG_CREATE_ROOM/IDC_SOUND_CLICK");

    const auto titleTextBox    = FindChild<Gx::TextBox>("IDC_EDIT_TITLE");
    const auto passwordTextBox = FindChild<Gx::TextBox>("IDC_EDIT_PASSWORD");

    const auto jamModeButton    = FindChild<Gx::RadioButton>("IDC_RADIO_JAM_MODE");
    const auto versusModeButton = FindChild<Gx::RadioButton>("IDC_RADIO_VERSUS_MODE");
    const auto singleModeButton = FindChild<Gx::RadioButton>("IDC_RADIO_SINGLE_MODE");

    const auto jamAnimation    = jamModeButton->FindChild<Gx::Animation>("IDC_ANIMATION_JAM");
    const auto versusAnimation = versusModeButton->FindChild<Gx::Animation>("IDC_ANIMATION_VERSUS");
    const auto singleAnimation = singleModeButton->FindChild<Gx::Animation>("IDC_ANIMATION_SINGLE");

    const auto levelLimitCheckBox   = FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_LEVEL_LIMIT");
    const auto minLevelLimitTextBox = FindChild<Gx::TextBox>("IDC_EDIT_MIN_LEVEL_LIMIT");
    const auto maxLevelLimitTextBox = FindChild<Gx::TextBox>("IDC_EDIT_MAX_LEVEL_LIMIT");

    const auto toolTip = FindChild<Gx::ToolTip>("IDC_TOOLTIP_INFO");

    titleTextBox->SetMaximumTextLength(21);

    passwordTextBox->SetMasked(true);
    passwordTextBox->SetMaximumTextLength(12);

    levelLimitCheckBox->SetCheckStateChangeCallback([=] (auto sender)
    {
        minLevelLimitTextBox->SetEnabled(sender->IsChecked());
        maxLevelLimitTextBox->SetEnabled(sender->IsChecked());

        if (!sender->IsChecked())
        {
            minLevelLimitTextBox->SetString("");
            maxLevelLimitTextBox->SetString("");
        }
    });

    minLevelLimitTextBox->SetNumericModeEnabled(true);
    maxLevelLimitTextBox->SetNumericModeEnabled(true);

    jamModeButton->SetClickCallback([=, &mixer] (auto& sender, auto& ev) {
        mixer.Play(sfxClick, "SFX");
    });

    jamModeButton->SetCheckStateChangeCallback([=] (auto sender)
    {
        if (!sender->IsChecked())
            return;

        if (jamAnimation->GetState() != Gx::Animation::AnimationState::Playing)
        {
            jamAnimation->Reset();
            jamAnimation->SetVisible(true);
        }

        toolTip->SetString("JAM Mode is not available.");
        toolTip->Show(this);

        jamModeButton->SetCheckedState(false);
        versusModeButton->SetCheckedState(true);
    });

    jamAnimation->SetAnimationCallback([=] (const Gx::Animation& sender) {
        jamAnimation->SetVisible(sender.GetState() == Gx::Animation::AnimationState::Initial || sender.GetState() == Gx::Animation::AnimationState::Playing);
    });

    versusModeButton->SetClickCallback([=, &mixer] (auto& sender, auto& ev) {
        mixer.Play(sfxClick, "SFX");
    });

    versusModeButton->SetCheckStateChangeCallback([=] (auto sender)
    {
        versusAnimation->Reset();
        versusAnimation->SetRepeatCount(sender->IsChecked() ? 3 : 1);
    });

    versusAnimation->SetAnimationCallback([=] (const Gx::Animation& sender) {
        versusAnimation->SetVisible(sender.GetState() == Gx::Animation::AnimationState::Initial || sender.GetState() == Gx::Animation::AnimationState::Playing);
    });

    singleModeButton->SetClickCallback([=, &mixer] (auto& sender, auto& ev) {
        mixer.Play(sfxClick, "SFX");
    });

    singleModeButton->SetCheckStateChangeCallback([=] (auto sender)
    {
        singleAnimation->Reset();
        singleAnimation->SetRepeatCount(sender->IsChecked() ? 3 : 1);

        levelLimitCheckBox->SetEnabled(!sender->IsChecked());
        if (sender->IsChecked())
            levelLimitCheckBox->SetCheckedState(false);

        passwordTextBox->SetEnabled(!sender->IsChecked());
        if (!passwordTextBox->IsEnabled())
            passwordTextBox->SetString("");
    });

    singleAnimation->SetAnimationCallback([=] (const Gx::Animation& sender) {
        singleAnimation->SetVisible(sender.GetState() == Gx::Animation::AnimationState::Initial || sender.GetState() == Gx::Animation::AnimationState::Playing);
    });

    m_initialized = true;
}

void CreateRoomDialog::OnShown(Gx::Scene &scene)
{
    Dialog::OnShown(scene);

    Initialize();

    const auto parent = GetParent<::State>();
    auto& session = parent->Require<SessionContext>();

    const auto titleTextBox    = FindChild<Gx::TextBox>("IDC_EDIT_TITLE");
    const auto passwordTextBox = FindChild<Gx::TextBox>("IDC_EDIT_PASSWORD");

    const auto jamModeButton    = FindChild<Gx::RadioButton>("IDC_RADIO_JAM_MODE");
    const auto singleModeButton = FindChild<Gx::RadioButton>("IDC_RADIO_SINGLE_MODE");
    const auto versusModeButton = FindChild<Gx::RadioButton>("IDC_RADIO_VERSUS_MODE");

    const auto jamAnimation    = jamModeButton->FindChild<Gx::Animation>("IDC_ANIMATION_JAM");
    const auto singleAnimation = singleModeButton->FindChild<Gx::Animation>("IDC_ANIMATION_SINGLE");
    const auto versusAnimation = versusModeButton->FindChild<Gx::Animation>("IDC_ANIMATION_VERSUS");

    const auto levelLimitCheckBox   = FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_LEVEL_LIMIT");
    const auto minLevelLimitTextBox = FindChild<Gx::TextBox>("IDC_EDIT_MIN_LEVEL_LIMIT");
    const auto maxLevelLimitTextBox = FindChild<Gx::TextBox>("IDC_EDIT_MAX_LEVEL_LIMIT");

    const auto toolTip = FindChild<Gx::ToolTip>("IDC_TOOLTIP_INFO");

    titleTextBox->SetString(session.GetCurrentPlayer().Name + "'s Room");
    titleTextBox->SelectAll();
    passwordTextBox->SetString("");

    jamModeButton->SetCheckedState(false);
    singleModeButton->SetCheckedState(false);
    versusModeButton->SetCheckedState(true);

    jamAnimation->Stop();
    singleAnimation->Stop();
    versusAnimation->SetRepeatCount(1);
    versusAnimation->Reset();

    levelLimitCheckBox->SetCheckedState(false);
    minLevelLimitTextBox->SetEnabled(false);
    maxLevelLimitTextBox->SetEnabled(false);
    minLevelLimitTextBox->SetString("");
    maxLevelLimitTextBox->SetString("");

    toolTip->Hide();
}

void CreateRoomDialog::OnAccepted()
{
    const auto titleTextBox = FindChild<Gx::TextBox>("IDC_EDIT_TITLE");
    auto passwordTextBox    = FindChild<Gx::TextBox>("IDC_EDIT_PASSWORD");

    const auto levelLimitCheckBox   = FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_LEVEL_LIMIT");
    const auto minLevelLimitTextBox = FindChild<Gx::TextBox>("IDC_EDIT_MIN_LEVEL_LIMIT");
    const auto maxLevelLimitTextBox = FindChild<Gx::TextBox>("IDC_EDIT_MAX_LEVEL_LIMIT");

    const auto toolTip = FindChild<Gx::ToolTip>("IDC_TOOLTIP_INFO");

    if (titleTextBox->GetString().isEmpty())
    {
        toolTip->SetString("Please enter a room name.");
        toolTip->Show(this);
        return;
    }

    if (levelLimitCheckBox->IsChecked())
    {
        if (minLevelLimitTextBox->GetString().isEmpty() || maxLevelLimitTextBox->GetString().isEmpty())
        {
            toolTip->SetString("Please set level limit. Ex) 10 ~ 20");
            toolTip->Show(this);

            return;
        }

        toolTip->SetString("");
        const unsigned int min = std::stoi(std::string(minLevelLimitTextBox->GetString()));
        const unsigned int max = std::stoi(std::string(maxLevelLimitTextBox->GetString()));

        if (min > 100 || max > 100)
            toolTip->SetString("Wrong level selected. You can enter level range from 1 to 100");
        else if (min > max)
            toolTip->SetString("You must enter higher numbers in HIGH LEVEL");

        if (!toolTip->GetString().isEmpty())
        {
            toolTip->Show(this);
            return;
        }
    }

    Dialog::OnAccepted();
}

GameMode CreateRoomDialog::GetRoomMode() const
{
    const auto jamModeButton    = FindChild<Gx::RadioButton>("IDC_RADIO_JAM_MODE");
    const auto singleModeButton = FindChild<Gx::RadioButton>("IDC_RADIO_SINGLE_MODE");
    auto versusModeButton       = FindChild<Gx::RadioButton>("IDC_RADIO_VERSUS_MODE");

    if (jamModeButton && jamModeButton->IsChecked())
        return GameMode::Jam;

    if (singleModeButton && singleModeButton->IsChecked())
        return GameMode::Single;

    return GameMode::Versus;
}

std::string CreateRoomDialog::GetRoomName() const
{
    if (const auto titleTextBox = FindChild<Gx::TextBox>("IDC_EDIT_TITLE"); titleTextBox)
        return titleTextBox->GetString();

    return {};
}

std::string CreateRoomDialog::GetRoomPassword() const
{
    if (const auto passwordTextBox = FindChild<Gx::TextBox>("IDC_EDIT_PASSWORD"); passwordTextBox)
        return passwordTextBox->GetString();

    return {};
}

unsigned int CreateRoomDialog::GetMinLevelLimit() const
{
    const auto levelLimitCheckBox   = FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_LEVEL_LIMIT");
    const auto minLevelLimitTextBox = FindChild<Gx::TextBox>("IDC_EDIT_MIN_LEVEL_LIMIT");

    if (minLevelLimitTextBox && levelLimitCheckBox && levelLimitCheckBox->IsChecked())
        return std::stoi(std::string(minLevelLimitTextBox->GetString()));

    return 0;
}

unsigned int CreateRoomDialog::GetMaxLevelLimit() const
{
    const auto levelLimitCheckBox   = FindChild<Gx::CheckBox>("IDC_CHECKBOX_ENABLE_LEVEL_LIMIT");
    const auto maxLevelLimitTextBox = FindChild<Gx::TextBox>("IDC_EDIT_MAX_LEVEL_LIMIT");

    if (maxLevelLimitTextBox && levelLimitCheckBox && levelLimitCheckBox->IsChecked())
        return std::stoi(std::string(maxLevelLimitTextBox->GetString()));

    return 0;
}

