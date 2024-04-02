#ifndef O2JAM_DIALOG_SELECT_MUSIC_DIALOG_HPP
#define O2JAM_DIALOG_SELECT_MUSIC_DIALOG_HPP

#include <OTwo/Data/Room.hpp>
#include <OTwo/Metadata/Chart/O2ChartMetadata.hpp>

#include <Genode/UI/Dialog.hpp>

#include <vector>

enum MusicSortMode
{
    ID,
    Title,
    Level,
    Duration
};

enum MusicSortOrder
{
    None,
    Ascending,
    Descending
};

class SelectMusicDialog : public Gx::Dialog
{
public:
    using Gx::Dialog::Dialog;

    explicit SelectMusicDialog(const Gx::Dialog &copy);
    void Initialize() override;

    O2ChartMetadata GetSelectedMusic() const;
    OhmLevel GetSelectedRandomLevels() const;
    Difficulty GetDifficulty() const;
    Genre GetGenre() const;

    void Sort(MusicSortMode mode, MusicSortOrder order = static_cast<MusicSortOrder>(0));

protected:
    void OnAccepted() override;
    void OnCancelled() override;

    void Invalidate() override;

private:
    bool m_initialized;
    unsigned int m_page;
    Difficulty m_difficulty;
    MusicSortMode m_sort;
    MusicSortOrder m_order;
    Genre m_genre;
    OhmLevel m_random;
    O2ChartMetadata m_music;
    std::vector<O2ChartMetadata> m_musicList;
    std::vector<O2ChartMetadata*> m_displayList;
};

#endif
