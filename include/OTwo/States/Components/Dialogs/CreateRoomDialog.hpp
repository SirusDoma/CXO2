#ifndef O2JAM_DIALOG_CREATE_ROOM_DIALOG_HPP
#define O2JAM_DIALOG_CREATE_ROOM_DIALOG_HPP

#include <Genode/UI/Dialog.hpp>
#include <Genode/UI/ToolTip.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/TextBox.hpp>

#include <Genode/Graphics/Animation.hpp>

class CreateRoomDialog : public Gx::Dialog
{
public:
    using Gx::Dialog::Dialog;

    explicit CreateRoomDialog(const Gx::Dialog &copy);

    void Initialize() override;

protected:
    void OnShown(Gx::Scene &scene) override;
    void OnAccepted() override;

private:
    bool m_initialized;
};

#endif
