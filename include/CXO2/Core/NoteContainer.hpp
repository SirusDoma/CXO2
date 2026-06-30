#pragma once

#include <CXO2/Core/Chart.hpp>
#include <CXO2/Config/GameConfig.hpp>

#include <Genode/SceneGraph.hpp>
#include <Genode/Graphics/VertexPool.hpp>

namespace Cx
{
    class ChartRenderer;
    class Note;
    class LongNote;
    using Measure = Note;
    class NoteContainer : public virtual Gx::Node, public Gx::RenderableContainer, public Gx::UpdatableContainer
    {
    public:
        NoteContainer();
        void Initialize(const ChartRenderer& renderer, const Chart& chart, Difficulty difficulty);

        void AddNote(Note& note);
        void AddMeasure(Measure& measure);
        Note* GetNote(Chart::Channel channel, Chart::NoteType type, double position) const;

        sf::FloatRect GetViewport() const;
        void SetViewport(const sf::FloatRect& viewport);

        Gx::VertexPool& GetNoteVertexPool();
        Gx::VertexPool& GetGuideLineVertexPool();

        const sf::Texture* GetTexture(NoteShape shape);
        void SetTexture(NoteShape shape, const sf::Texture& texture);

        void RegisterPrefab(Updatable& prefab);
        std::unordered_set<Updatable*> GetRegisteredPrefabs();

        void UpdateGeometry(const Chart::NoteEvent& ev, const sf::Time& delta) const;
        void UpdateMeasures(const sf::Time& delta) const;

    private:
        using TextureMap = std::unordered_map<NoteShape, const sf::Texture*>;

        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        const ChartRenderer*  m_renderer;
        const Chart*          m_chart;
        Difficulty            m_difficulty;
        sf::FloatRect         m_viewport;

        Gx::VertexPool m_noteVertices;
        Gx::VertexPool m_guideVertices;

        std::vector<Note*> m_notes;
        std::vector<LongNote*> m_longNotes;
        std::vector<Note*> m_measures;
        std::unordered_set<Updatable*> m_prefabs;

        TextureMap m_textures;
        NoteShape  m_shape;

        mutable unsigned int m_tapCounter;
        mutable unsigned int m_longCounter;

    };
}
