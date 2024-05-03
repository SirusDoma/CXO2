#include <OTwo/IO/PlayingResourceContext.hpp>

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
