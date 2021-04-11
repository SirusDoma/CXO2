#ifndef O2JAM_COMMON_OPTION_DIALOG_HPP
#define O2JAM_COMMON_OPTION_DIALOG_HPP

#include <Genode/UI/Dialog.hpp>
#include <Genode/UI/Image.hpp>

class OptionDialog : public Gx::Dialog
{
public:
    using Gx::Dialog::Dialog;

    void Initialize(Gx::Scene &scene);

private:
    Gx::Image *m_background;
    Gx::UiContainer m_keyOptionContainer, m_soundOptionContainer;
};

#endif
