#ifndef O2JAM_CHART_LONG_NOTE_HPP
#define O2JAM_CHART_LONG_NOTE_HPP

#include <OTwo/Chart/Note.hpp>
#include <OTwo/Chart/Chart.hpp>
#include <OTwo/Chart/ChartRenderer.hpp>
#include <OTwo/Models/Game.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>

class LongNote : public virtual Note
{
public:
    LongNote(const ChartRenderer &renderer, const Chart::NoteEvent &ev, const NoteSpriteMap &sprites, const NoteSpriteMap &heads);
    LongNote(const ChartRenderer &renderer, Chart::Channel channel, double position, double length, const NoteSpriteMap &sprites, const NoteSpriteMap &heads);

    bool IsVisible() const override;
    void SetVisible(bool visible) override;

private:
    Gx::RenderStates Render(Gx::RenderSurface &surface, Gx::RenderStates states) const override;
    void Update(const double delta) override;

    const double  m_position;
    const double  m_length;
    NoteSpriteMap m_sprites;
    NoteSpriteMap m_heads;

    bool m_visible;
    Accuracy m_accuracy;
};

#endif
