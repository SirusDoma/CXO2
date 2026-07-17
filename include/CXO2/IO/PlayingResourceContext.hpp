#pragma once

#include <Genode/IO/ResourceContext.hpp>
#include <CXO2/Models/Game.hpp>

namespace Cx
{
    class PlayingResourceContext : public Gx::ResourceContext
    {
    public:
        using Gx::ResourceContext::ResourceContext;

        [[nodiscard]] bool IsFxEnabled() const;
        void SetFxEnabled(const bool enabled);

        [[nodiscard]] unsigned int GetMapID() const;
        void SetMapID(const unsigned int mapID);

        [[nodiscard]] unsigned int GetEffectID() const;
        void SetEffectID(const unsigned int effectID);

        [[nodiscard]] GameMode GetMode() const;
        void SetMode(const GameMode mode);

    private:
        bool m_fxEnabled;
        unsigned int m_mapID{0};
        unsigned int m_effectID{1};
        GameMode m_mode  = GameMode::Versus;
    };
}
