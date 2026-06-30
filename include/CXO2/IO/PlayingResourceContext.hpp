#pragma once

#include <Genode/IO/ResourceContext.hpp>
#include <CXO2/Models/Game.hpp>

namespace Cx
{
    class PlayingResourceContext : public Gx::ResourceContext
    {
    public:
        using Gx::ResourceContext::ResourceContext;

        bool IsFxEnabled() const;
        void SetFxEnabled(const bool enabled);

        unsigned int GetMapID() const;
        void SetMapID(const unsigned int mapID);

        unsigned int GetEffectID() const;
        void SetEffectID(const unsigned int effectID);

        GameMode GetMode() const;
        void SetMode(const GameMode mode);

    private:
        bool m_fxEnabled;
        unsigned int m_mapID{0};
        unsigned int m_effectID{1};
        GameMode m_mode  = GameMode::Versus;
    };
}
