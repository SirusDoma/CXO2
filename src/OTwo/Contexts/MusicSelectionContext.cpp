#include <OTwo/Contexts/MusicSelectionContext.hpp>

MusicSelectionContext::MusicSelectionContext() :
    m_metadata({0}),
    m_random(static_cast<LevelCategory>(0)),
    m_sort(MusicSortMode::ID),
    m_order(MusicSortOrder::Descending),
    m_difficulty(Difficulty::EX),
    m_speed(1.0f)
{
}

ChartMetadata MusicSelectionContext::GetMetadata() const
{
    return m_metadata;
}

void MusicSelectionContext::SetMetadata(const ChartMetadata &metadata)
{
    m_metadata = metadata;
}

LevelCategory MusicSelectionContext::GetRandomLevel() const
{
    return m_random;
}

void MusicSelectionContext::SetRandomLevel(const LevelCategory random)
{
    m_random = random;
}

MusicSortMode MusicSelectionContext::GetSortMode() const
{
    return m_sort;
}

void MusicSelectionContext::SetSortMode(const MusicSortMode sort)
{
    m_sort = sort;
}

MusicSortOrder MusicSelectionContext::GetSortOrder() const
{
    return m_order;
}

void MusicSelectionContext::SetSortOrder(const MusicSortOrder order)
{
    m_order = order;
}

Difficulty MusicSelectionContext::GetDifficulty() const
{
    return m_difficulty;
}

void MusicSelectionContext::SetDifficulty(const Difficulty difficulty)
{
    m_difficulty = difficulty;
}

float MusicSelectionContext::GetSpeed() const
{
    return m_speed;
}

void MusicSelectionContext::SetSpeed(const float speed)
{
    m_speed = speed;
}
