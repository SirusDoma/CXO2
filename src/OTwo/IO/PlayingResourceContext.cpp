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
    m_effectID = effectID;
}
