#pragma once

#include <OTwo/Models/Room.hpp>
#include <OTwo/Models/Game.hpp>
#include <OTwo/Metadata/Chart/O2JamChartMetadata.hpp>
#include <OTwo/Contexts/RoomContext.hpp>

#include <Genode/IO/Resource.hpp>
#include <Genode/UI/Dialog.hpp>

#include <vector>

namespace Gx
{
    class AudioMixer;
    class ResourceManager;
}

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
};
