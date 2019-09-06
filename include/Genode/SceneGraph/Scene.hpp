#ifndef GENODE_SCENE_HPP
#define GENODE_SCENE_HPP

#include <vector>

#include <SFML/Graphics.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities.hpp>
#include <Genode/Tasks/TaskContainer.hpp>

namespace Gx
{
    class SceneDirector;
    class Scene : public Node, public TaskContainer, public Renderable, public virtual Updatable, public Inputable
    {
    public:
        friend SceneDirector;

        Scene();
        Scene(const std::string& name);
        virtual ~Scene();

    protected:
        virtual void Initialize();
        virtual bool Close(bool quit = false);

        virtual sf::RenderStates Render(sf::RenderTarget& target, sf::RenderStates states) const;
        virtual void Update(double delta);
        virtual bool Input(sf::Event ev);

        SceneDirector* GetDirector() const;

    private:
        void Render(Node* node, sf::RenderTarget& target, sf::RenderStates states) const;
        void Update(Node* node, double delta);
        void Input(Node* node, sf::Event ev);

        void SetDirector(SceneDirector* director);

        SceneDirector* m_director;
    };
}

#endif
