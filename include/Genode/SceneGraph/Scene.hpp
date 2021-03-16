#ifndef GENODE_SCENE_HPP
#define GENODE_SCENE_HPP

#include <vector>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>
#include <Genode/SceneGraph/InputableContainer.hpp>
#include <Genode/Entities.hpp>
#include <Genode/Tasks/TaskContainer.hpp>

namespace Gx
{
    class SceneDirector;
    class Scene : public virtual Node, public RenderableContainer, public virtual UpdatableContainer, public InputableContainer, public TaskContainer
    {
    public:
        friend SceneDirector;

        Scene();
        Scene(const std::string& name);
        virtual ~Scene();

    protected:
        virtual void Initialize();
        virtual bool Close(bool quit = false);
        virtual void Update(double delta);

        SceneDirector* GetDirector() const;

    private:
        SceneDirector* m_director;
        void SetDirector(SceneDirector* director);
    };
}

#endif
