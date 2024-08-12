#ifndef O2JAM_CORE_NOTE_HPP
#define O2JAM_CORE_NOTE_HPP

#include <OTwo/Core/Chart.hpp>
#include <OTwo/Config/GameConfig.hpp>

#include <Genode/Entities/Renderable.hpp>
#include <Genode/Graphics/Sprite.hpp>
#include <OTwo/Core/NoteGuideLine.hpp>

static constexpr unsigned int DefaultMeasureHeight = 384;

class ChartRenderer;
class Note : public Gx::Renderable
{
public:
    explicit Note(const Chart::NoteEvent& ev);
    explicit Note(double position, Chart::Channel channel = Chart::Channel::Background);

    double GetRenderPosition() const;
    Chart::Channel GetChannel() const;
    NoteGuideLine* GetGuideLine();

    const std::array<sf::Vertex*, 6>& GetVertices() const;
    void SetVertices(const std::array<sf::Vertex*, 6>& vertices);

    const Gx::Sprite* GetPrefab(NoteShape shape) const;
    void SetPrefab(NoteShape shape, Gx::Sprite& prefab);

    bool IsVisible() const override;
    void SetVisible(const bool visible) override;

    void Hit();
    virtual void Render(const ChartRenderer &renderer, double delta);

protected:
    using VerticesPtr = std::array<sf::Vertex*, 6>;
    using PrefabMap   = std::unordered_map<NoteShape, Gx::Sprite*>;

    static void UpdatePositions(const VerticesPtr& vertices, const sf::Vector2f &position, const sf::FloatRect &bounds);
    static void UpdateTexCoords(const VerticesPtr& vertices, const sf::IntRect &texcoords);

private:
    Gx::RenderStates Render(Gx::RenderSurface &surface, Gx::RenderStates states) const override;

    VerticesPtr    m_vertices;
    PrefabMap      m_prefabs;
    double         m_position;
    Chart::Channel m_channel;
    NoteGuideLine  m_line;
    bool           m_hit;
};

using Measure = Note;

#endif //NOTE_HPP
