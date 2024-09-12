#ifndef O2JAM_CORE_NOTE_HPP
#define O2JAM_CORE_NOTE_HPP

#include <OTwo/Core/Chart.hpp>
#include <OTwo/Config/GameConfig.hpp>

#include <OTwo/Core/NoteFactory.hpp>
#include <OTwo/Core/NoteGuideLine.hpp>

#include <Genode/Entities/Renderable.hpp>
#include <Genode/Graphics/Sprite.hpp>

static constexpr unsigned int DefaultMeasureHeight = 384;

class ChartRenderer;
class Note : public Gx::Renderable
{
public:
    explicit Note(const Chart::NoteEvent& ev);
    explicit Note(double position, Chart::Channel channel = Chart::Channel::Background);

    double GetRenderPosition() const;
    void SetRenderPosition(double position);

    Chart::Channel GetChannel() const;
    void SetChannel(Chart::Channel channel);

    NoteGuideLine* GetGuideLine();

    const std::array<sf::Vertex*, 6>& GetVertices() const;
    void SetVertices(const std::array<sf::Vertex*, 6>& vertices);

    const Gx::Sprite* GetPrefab(NoteShape shape) const;
    void SetPrefabs(const PrefabMap& prefabs);

    bool IsVisible() const override;
    void SetVisible(const bool visible) override;

    virtual void Render(const ChartRenderer& renderer, double delta);

protected:
    using VerticesPtr = std::array<sf::Vertex*, 6>;

    static void UpdatePositions(const VerticesPtr& vertices, const sf::Vector2f& position, const sf::FloatRect& bounds);
    static void UpdateTexCoords(const VerticesPtr& vertices, const sf::IntRect& texcoords);

private:
    Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

    VerticesPtr    m_vertices;
    PrefabMap      m_prefabs;
    double         m_position;
    Chart::Channel m_channel;
    NoteGuideLine  m_line;
};

#endif //NOTE_HPP
