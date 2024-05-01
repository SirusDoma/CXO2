#ifndef O2JAM_STATE_MANAGER_HPP
#define O2JAM_STATE_MANAGER_HPP

#include <OTwo/States/State.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>

class SceneDirectorDecorator
{
public:
    static SceneDirectorDecorator Decorate(Gx::SceneDirector &director);

    template<typename T>
    void Register(const std::string &fileName);

    template<typename T>
    void Register(State &state);

    template<typename T>
    void Present() const;

private:
    explicit SceneDirectorDecorator(Gx::SceneDirector &director);

    Gx::SceneDirector   *m_director;
};

#include <OTwo/States/SceneDirectorDecorator.inl>
#endif
