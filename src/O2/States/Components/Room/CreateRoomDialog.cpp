#include <O2/States/Components/Room/CreateRoomDialog.hpp>


void CreateRoomDialog::Initialize(Gx::Application &app)
{
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

    m_vsModeButton->SetCheckStateChangeCallback([=] (auto sender)
    {
        if (!sender->IsChecked())
            return;

        m_vsModeButton->SetVisible(false);
        m_vsModeAnimation->Reset();
        m_vsModeAnimation->SetVisible(true);
    });

    m_singleModeButton->SetCheckStateChangeCallback([=] (auto sender)
    {
        if (!sender->IsChecked())
            return;

        m_singleModeAnimation->Reset();
        m_singleModeAnimation->SetVisible(true);
        m_singleModeButton->SetVisible(false);
    });

    AddChild(m_jamModeButton, m_vsModeButton, m_singleModeButton);
}

void CreateRoomDialog::SetJamModeButton(Gx::RadioButton *jamModeButton)
{
    m_jamModeButton = jamModeButton;
}

void CreateRoomDialog::SetVsModeButton(Gx::RadioButton *vsModeButton)
{
    m_vsModeButton = vsModeButton;
}

void CreateRoomDialog::SetSingleModeButton(Gx::RadioButton *singleModeButton)
{
    m_singleModeButton = singleModeButton;
}

void CreateRoomDialog::SetJamAnimation(Gx::Animation *jamAnimation)
{
    m_jamAnimation = jamAnimation;
    m_jamAnimation->SetAnimationCallback([=] (Gx::Animation& sender)
    {
        sender.SetVisible(sender.GetState() != Gx::Animation::AnimationState::Stopped && sender.GetState() != Gx::Animation::AnimationState::Completed);
        m_jamModeButton->SetVisible(sender.GetState() == Gx::Animation::AnimationState::Stopped || sender.GetState() == Gx::Animation::AnimationState::Completed);
    });
    m_jamAnimation->Stop();

    Node::AddChild(m_jamAnimation);
}

void CreateRoomDialog::SetVsModeAnimation(Gx::Animation *vsModeAnimation)
{
    m_vsModeAnimation = vsModeAnimation;
    m_vsModeAnimation->SetAnimationCallback([=] (Gx::Animation& sender)
    {
        sender.SetVisible(sender.GetState() != Gx::Animation::AnimationState::Stopped && sender.GetState() != Gx::Animation::AnimationState::Completed);
        m_vsModeButton->SetVisible(sender.GetState() == Gx::Animation::AnimationState::Stopped || sender.GetState() == Gx::Animation::AnimationState::Completed);
    });
    m_vsModeAnimation->Stop();

    Node::AddChild(m_vsModeAnimation);
}

void CreateRoomDialog::SetSingleModeAnimation(Gx::Animation *singleModeAnimation)
{
    m_singleModeAnimation = singleModeAnimation;
    m_singleModeAnimation->SetAnimationCallback([=] (Gx::Animation& sender)
    {
        sender.SetVisible(sender.GetState() != Gx::Animation::AnimationState::Stopped && sender.GetState() != Gx::Animation::AnimationState::Completed);
        m_singleModeButton->SetVisible(sender.GetState() == Gx::Animation::AnimationState::Stopped || sender.GetState() == Gx::Animation::AnimationState::Completed);
    });
    m_singleModeAnimation->Stop();

    Node::AddChild(m_singleModeAnimation);
}

void CreateRoomDialog::SetLevelLimit(Gx::CheckBox *levelLimit)
{
    m_levelLimit = levelLimit;
    AddChild(m_levelLimit);
}
