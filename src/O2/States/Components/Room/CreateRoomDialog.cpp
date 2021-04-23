#include <O2/States/Components/Room/CreateRoomDialog.hpp>


void CreateRoomDialog::Initialize(Gx::Application &app)
{
    auto& mixer        = app.Require<Gx::Mixer>();
    auto sfxCreateMode = mixer.Create<sf::Sound>("Interface/Metadata/Dialog/CreateRoom/ModeSound.json");

    m_jamModeButton->SetClickCallback([=, &mixer = mixer] (auto& sender, auto& ev) { mixer.Play(sfxCreateMode, "SFX"); });
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

        // TODO: Tooltip, Jam mode placeholder
        m_jamModeButton->SetCheckedState(false);
        m_vsModeButton->SetCheckedState(true);
    });

    m_vsModeButton->SetClickCallback([=, &mixer = mixer] (auto& sender, auto& ev) { mixer.Play(sfxCreateMode, "SFX"); });
    m_vsModeButton->SetCheckStateChangeCallback([=] (auto sender)
    {
        if (!sender->IsChecked())
            return;

        m_vsModeButton->SetVisible(false);
        m_vsModeAnimation->Reset();
        m_vsModeAnimation->SetVisible(true);
    });

    m_singleModeButton->SetClickCallback([=, &mixer = mixer] (auto& sender, auto& ev) { mixer.Play(sfxCreateMode, "SFX"); });
    m_singleModeButton->SetCheckStateChangeCallback([=] (auto sender)
    {
        if (!sender->IsChecked())
            return;

        m_singleModeAnimation->Reset();
        m_singleModeAnimation->SetVisible(true);
        m_singleModeButton->SetVisible(false);
    });

    AddChild(m_jamModeButton.get(), m_vsModeButton.get(), m_singleModeButton.get());
}

void CreateRoomDialog::SetJamModeButton(Gx::ResourcePtr<Gx::RadioButton> jamModeButton)
{
    m_jamModeButton = std::move(jamModeButton);
}

void CreateRoomDialog::SetVsModeButton(Gx::ResourcePtr<Gx::RadioButton> vsModeButton)
{
    m_vsModeButton = std::move(vsModeButton);
}

void CreateRoomDialog::SetSingleModeButton(Gx::ResourcePtr<Gx::RadioButton> singleModeButton)
{
    m_singleModeButton = std::move(singleModeButton);
}

void CreateRoomDialog::SetJamAnimation(Gx::ResourcePtr<Gx::Animation> jamAnimation)
{
    m_jamAnimation = std::move(jamAnimation);
    m_jamAnimation->SetAnimationCallback([=] (Gx::Animation& sender)
    {
        sender.SetVisible(sender.GetState() != Gx::Animation::AnimationState::Stopped && sender.GetState() != Gx::Animation::AnimationState::Completed);
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
        sender.SetVisible(sender.GetState() != Gx::Animation::AnimationState::Stopped && sender.GetState() != Gx::Animation::AnimationState::Completed);
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
        sender.SetVisible(sender.GetState() != Gx::Animation::AnimationState::Stopped && sender.GetState() != Gx::Animation::AnimationState::Completed);
        m_singleModeButton->SetVisible(sender.GetState() == Gx::Animation::AnimationState::Stopped || sender.GetState() == Gx::Animation::AnimationState::Completed);
    });
    m_singleModeAnimation->Stop();

    Node::AddChild(m_singleModeAnimation.get());
}

void CreateRoomDialog::SetLevelLimitCheckBox(Gx::ResourcePtr<Gx::CheckBox> levelLimitCheckBox)
{
    m_levelLimitCheckBox = std::move(levelLimitCheckBox);
    AddChild(m_levelLimitCheckBox.get());
}
