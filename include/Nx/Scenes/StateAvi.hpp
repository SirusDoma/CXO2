#ifndef STATE_LOGO_HPP
#define STATE_LOGO_HPP

#include <SFML/Audio/Music.hpp>

#include <Genode/SceneGraph.hpp>
#include <Genode/Graphics.hpp>
#include <Genode/IO.hpp>

#include <iostream>
#include <Nx/IO/Archives/OmcArchive.hpp>

class StateAvi : public Gx::Scene
{
public:
    virtual void Initialize()
    {
        // Create sprite from definition (!!!)
        auto sprite = Gx::ResourceManager::Instance()->Create<Gx::Sprite>("Definitions\\StateAvi\\Background.json");
        AddChild(sprite);

        // Create music from definition (!!!)
        auto bgm = Gx::ResourceManager::Instance()->Create<sf::Music>("Definitions\\StateAvi\\Music.json");
        if (bgm)
            bgm->play();

        Run(new Gx::Sequence({
            new Gx::Fade(sprite, 255, 2500),
            new Gx::Fade(sprite, 0, 2500)
        }));
    }
};

#endif