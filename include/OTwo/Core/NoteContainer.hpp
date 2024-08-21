#ifndef O2JAM_CORE_NOTE_CONTAINER_HPP
#define O2JAM_CORE_NOTE_CONTAINER_HPP

#include <OTwo/Core/Chart.hpp>
#include <OTwo/Config/GameConfig.hpp>

#include <Genode/SceneGraph.hpp>

class ChartRenderer;
class Note;
class LongNote;
using Measure = Note;
class NoteContainer : public virtual Gx::Node, public Gx::RenderableContainer, public Gx::UpdatableContainer
{
public:
    NoteContainer();
    void Initialize(const ChartRenderer &renderer, const Chart& chart, Difficulty difficulty);

    void AddNote(Note& note);
    void AddMeasure(Measure& measure);
    Note* GetNote(Chart::Channel channel, Chart::NoteType type, double position) const;

    sf::FloatRect GetViewport() const;
    void SetViewport(const sf::FloatRect &viewport);

    sf::VertexArray& GetNoteVertices();
    sf::VertexArray& GetMeasureVertices();
    sf::VertexArray& GetGuideLineVertices();

    const sf::Texture* GetTexture(NoteShape shape);
    void SetTexture(NoteShape shape, const sf::Texture &texture);

    void RegisterPrefab(Gx::Updatable &prefab);
    std::unordered_set<Gx::Updatable*> GetRegisteredPrefabs();

    unsigned int GetLastMeasure() const;

    void Render(const Chart::NoteEvent& ev, double delta) const;
    void RenderMeasures(double delta) const;

private:
    using TextureMap = std::unordered_map<NoteShape, const sf::Texture*>;

    void Update(const double delta) override;
    Gx::RenderStates Render(Gx::RenderSurface &surface, Gx::RenderStates states) const override;

    const ChartRenderer*  m_renderer;
    const Chart*          m_chart;
    Difficulty            m_difficulty;
    sf::FloatRect         m_viewport;
    sf::VertexArray       m_noteVertices;
    sf::VertexArray       m_measureVertices;
    sf::VertexArray       m_guideLineVertices;

    std::vector<Note*> m_notes;
    std::vector<LongNote*> m_longNotes;
    std::vector<Note*> m_measures;
    std::unordered_set<Gx::Updatable*> m_prefabs;

    TextureMap m_textures;
    NoteShape  m_shape;

    unsigned int m_lastMeasure;
    mutable unsigned int m_tapCounter;
    mutable unsigned int m_longCounter;

};

#endif
