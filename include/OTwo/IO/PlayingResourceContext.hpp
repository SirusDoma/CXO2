#ifndef O2JAM_PLAYING_RESOURCE_CONTEXT_HPP
#define O2JAM_PLAYING_RESOURCE_CONTEXT_HPP

#include <Genode/IO/ResourceContext.hpp>

class PlayingResourceContext : public Gx::ResourceContext
{
public:
    // TODO: Read `requires` and only pick Playng_BG and Note_Click based on these IDs
    //       Do not add the entire objects inside 'requires' to the child
    unsigned int GetMapID() const;
    void SetMapID(const unsigned int mapID);

    unsigned int GetEffectID() const;
    void SetEffectID(const unsigned int effectID);

private:
    unsigned int m_mapID{1};
    unsigned int m_effectID{1};
};

#endif
