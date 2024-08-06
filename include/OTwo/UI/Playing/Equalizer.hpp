#ifndef O2JAM_PLAYING_EQUALIZER_HPP
#define O2JAM_PLAYING_EQUALIZER_HPP

#include <Genode/UI/Gauge.hpp>
#include <Genode/UI/List.hpp>

class Equalizer : virtual Gx::List, public virtual Gx::Control
{
public:
    Equalizer();
    Equalizer(int count, float spacing);

    void OnControlChildAdded(Gx::Control *control) override;
    void OnControlChildRemove(Gx::Control *control) override;

private:
    std::vector<Gx::Gauge*> m_gauges;
};

#endif
