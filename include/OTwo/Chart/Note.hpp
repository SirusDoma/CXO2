#ifndef O2JAM_CHART_NOTE_HPP
#define O2JAM_CHART_NOTE_HPP

#include <OTwo/Chart/Chart.hpp>
#include <OTwo/Chart/ChartRenderer.hpp>
#include <OTwo/Models/Game.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>

class Note : public virtual Gx::Node, public Gx::RenderableContainer, public Gx::UpdatableContainer
{
public:
    Note(const ChartRenderer &renderer, const Chart::Event &ev, const NoteSpriteMap &sprites);
    Note(const ChartRenderer &renderer, Chart::Channel channel, double position, const NoteSpriteMap &sprites);

    bool IsVisible() const override;
    void SetVisible(bool visible) override;

    Accuracy GetJudgementAccuracy() const;
    void Judge(Accuracy accuracy);

protected:
    Note(const ChartRenderer &renderer, const Chart::Channel channel, const double position);

    Gx::RenderStates Render(Gx::RenderSurface &surface, Gx::RenderStates states) const override;
    void Update(const double delta) override;

    const ChartRenderer  *m_renderer;
    const Chart::Channel  m_channel;
    const double          m_position;
    GameConfig           *m_config;

private:
    NoteSpriteMap         m_sprites;

    bool m_visible;
    Accuracy m_accuracy;
};

#endif
