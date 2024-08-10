#ifndef O2JAM_CHART_NOTE_FACTORY_HPP
#define O2JAM_CHART_NOTE_FACTORY_HPP

#include <OTwo/Chart/Chart.hpp>
#include <OTwo/Chart/NoteContainer.hpp>
#include <OTwo/Chart/ChartRenderer.hpp>

#include <Genode/IO/ResourceManager.hpp>

class NoteFactory
{
public:
    explicit NoteFactory(Gx::ResourceManager &resources, const ChannelSet &instantiables = {
        Chart::Channel::Note1,
        Chart::Channel::Note2,
        Chart::Channel::Note3,
        Chart::Channel::Note4,
        Chart::Channel::Note5,
        Chart::Channel::Note6,
        Chart::Channel::Note7
    });

    NoteFactory(Gx::ResourceManager &instantiationResources, Gx::ResourceManager &prefabResources, const ChannelSet &instantiables);

    NoteContainer *Generate(const Chart &chart, const ChartRenderer::RenderSettings &settings) const;

private:
    using PrefabMap = std::unordered_map<Chart::Channel, std::unordered_map<NoteShape, Gx::Sprite*>>;
    Gx::ResourceManager *m_resources, *m_prefabResources;
    ChannelSet           m_channels;
};

#endif
