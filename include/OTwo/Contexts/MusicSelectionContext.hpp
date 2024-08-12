#ifndef O2JAM_MUSIC_SELECTION_CONTEXT_HPP
#define O2JAM_MUSIC_SELECTION_CONTEXT_HPP

#include <Genode/System/Provider.hpp>

#include <OTwo/Metadata/Chart/ChartMetadata.hpp>
#include <OTwo/Metadata/Chart/ChartMetadataView.hpp>

enum class MusicSortMode
{
    ID,
    Title,
    Level,
    Duration
};

enum class MusicSortOrder
{
    None,
    Ascending,
    Descending
};

class MusicSelectionContext : public Gx::Provider
{
public:
    MusicSelectionContext();

    ChartMetadata GetMetadata() const;
    void SetMetadata(const ChartMetadata &metadata);

    LevelCategory GetRandomLevel() const;
    void SetRandomLevel(const LevelCategory random);

    MusicSortMode GetSortMode() const;
    void SetSortMode(const MusicSortMode sort);

    MusicSortOrder GetSortOrder() const;
    void SetSortOrder(const MusicSortOrder order);

    Difficulty GetDifficulty() const;
    void SetDifficulty(const Difficulty difficulty);

    float GetSpeed() const;
    void SetSpeed(const float speed);

private:
    ChartMetadata  m_metadata;
    LevelCategory  m_random;
    MusicSortMode  m_sort;
    MusicSortOrder m_order;
    Difficulty     m_difficulty;
    float          m_speed;
};

#endif
