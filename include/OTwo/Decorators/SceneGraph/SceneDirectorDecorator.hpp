#pragma once

#include <OTwo/States/State.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/SceneGraph/SceneDirector.hpp>

class SceneDirectorDecorator
{
public:
    static SceneDirectorDecorator Decorate(Gx::SceneDirector& director);

    template<typename T>
    std::enable_if_t<std::is_base_of_v<State, T>, void>
    Register(const std::string& fileName);

    template<typename T>
    std::enable_if_t<std::is_base_of_v<State, T>, void>
    Present(const Gx::ResourceContext& context = Gx::ResourceContext(typeid(T).name())) const;

private:
    explicit SceneDirectorDecorator(Gx::SceneDirector& director);

    Gx::SceneDirector* m_director;
};

#include <OTwo/Decorators/SceneGraph/SceneDirectorDecorator.inl>
