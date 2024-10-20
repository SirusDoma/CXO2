#include <OTwo/IO/PlayingResourceContext.hpp>

bool PlayingResourceContext::IsFxEnabled() const
{
    return m_fxEnabled;
}

void PlayingResourceContext::SetFxEnabled(const bool enabled)
{
    m_fxEnabled = enabled;
}

unsigned int PlayingResourceContext::GetMapID() const
{
    return m_mapID;
}

void PlayingResourceContext::SetMapID(const unsigned int mapID)
{
    m_mapID = mapID;
}

unsigned int PlayingResourceContext::GetEffectID() const
{
    return m_effectID;
}

void PlayingResourceContext::SetEffectID(const unsigned int effectID)
{
    if (effectID != 1 && effectID != 2)
        throw Gx::NotSupportedException("Effect ID must be between 1 and 2");

    m_effectID = effectID;
}

GameMode PlayingResourceContext::GetMode() const
{
    return m_mode;
}

void PlayingResourceContext::SetMode(const GameMode mode)
{
    m_mode = mode;
}
