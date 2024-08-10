#ifndef O2JAM_NOTE_CONTAINER_HPP
#define O2JAM_NOTE_CONTAINER_HPP

#include <Genode/SceneGraph.hpp>

#include <OTwo/Chart/Note.hpp>

class ChartRenderer;
class NoteContainer : public virtual Gx::Renderable, public virtual Gx::Updatable, Gx::RenderableContainer, Gx::UpdatableContainer
{
public:
    NoteContainer();

    void Add(Note& note);
    Note* GetNote(Chart::Channel channel, double position) const;

    sf::FloatRect GetViewport() const;
    void SetViewport(const sf::FloatRect &viewport);

    sf::VertexArray& GetNoteVertices();
    sf::VertexArray& GetMeasureVertices();
    sf::VertexArray& GetGuideLineVertices();

    const sf::Texture* GetTexture(NoteShape shape);
    void SetTexture(NoteShape shape, const sf::Texture &texture);

    void RegisterPrefab(Gx::Updatable &prefab);
    std::unordered_set<Gx::Updatable*> GetRegisteredPrefabs();

    void Render(const ChartRenderer &renderer, double delta);

private:
    using TextureMap = std::unordered_map<NoteShape, const sf::Texture*>;

    Gx::RenderStates Render(Gx::RenderSurface &surface, Gx::RenderStates states) const override;

    sf::FloatRect   m_viewport;
    sf::VertexArray m_noteVertices;
    sf::VertexArray m_measureVertices;
    sf::VertexArray m_guideLineVertices;

    std::unordered_map<double, std::vector<Note*>> m_notes;
    std::unordered_set<Gx::Updatable*> m_prefabs;

    TextureMap m_textures;
    NoteShape  m_shape;

};

#endif
