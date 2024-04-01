#include <OTwo/States/Components/Dialogs/SelectMusicDialog.hpp>

SelectMusicDialog::SelectMusicDialog(const Gx::Dialog &copy) :
    Gx::Dialog(copy),
    Gx::UiContainer(copy),
    Gx::Node(copy),
    m_initialized(false)
{
}

void SelectMusicDialog::Initialize()
{
    if (m_initialized)
        return;

    Gx::Dialog::Initialize();

    // Rewire callbacks due to copy constructor
    if (auto acceptButton = GetAcceptButton(); acceptButton)
        SetAcceptButton(*acceptButton);

    if (auto cancelButton = GetCancelButton(); cancelButton)
        SetCancelButton(*cancelButton);
}
