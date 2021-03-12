#ifndef STATE_LOGO_HPP
#define STATE_LOGO_HPP

#include <SFML/Audio/Music.hpp>

#include <Genode/SceneGraph.hpp>
#include <Genode/Graphics.hpp>

#include <O2/IO/Archives/OmcArchive.hpp>

class StateAvi : public Gx::Scene
{
public:
    virtual void Initialize();
};

#endif