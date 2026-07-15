#pragma once

#include <Genode/UI/UiContainer.hpp>

namespace Gx
{
    class AudioMixer;
    class RadioButton;
    class ResourceManager;
}

namespace Cx
{

    class WaitingService;
    class RoomContext;
    class MapSelector : public Gx::UiContainer
    {
    public:
        MapSelector(Gx::AudioMixer& mixer, Gx::ResourceManager& resources, unsigned int mapId, unsigned int effectId);

        void Initialize() override;

        unsigned int GetMapID() const;
        unsigned int GetEffectID() const;

        void SetMapID(int mapID, bool silent = false);
        void SetEffectID(unsigned int effectID);

        void SetMapChangedCallback(const std::function<void(unsigned int)> &callback);
        void SetEffectChangedCallback(const std::function<void(unsigned int)> &callback);

        void SetControlsEnabled(bool enabled) const;

        std::size_t GetMapCount() const;

    private:
        void OnMapLeftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMapRightButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMapSelectTopCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev);
        void OnMapSelectBottomCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev);

        Gx::AudioMixer& m_mixer;
        Gx::ResourceManager& m_resources;

        unsigned int m_mapID;
        unsigned int m_effectID;

        std::function<void(unsigned int)> m_mapCallback;
        std::function<void(unsigned int)> m_effectCallback;
    };
}
