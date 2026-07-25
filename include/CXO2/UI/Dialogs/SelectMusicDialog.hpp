#pragma once

#include <CXO2/Models/Room.hpp>
#include <CXO2/Models/Game.hpp>
#include <CXO2/Metadata/Chart/O2JamChartMetadata.hpp>
#include <CXO2/Contexts/RoomContext.hpp>

#include <Genode/IO/Resource.hpp>
#include <Genode/UI/Dialog.hpp>

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Gx
{
    class AudioMixer;
    class List;
    class RadioButton;
    class ResourceManager;
    class ToggleButton;
}

namespace Cx
{

    class SessionContext;
    class SpeedButton;
    class SelectMusicDialog : public Gx::Dialog
    {
    public:
        SelectMusicDialog(Gx::AudioMixer& mixer, Gx::ResourceManager& resources, SessionContext& session, RoomContext& room);

        void Initialize() override;

        ChartMetadata GetSelectedMusic() const;
        Difficulty GetSelectedDifficulty() const;
        LevelCategory GetSelectedRandomLevels() const;
        bool IsRandomActive() const;

        Genre GetSelectedGenre() const;
        float GetSelectedSpeed() const;
        SpeedMode GetSelectedSpeedMode() const;

        void Sort(MusicSortMode sort, MusicSortOrder order = static_cast<MusicSortOrder>(0));

    protected:
        void OnKeyPressed(const sf::Event::KeyPressed& ev) override;

        void OnPresented(Parent& parent, const Gx::PresentationContext& context) override;
        void OnAccepted() override;
        void OnCancelled() override;

    private:
        void FilterMusic();
        unsigned int CountRandomMusic(int min, int max, std::unordered_set<unsigned int>& scanned);

        unsigned int GetMaxPage() const;
        unsigned int GetPage() const;
        void SetPage(unsigned int page);

        void ToggleSort(MusicSortMode mode);
        void SelectDifficulty(Gx::RadioButton& sender, Difficulty difficulty);

        void InvalidateMusicList();
        void InvalidateRandomView();
        void InvalidateSelectors();
        void InvalidateMusicInfo();

        void OnLeftButtonClicked(const Gx::Control& sender, const Gx::Control::Event& ev);
        void OnRightButtonClicked(const Gx::Control& sender, const Gx::Control::Event& ev);

        void OnMusicButtonFocusChanged(Gx::Control& sender, const Gx::Control::Event& ev);
        void OnMusicButtonCheckChanged(Gx::RadioButton& sender, const Gx::Control::Event& ev);
        void OnMusicSelectorScrolled(Gx::Control& sender, const Gx::Control::Event& ev);

        void OnSortNewButtonClicked(Gx::Control& sender, const Gx::Control::Event& ev);
        void OnSortTitleButtonClicked(Gx::Control& sender, const Gx::Control::Event& ev);
        void OnSortLevelButtonClicked(Gx::Control& sender, const Gx::Control::Event& ev);
        void OnSortDurationButtonClicked(Gx::Control& sender, const Gx::Control::Event& ev);
        void OnGenreButtonCheckChanged(Gx::RadioButton& sender, const Gx::Control::Event& ev);
        void OnRandomLevelButtonFocusChanged(Gx::Control& sender, const Gx::Control::Event& ev);
        void OnRandomLevelButtonCheckChanged(Gx::ToggleButton& sender, const Gx::Control::Event& ev);
        void OnExButtonCheckChanged(Gx::RadioButton& sender, const Gx::Control::Event& ev);
        void OnNxButtonCheckChanged(Gx::RadioButton& sender, const Gx::Control::Event& ev);
        void OnHxButtonCheckChanged(Gx::RadioButton& sender, const Gx::Control::Event& ev);
        void OnSpeedButtonCheckChanged(Gx::RadioButton& sender, const Gx::Control::Event& ev);

        bool         m_initialized = false;
        unsigned int m_page = 0;
        unsigned int m_randomMusicCount = 0;
        unsigned int m_coverID;
        float        m_speed;
        SpeedMode    m_speedMode = SpeedMode::HiSpeed;

        Gx::AudioMixer&        m_mixer;
        Gx::ResourceManager&   m_resources;
        SessionContext&        m_session;
        RoomContext&           m_room;

        Difficulty                    m_difficulty = Difficulty::EX;
        std::optional<MusicSortMode>  m_sort       = std::nullopt;
        std::optional<MusicSortOrder> m_order      = std::nullopt;
        std::optional<Genre>          m_genre      = std::nullopt;
        LevelCategory                 m_random     = static_cast<LevelCategory>(0);

        ChartMetadata                m_music;
        Gx::ResourcePtr<sf::Texture> m_thumbnail;
        std::vector<ChartMetadata>   m_musicList;
        std::vector<ChartMetadata>   m_filteredList;
        sf::Color                    m_titleColor = sf::Color(25, 25, 25);

        std::vector<SpeedButton*> m_speedButtons;

        std::unordered_map<Gx::RadioButton*, std::size_t>          m_musicButtonIndices;
        std::unordered_map<Gx::RadioButton*, std::optional<Genre>> m_genreButtonValues;
        std::unordered_map<Gx::RadioButton*, Difficulty>           m_difficultyButtonValues;
        std::unordered_map<Gx::ToggleButton*, LevelCategory>       m_randomLevelButtonValues;
    };
}
