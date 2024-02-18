#include <OTwo/Dialogs/CreateRoomDialog.hpp>

void CreateRoomDialog::Initialize()
{
    AddChild(m_jamModeButton.get(), m_vsModeButton.get(), m_singleModeButton.get());
}

void CreateRoomDialog::OnShown(Gx::Scene &scene)
{
    Dialog::OnShown(scene);

    m_jamModeButton->SetCheckedState(false);
    m_singleModeButton->SetCheckedState(false);
    m_vsModeButton->SetCheckedState(true);

    m_jamAnimation->Stop();
    m_singleModeAnimation->Stop();
    m_vsModeAnimation->SetRepeatCount(1);
    m_vsModeAnimation->Reset();

    // TODO: Add ProfileController
    m_titleTextBox->SetString("CXO2's Room");
    m_titleTextBox->SelectAll();
    m_passwordTextBox->SetString("");

    m_levelLimitCheckBox->SetCheckedState(false);
    m_minLevelLimitTextBox->SetEnabled(false);
    m_maxLevelLimitTextBox->SetEnabled(false);
    m_minLevelLimitTextBox->SetString("");
    m_maxLevelLimitTextBox->SetString("");
    m_toolTip->Hide();
}

void CreateRoomDialog::SetTitleTextBox(Gx::ResourcePtr<Gx::TextBox> titleTextBox)
{
    m_titleTextBox = std::move(titleTextBox);
    m_titleTextBox->SetFocus(true);
    AddChild(m_titleTextBox.get());
}

void CreateRoomDialog::SetPasswordTextBox(Gx::ResourcePtr<Gx::TextBox> passwordTextBox)
{
    m_passwordTextBox = std::move(passwordTextBox);
    m_passwordTextBox->SetMasked(true);
    AddChild(m_passwordTextBox.get());
}

void CreateRoomDialog::SetJamModeButton(Gx::ResourcePtr<Gx::RadioButton> jamModeButton)
{
    m_jamModeButton = std::move(jamModeButton);
    m_jamModeButton->SetClickCallback(
        [&] (auto& sender, auto& ev)
        {
            GetScene()->GetApplication().Require<Gx::Mixer>().Play(m_sfxCreateMode.get(), "SFX");
        }
    );

    m_jamModeButton->SetCheckStateChangeCallback([=] (auto sender)
    {
        if (!sender->IsChecked())
            return;

        if (m_jamAnimation->GetState() != Gx::Animation::AnimationState::Playing)
        {
            m_jamModeButton->SetVisible(false);
            m_jamAnimation->Reset();
            m_jamAnimation->SetVisible(true);
        }

        m_toolTip->SetString("JAM Mode is not available.");
        m_toolTip->Show(this);

        m_jamModeButton->SetCheckedState(false);
        m_vsModeButton->SetCheckedState(true);
    });
}

void CreateRoomDialog::SetVsModeButton(Gx::ResourcePtr<Gx::RadioButton> vsModeButton)
{
    m_vsModeButton = std::move(vsModeButton);
    m_vsModeButton->SetClickCallback(
        [&] (auto& sender, auto& ev)
        {
            GetScene()->GetApplication().Require<Gx::Mixer>().Play(m_sfxCreateMode.get(), "SFX");
        }
    );
    m_vsModeButton->SetCheckStateChangeCallback([=] (auto sender)
    {
        m_vsModeAnimation->Reset();
        m_vsModeAnimation->SetRepeatCount(sender->IsChecked() ? 3 : 1);
    });
}

void CreateRoomDialog::SetSingleModeButton(Gx::ResourcePtr<Gx::RadioButton> singleModeButton)
{
    m_singleModeButton = std::move(singleModeButton);
    m_singleModeButton->SetClickCallback(
        [&] (auto& sender, auto& ev)
        {
            GetScene()->GetApplication().Require<Gx::Mixer>().Play(m_sfxCreateMode.get(), "SFX");
        }
    );
    m_singleModeButton->SetCheckStateChangeCallback([=] (auto sender)
    {
        m_singleModeAnimation->Reset();
        m_singleModeAnimation->SetRepeatCount(sender->IsChecked() ? 3 : 1);

        m_passwordTextBox->SetEnabled(!sender->IsChecked());
        if (!m_passwordTextBox->IsEnabled())
            m_passwordTextBox->SetString("");
    });
}

void CreateRoomDialog::SetJamAnimation(Gx::ResourcePtr<Gx::Animation> jamAnimation)
{
    m_jamAnimation = std::move(jamAnimation);
    m_jamAnimation->SetAnimationCallback([=] (Gx::Animation& sender)
    {
        //sender.SetVisible(sender.GetState() != Gx::Animation::AnimationState::Stopped && sender.GetState() != Gx::Animation::AnimationState::Completed);
        m_jamModeButton->SetVisible(sender.GetState() == Gx::Animation::AnimationState::Stopped || sender.GetState() == Gx::Animation::AnimationState::Completed);
    });
    m_jamAnimation->Stop();

    Node::AddChild(m_jamAnimation.get());
}

void CreateRoomDialog::SetVsModeAnimation(Gx::ResourcePtr<Gx::Animation> vsModeAnimation)
{
    m_vsModeAnimation = std::move(vsModeAnimation);
    m_vsModeAnimation->SetAnimationCallback([=] (Gx::Animation& sender)
    {
        m_vsModeButton->SetVisible(sender.GetState() == Gx::Animation::AnimationState::Stopped || sender.GetState() == Gx::Animation::AnimationState::Completed);
    });
    m_vsModeAnimation->Stop();

    Node::AddChild(m_vsModeAnimation.get());
}

void CreateRoomDialog::SetSingleModeAnimation(Gx::ResourcePtr<Gx::Animation> singleModeAnimation)
{
    m_singleModeAnimation = std::move(singleModeAnimation);
    m_singleModeAnimation->SetAnimationCallback([=] (Gx::Animation& sender)
    {
        m_singleModeButton->SetVisible(sender.GetState() == Gx::Animation::AnimationState::Stopped || sender.GetState() == Gx::Animation::AnimationState::Completed);
    });
    m_singleModeAnimation->Stop();

    Node::AddChild(m_singleModeAnimation.get());
}

void CreateRoomDialog::SetLevelLimitCheckBox(Gx::ResourcePtr<Gx::CheckBox> levelLimitCheckBox)
{
    m_levelLimitCheckBox = std::move(levelLimitCheckBox);
    m_levelLimitCheckBox->SetCheckStateChangeCallback([=] (auto sender)
    {
        m_minLevelLimitTextBox->SetEnabled(sender->IsChecked());
        m_maxLevelLimitTextBox->SetEnabled(sender->IsChecked());

        if (!sender->IsChecked())
        {
            m_minLevelLimitTextBox->SetString("");
            m_maxLevelLimitTextBox->SetString("");
        }
    });

    AddChild(m_levelLimitCheckBox.get());
}

void CreateRoomDialog::SetMinLevelLimitTextBox(Gx::ResourcePtr<Gx::TextBox> textBox)
{
    m_minLevelLimitTextBox = std::move(textBox);
    m_minLevelLimitTextBox->SetNumericModeEnabled(true);
    AddChild(m_minLevelLimitTextBox.get());
}

void CreateRoomDialog::SetMaxLevelLimitTextBox(Gx::ResourcePtr<Gx::TextBox> textBox)
{
    m_maxLevelLimitTextBox = std::move(textBox);
    m_maxLevelLimitTextBox->SetNumericModeEnabled(true);
    AddChild(m_maxLevelLimitTextBox.get());
}

void CreateRoomDialog::SetToolTip(Gx::ResourcePtr<Gx::ToolTip> toolTip)
{
    m_toolTip = std::move(toolTip);
    AddChild(m_toolTip.get());
}

void CreateRoomDialog::SetCreateModeSoundEffect(Gx::ResourcePtr<sf::Sound> sound)
{
    m_sfxCreateMode = std::move(sound);
}

void CreateRoomDialog::OnAccepted()
{
    if (m_titleTextBox->GetString().isEmpty())
    {
        m_toolTip->SetString("Please enter a room name.");
        m_toolTip->Show(this);
        return;
    }

    if (m_levelLimitCheckBox->IsChecked())
    {
        if (m_minLevelLimitTextBox->GetString().isEmpty() || m_maxLevelLimitTextBox->GetString().isEmpty())
        {
            m_toolTip->SetString("Please set level limit. Ex) 10 ~ 20");
            m_toolTip->Show(this);

            return;
        }

        m_toolTip->SetString("");
        unsigned int min = std::stoi(std::string(m_minLevelLimitTextBox->GetString()));
        unsigned int max = std::stoi(std::string(m_maxLevelLimitTextBox->GetString()));

        if (min > 100 || max > 100)
            m_toolTip->SetString("Wrong level selected. You can enter level range from 1 to 100");
        else if (min > max)
            m_toolTip->SetString("You must enter higher numbers in HIGH LEVEL");

        if (!m_toolTip->GetString().isEmpty())
        {
            m_toolTip->Show(this);
            return;
        }
    }

    Dialog::OnAccepted();
}
