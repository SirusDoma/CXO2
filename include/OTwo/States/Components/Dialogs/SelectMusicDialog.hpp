#ifndef O2JAM_DIALOG_SELECT_MUSIC_DIALOG_HPP
#define O2JAM_DIALOG_SELECT_MUSIC_DIALOG_HPP

#include <OTwo/Data/Room.hpp>

#include <Genode/UI/Dialog.hpp>

#include <vector>

class SelectMusicDialog : public Gx::Dialog
{
public:
    using Gx::Dialog::Dialog;

    explicit SelectMusicDialog(const Gx::Dialog &copy);
    void Initialize() override;

protected:
    void Invalidate() override;

private:
    bool m_initialized;
    unsigned int m_page;
    ChartMetadata *m_music = nullptr;
    std::vector<ChartMetadata> m_musicList;
    std::vector<ChartMetadata*> m_displayList;
};

#endif
