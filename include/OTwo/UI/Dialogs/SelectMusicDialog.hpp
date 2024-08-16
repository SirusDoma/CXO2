#ifndef O2JAM_DIALOG_SELECT_MUSIC_DIALOG_HPP
#define O2JAM_DIALOG_SELECT_MUSIC_DIALOG_HPP

#include <OTwo/Models/Room.hpp>
#include <OTwo/Models/Game.hpp>
#include <OTwo/Metadata/Chart/ChartMetadata.hpp>
#include <OTwo/Contexts/MusicSelectionContext.hpp>

#include <Genode/UI/Dialog.hpp>

#include <vector>

class SelectMusicDialog : public Gx::Dialog
{
public:
    using Gx::Dialog::Dialog;
    explicit SelectMusicDialog(const Gx::Dialog &copy);

    void Initialize() override;

    ChartMetadata GetSelectedMusic() const;
    LevelCategory GetSelectedRandomLevels() const;
    Difficulty GetSelectedDifficulty() const;
    Genre GetSelectedGenre() const;
    float GetSelectedSpeed() const;

    void Sort(MusicSortMode sort, MusicSortOrder order = static_cast<MusicSortOrder>(0));

protected:
    void OnKeyDown(sf::Event::KeyEvent ev) override;

    void OnShown(Gx::Scene &scene) override;
    void OnAccepted() override;
    void OnCancelled() override;

    void CacheMusicCover() const;

    void Invalidate() override;

private:
    bool m_initialized;
    unsigned int m_page;
    unsigned int m_coverID;
    float m_speed;
    Gx::ResourcePtr<sf::Texture> m_thumbnail;
    Difficulty m_difficulty;
    MusicSortMode m_sort;
    MusicSortOrder m_order;
    Genre m_genre;
    LevelCategory m_random;
    ChartMetadata m_music;
    std::vector<ChartMetadata> m_musicList;
    std::vector<ChartMetadata*> m_displayList;
};

#endif
