#pragma once

#include <CXO2/Models/Room.hpp>
#include <CXO2/Models/Game.hpp>
#include <CXO2/Metadata/Chart/O2JamChartMetadata.hpp>
#include <CXO2/Contexts/RoomContext.hpp>

#include <Genode/IO/Resource.hpp>
#include <Genode/UI/Dialog.hpp>

#include <unordered_map>
#include <vector>

namespace Gx
{
    class AudioMixer;
    class RadioButton;
    class ResourceManager;
    class ToggleButton;
}

namespace Cx
{

    class SessionContext;
    class SelectMusicDialog : public Gx::Dialog
    {
    public:
        SelectMusicDialog(Gx::AudioMixer& mixer, Gx::ResourceManager& resources, SessionContext& session, RoomContext& room);

        void Initialize() override;

        ChartMetadata GetSelectedMusic() const;
        Difficulty GetSelectedDifficulty() const;
        LevelCategory GetSelectedRandomLevels() const;

        Genre GetSelectedGenre() const;
        float GetSelectedSpeed() const;

        void Sort(MusicSortMode sort, MusicSortOrder order = static_cast<MusicSortOrder>(0));

    protected:
        void OnKeyPressed(const sf::Event::KeyPressed& ev) override;

        void OnPresented(Parent& parent, const Gx::PresentationContext& context) override;
        void OnAccepted() override;
        void OnCancelled() override;

        void Invalidate() override;

    private:
        void ToggleSort(MusicSortMode mode);
        void SelectDifficulty(Gx::RadioButton& sender, Difficulty difficulty);

        void OnLeftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnRightButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnMusicButtonFocusChanged(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMusicButtonCheckChanged(Gx::RadioButton& sender);
        void OnMusicSelectorScrolled(Gx::Control& sender, Gx::Control::Event& ev);

        void OnSortNewButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnSortTitleButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnSortLevelButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnSortDurationButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnGenreButtonCheckChanged(Gx::RadioButton& sender);
        void OnRandomLevelButtonCheckChanged(Gx::ToggleButton& sender);
        void OnExButtonCheckChanged(Gx::RadioButton& sender);
        void OnNxButtonCheckChanged(Gx::RadioButton& sender);
        void OnHxButtonCheckChanged(Gx::RadioButton& sender);
        void OnSpeedButtonCheckChanged(Gx::RadioButton& sender);

        bool         m_initialized = false;
        unsigned int m_page = 0;
        unsigned int m_randomMusicCount = 0;
        unsigned int m_coverID;
        float        m_speed;

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
        std::vector<ChartMetadata>   m_displayList;

        std::unordered_map<Gx::RadioButton*, std::size_t>          m_musicButtonIndices;
        std::unordered_map<Gx::RadioButton*, std::optional<Genre>> m_genreButtonValues;
        std::unordered_map<Gx::ToggleButton*, LevelCategory>       m_randomLevelButtonValues;
        std::unordered_map<Gx::RadioButton*, float>                m_speedButtonValues;
    };
}
