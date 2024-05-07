#ifndef O2JAM_PLAYING_RESOURCE_CONTEXT_HPP
#define O2JAM_PLAYING_RESOURCE_CONTEXT_HPP

#include <Genode/IO/ResourceContext.hpp>

class PlayingResourceContext : public Gx::ResourceContext
{
public:
    using Gx::ResourceContext::ResourceContext;
    unsigned int GetMapID() const;
    void SetMapID(const unsigned int mapID);

    unsigned int GetEffectID() const;
    void SetEffectID(const unsigned int effectID);

private:
    unsigned int m_mapID{0};
    unsigned int m_effectID{1};
};

#endif
