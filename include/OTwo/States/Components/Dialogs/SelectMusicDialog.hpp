#ifndef O2JAM_DIALOG_SELECT_MUSIC_DIALOG_HPP
#define O2JAM_DIALOG_SELECT_MUSIC_DIALOG_HPP

#include <OTwo/Data/Room.hpp>

#include <Genode/UI/Dialog.hpp>
#include <Genode/UI/ToolTip.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/TextBox.hpp>

#include <Genode/Graphics/Animation.hpp>

class SelectMusicDialog : public Gx::Dialog
{
public:
    using Gx::Dialog::Dialog;

    explicit SelectMusicDialog(const Gx::Dialog &copy);
    void Initialize() override;

private:
    bool m_initialized;
};

#endif
