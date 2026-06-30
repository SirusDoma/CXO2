#pragma once

#include <CXO2/Core/Chart.hpp>
#include <CXO2/Core/NoteContainer.hpp>
#include <CXO2/Core/ChartRenderer.hpp>

namespace Cx
{
    using PrefabMap = std::unordered_map<Chart::Channel, std::unordered_map<NoteShape, Gx::Sprite*>>;

    class NoteFactory
    {
    public:
        explicit NoteFactory(Gx::ResourceManager& resources, const ChannelSet& instantiables = {
            Chart::Channel::Note1,
            Chart::Channel::Note2,
            Chart::Channel::Note3,
            Chart::Channel::Note4,
            Chart::Channel::Note5,
            Chart::Channel::Note6,
            Chart::Channel::Note7
        });

        NoteFactory(Gx::ResourceManager& instantiationResources, Gx::ResourceManager& prefabResources, const ChannelSet& instantiables);

        NoteContainer& Generate(const ChartRenderer::RenderSettings& settings) const;

    private:
        Gx::ResourceManager* m_resources, *m_prefabResources;
        ChannelSet           m_channels;
    };
}
