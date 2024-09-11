#ifndef O2JAM_DIALOG_SELECT_MUSIC_DIALOG_HPP
#define O2JAM_DIALOG_SELECT_MUSIC_DIALOG_HPP

#include <OTwo/Models/Room.hpp>
#include <OTwo/Models/Game.hpp>
#include <OTwo/Metadata/Chart/ChartMetadata.hpp>
#include <OTwo/Contexts/MusicSelectionContext.hpp>

#include <Genode/UI/Dialog.hpp>

#include <vector>

namespace Gx
{
    class Mixer;
    class ResourceManager;
}

class SessionContext;
class SelectMusicDialog : public Gx::Dialog
{
public:
    SelectMusicDialog(Gx::Mixer& mixer, Gx::ResourceManager& resources, SessionContext& session, MusicSelectionContext& selection);

    void Initialize() override;

    ChartMetadata GetSelectedMusic() const;
    LevelCategory GetSelectedRandomLevels() const;
    Difficulty GetSelectedDifficulty() const;
    Genre GetSelectedGenre() const;
    float GetSelectedSpeed() const;

    void Sort(MusicSortMode sort, MusicSortOrder order = static_cast<MusicSortOrder>(0));
    void CacheMusicCover(bool refresh = true) const;

protected:
    void OnKeyPressed(const sf::Event::KeyPressed& ev) override;

    void OnShown(Gx::Scene &scene) override;
    void OnAccepted() override;
    void OnCancelled() override;

    void Invalidate() override;

private:
    bool         m_initialized = false;
    unsigned int m_page = 0;
    unsigned int m_coverID;
    float        m_speed;

    Gx::Mixer&             m_mixer;
    Gx::ResourceManager&   m_resources;
    SessionContext&        m_session;
    MusicSelectionContext& m_selection;

    Difficulty     m_difficulty = Difficulty::EX;
    MusicSortMode  m_sort       = static_cast<MusicSortMode>(-1);
    MusicSortOrder m_order      = static_cast<MusicSortOrder>(-1);
    Genre          m_genre      = static_cast<Genre>(-1);
    LevelCategory  m_random     = static_cast<LevelCategory>(0);

    ChartMetadata                m_music;
    Gx::ResourcePtr<sf::Texture> m_thumbnail;
    std::vector<ChartMetadata>   m_musicList;
    std::vector<ChartMetadata>   m_displayList;
};

#endif
