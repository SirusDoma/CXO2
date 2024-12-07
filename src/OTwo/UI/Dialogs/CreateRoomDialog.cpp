#include <OTwo/UI/Dialogs/CreateRoomDialog.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/States/State.hpp>

#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/InputField.hpp>
#include <Genode/UI/ToggleButton.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/ToolTip.hpp>

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

    const auto parent   = dynamic_cast<::State*>(GetPresentableParent());
    const auto sfxClick = parent->Instantiate<sf::Sound>("bgEffect/10");

    const auto titleInput    = FindChild<Gx::InputField>("IDC_EDIT_TITLE");
    const auto passwordInput = FindChild<Gx::InputField>("IDC_EDIT_PASSWORD");

    const auto jamModeButton    = FindChild<Gx::RadioButton>("IDC_RADIO_JAM_MODE");
    const auto versusModeButton = FindChild<Gx::RadioButton>("IDC_RADIO_VERSUS_MODE");
    const auto singleModeButton = FindChild<Gx::RadioButton>("IDC_RADIO_SINGLE_MODE");

    const auto jamAnimation    = jamModeButton->FindChild<Gx::Animation>("IDC_ANIMATION_JAM");
    const auto versusAnimation = versusModeButton->FindChild<Gx::Animation>("IDC_ANIMATION_VERSUS");
    const auto singleAnimation = singleModeButton->FindChild<Gx::Animation>("IDC_ANIMATION_SINGLE");

    const auto levelLimitToggleButton = FindChild<Gx::ToggleButton>("IDC_TOGGLE_LEVEL_LIMIT");
    const auto minLevelLimitInput     = FindChild<Gx::InputField>("IDC_EDIT_MIN_LEVEL_LIMIT");
    const auto maxLevelLimitInput     = FindChild<Gx::InputField>("IDC_EDIT_MAX_LEVEL_LIMIT");

    const auto toolTip = FindChild<Gx::ToolTip>("IDC_TOOLTIP_INFO");

    titleInput->SetMaximumTextLength(21);

    passwordInput->SetMasked(true);
    passwordInput->SetMaximumTextLength(12);

    levelLimitToggleButton->SetCheckStateChangeCallback([=] (auto& sender)
    {
        minLevelLimitInput->SetEnabled(sender.IsChecked());
        maxLevelLimitInput->SetEnabled(sender.IsChecked());

        if (!sender.IsChecked())
        {
            minLevelLimitInput->SetString("");
            maxLevelLimitInput->SetString("");
        }
    });

    minLevelLimitInput->SetNumericModeEnabled(true);
    maxLevelLimitInput->SetNumericModeEnabled(true);

    jamModeButton->SetClickCallback([=] (auto& sender, auto& ev) {
        m_mixer.Play(*sfxClick, "SFX");
    });

    jamModeButton->SetCheckStateChangeCallback([=] (auto& sender)
    {
        if (!sender.IsChecked())
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

    versusModeButton->SetClickCallback([=] (auto& sender, auto& ev) {
        m_mixer.Play(*sfxClick, "SFX");
    });

    versusModeButton->SetCheckStateChangeCallback([=] (auto& sender)
    {
        versusAnimation->Reset();
        versusAnimation->SetRepeatCount(sender.IsChecked() ? 3 : 1);
    });

    versusAnimation->SetAnimationCallback([=] (const Gx::Animation& sender) {
        versusAnimation->SetVisible(sender.GetState() == Gx::Animation::AnimationState::Initial || sender.GetState() == Gx::Animation::AnimationState::Playing);
    });

    singleModeButton->SetClickCallback([=] (auto& sender, auto& ev) {
        m_mixer.Play(*sfxClick, "SFX");
    });

    singleModeButton->SetCheckStateChangeCallback([=] (auto& sender)
    {
        singleAnimation->Reset();
        singleAnimation->SetRepeatCount(sender.IsChecked() ? 3 : 1);

        levelLimitToggleButton->SetEnabled(!sender.IsChecked());
        if (sender.IsChecked())
            levelLimitToggleButton->SetCheckedState(false);

        passwordInput->SetEnabled(!sender.IsChecked());
        if (!passwordInput->IsEnabled())
            passwordInput->SetString("");
    });

    singleAnimation->SetAnimationCallback([=] (const Gx::Animation& sender) {
        singleAnimation->SetVisible(sender.GetState() == Gx::Animation::AnimationState::Initial || sender.GetState() == Gx::Animation::AnimationState::Playing);
    });

    m_initialized = true;
}

void CreateRoomDialog::OnPresented(Parent& parent, const Gx::PresentationContext& context)
{
    Dialog::OnPresented(parent, context);

    Initialize();

    const auto titleInput    = FindChild<Gx::InputField>("IDC_EDIT_TITLE");
    const auto passwordInput = FindChild<Gx::InputField>("IDC_EDIT_PASSWORD");

    const auto jamModeButton    = FindChild<Gx::RadioButton>("IDC_RADIO_JAM_MODE");
    const auto singleModeButton = FindChild<Gx::RadioButton>("IDC_RADIO_SINGLE_MODE");
    const auto versusModeButton = FindChild<Gx::RadioButton>("IDC_RADIO_VERSUS_MODE");

    const auto jamAnimation    = jamModeButton->FindChild<Gx::Animation>("IDC_ANIMATION_JAM");
    const auto singleAnimation = singleModeButton->FindChild<Gx::Animation>("IDC_ANIMATION_SINGLE");
    const auto versusAnimation = versusModeButton->FindChild<Gx::Animation>("IDC_ANIMATION_VERSUS");

    const auto levelLimitToggleButton   = FindChild<Gx::ToggleButton>("IDC_TOGGLE_LEVEL_LIMIT");
    const auto minLevelLimitInput = FindChild<Gx::InputField>("IDC_EDIT_MIN_LEVEL_LIMIT");
    const auto maxLevelLimitInput = FindChild<Gx::InputField>("IDC_EDIT_MAX_LEVEL_LIMIT");

    const auto toolTip = FindChild<Gx::ToolTip>("IDC_TOOLTIP_INFO");

    titleInput->SetString(m_session.GetCurrentPlayer().Name + "'s Room");
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
    const auto titleInput = FindChild<Gx::InputField>("IDC_EDIT_TITLE");
    auto passwordInput    = FindChild<Gx::InputField>("IDC_EDIT_PASSWORD");

    const auto levelLimitToggleButton   = FindChild<Gx::ToggleButton>("IDC_TOGGLE_LEVEL_LIMIT");
    const auto minLevelLimitInput = FindChild<Gx::InputField>("IDC_EDIT_MIN_LEVEL_LIMIT");
    const auto maxLevelLimitInput = FindChild<Gx::InputField>("IDC_EDIT_MAX_LEVEL_LIMIT");

    const auto toolTip = FindChild<Gx::ToolTip>("IDC_TOOLTIP_INFO");

    if (titleInput->GetString().isEmpty())
    {
        toolTip->SetString("Please enter a room name.");
        toolTip->Show(this);
        return;
    }

    if (levelLimitToggleButton->IsChecked())
    {
        if (minLevelLimitInput->GetString().isEmpty() || maxLevelLimitInput->GetString().isEmpty())
        {
            toolTip->SetString("Please set level limit. Ex) 10 ~ 20");
            toolTip->Show(this);

            return;
        }

        toolTip->SetString("");
        const unsigned int min = std::stoi(std::string(minLevelLimitInput->GetString()));
        const unsigned int max = std::stoi(std::string(maxLevelLimitInput->GetString()));

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
    if (const auto titleInput = FindChild<Gx::InputField>("IDC_EDIT_TITLE"); titleInput)
        return titleInput->GetString();

    return {};
}

std::string CreateRoomDialog::GetRoomPassword() const
{
    if (const auto passwordInput = FindChild<Gx::InputField>("IDC_EDIT_PASSWORD"); passwordInput)
        return passwordInput->GetString();

    return {};
}

unsigned int CreateRoomDialog::GetMinLevelLimit() const
{
    const auto levelLimitToggleButton   = FindChild<Gx::ToggleButton>("IDC_TOGGLE_LEVEL_LIMIT");
    const auto minLevelLimitInput = FindChild<Gx::InputField>("IDC_EDIT_MIN_LEVEL_LIMIT");

    if (minLevelLimitInput && levelLimitToggleButton && levelLimitToggleButton->IsChecked())
        return std::stoi(std::string(minLevelLimitInput->GetString()));

    return 0;
}

unsigned int CreateRoomDialog::GetMaxLevelLimit() const
{
    const auto levelLimitToggleButton = FindChild<Gx::ToggleButton>("IDC_TOGGLE_LEVEL_LIMIT");
    const auto maxLevelLimitInput     = FindChild<Gx::InputField>("IDC_EDIT_MAX_LEVEL_LIMIT");

    if (maxLevelLimitInput && levelLimitToggleButton && levelLimitToggleButton->IsChecked())
        return std::stoi(std::string(maxLevelLimitInput->GetString()));

    return 0;
}

