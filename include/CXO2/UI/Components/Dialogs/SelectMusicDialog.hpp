#pragma once

#include <CXO2/Models/Room.hpp>
#include <CXO2/Models/Game.hpp>
#include <CXO2/Metadata/Chart/O2JamChartMetadata.hpp>
#include <CXO2/Contexts/RoomContext.hpp>


#include <Genode/IO/Resource.hpp>
#include <CXO2/UI/Dialog.hpp>

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Gx
{
    class AudioMixer;
    class ResourceManager;
}

namespace Cx
{
    class List;
    class RadioButton;
    class ToggleButton;

    class SessionContext;
    class SpeedButton;
    class SelectMusicDialog : public Cx::Dialog
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

        const std::vector<ChartMetadata>& GetMusicList() const;
        const std::vector<ChartMetadata>& GetFilteredList() const;
        void SetFilteredList(const std::vector<ChartMetadata>& list);

        void SetRandomColor(const sf::Color& color);
        void SetNoticeColor(const sf::Color& color);
        void SetDisabledColor(const sf::Color& color);
        void SetWarningColor(const sf::Color& color);

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
        void SelectDifficulty(Cx::RadioButton& sender, Difficulty difficulty);

        void InvalidateMusicList();
        void InvalidateRandomView();
        void InvalidateSelectors();
        void InvalidateMusicInfo();

        void OnLeftButtonClicked(const Control& sender, const Control::Event& ev);
        void OnRightButtonClicked(const Control& sender, const Control::Event& ev);

        void OnMusicButtonFocusChanged(Control& sender, const Control::Event& ev);
        void OnMusicButtonCheckChanged(Cx::RadioButton& sender, const Control::Event& ev);
        void OnMusicSelectorScrolled(Control& sender, const Control::Event& ev);

        void OnSortNewButtonClicked(Control& sender, const Control::Event& ev);
        void OnSortTitleButtonClicked(Control& sender, const Control::Event& ev);
        void OnSortLevelButtonClicked(Control& sender, const Control::Event& ev);
        void OnSortDurationButtonClicked(Control& sender, const Control::Event& ev);
        void OnGenreButtonCheckChanged(Cx::RadioButton& sender, const Control::Event& ev);
        void OnRandomLevelButtonFocusChanged(Control& sender, const Control::Event& ev);
        void OnRandomLevelButtonCheckChanged(Cx::ToggleButton& sender, const Control::Event& ev);
        void OnExButtonCheckChanged(Cx::RadioButton& sender, const Control::Event& ev);
        void OnNxButtonCheckChanged(Cx::RadioButton& sender, const Control::Event& ev);
        void OnHxButtonCheckChanged(Cx::RadioButton& sender, const Control::Event& ev);
        void OnSpeedButtonCheckChanged(Cx::RadioButton& sender, const Control::Event& ev);

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
        sf::Color                    m_titleColor       = sf::Color();
        sf::Color                    m_randomColor      = sf::Color();
        sf::Color                    m_noticeColor      = sf::Color();
        sf::Color                    m_disabledColor    = sf::Color();
        sf::Color                    m_warningColor     = sf::Color();

        std::vector<SpeedButton*> m_speedButtons;

        std::unordered_map<Cx::RadioButton*, std::size_t>          m_musicButtonIndices;
        std::unordered_map<Cx::RadioButton*, std::optional<Genre>> m_genreButtonValues;
        std::unordered_map<Cx::RadioButton*, Difficulty>           m_difficultyButtonValues;
        std::unordered_map<Cx::ToggleButton*, LevelCategory>       m_randomLevelButtonValues;
    };
}
