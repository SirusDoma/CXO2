#include <CXO2/UI/Waiting/MapSelector.hpp>
#include <CXO2/States/State.hpp>

#include <CXO2/Services/WaitingService.hpp>

#include <CXO2/StringTable/Identifiers/Sound.hpp>
#include <CXO2/StringTable/Identifiers/Map.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/Utilities/Randomizer.hpp>

namespace Cx
{
    using namespace StringTable::Identifiers;

    MapSelector::MapSelector(Gx::AudioMixer& mixer, Gx::ResourceManager& resources, const unsigned int mapId, const unsigned int effectId) :
        m_mixer(mixer),
        m_resources(resources),
        m_mapID(mapId),
        m_effectID(effectId)
    {
    }

    void MapSelector::Initialize()
    {
        Node::Initialize();

        const auto map       = FindChild<Gx::Image>(Resource::Map::IDC_IMAGE_MAP);
        const auto randomMap = FindChild<Gx::Image>(Resource::Map::IDC_IMAGE_RANDOM_MAP);
        map->SetFrame(0);

        if (randomMap)
        {
            randomMap->SetVisible(true);
            map->SetVisible(false);
        }

        const auto mapName = FindChild<Gx::Label>(Resource::Map::IDC_TEXT_MAP_NAME);
        mapName->SetString("Random");

        const auto effectGroup1 = FindChild<UiContainer>(Resource::Map::IDC_CONTAINER_EFFECT_1);
        for (const auto effect : effectGroup1->GetChildren())
        {
            if (const auto control = dynamic_cast<Gx::Animation*>(effect); control)
                control->SetVisible(false);
        }

        const auto effectGroup2 = FindChild<UiContainer>(Resource::Map::IDC_CONTAINER_EFFECT_2);
        for (const auto effect : effectGroup2->GetChildren())
        {
            if (const auto control = dynamic_cast<Gx::Animation*>(effect); control)
                control->SetVisible(false);
        }

        if (const auto effect1 = effectGroup1->FindChild<Gx::Animation>(Resource::Map::IDC_IMAGE_EFFECT0_1); effect1)
            effect1->SetVisible(true);

        if (const auto effect2 = effectGroup2->FindChild<Gx::Animation>(Resource::Map::IDC_IMAGE_EFFECT0_2); effect2)
            effect2->SetVisible(true);

        const auto mapLeftButton = FindChild<Gx::Button>(Resource::Map::IDC_BUTTON_MAP_LEFT);
        mapLeftButton->SetClickCallback([this] (auto& sender, auto& ev) { OnMapLeftButtonClicked(sender, ev); });

        const auto mapRightButton = FindChild<Gx::Button>(Resource::Map::IDC_BUTTON_MAP_RIGHT);
        mapRightButton->SetClickCallback([this] (auto& sender, auto& ev) { OnMapRightButtonClicked(sender, ev); });

        if (const auto mapLeftCover = FindChild<Gx::Image>(Resource::Map::IDC_IMAGE_COVER_LEFT))
            mapLeftCover->SetVisible(false);

        if (const auto mapRightCover = FindChild<Gx::Image>(Resource::Map::IDC_IMAGE_COVER_RIGHT))
            mapRightCover->SetVisible(false);

        const auto mapEffectTopButton = FindChild<Gx::RadioButton>(Resource::Map::IDC_RADIO_MAP_SELECT_TOP);
        mapEffectTopButton->SetCheckStateChangeCallback([this] (auto& sender) { OnMapSelectTopCheckChanged(sender); });

        const auto mapEffectBottomButton = FindChild<Gx::RadioButton>(Resource::Map::IDC_RADIO_MAP_SELECT_BOTTOM);
        mapEffectBottomButton->SetCheckStateChangeCallback([this] (auto& sender) { OnMapSelectBottomCheckChanged(sender); });
    }

    unsigned int MapSelector::GetMapID() const
    {
        return m_mapID;
    }

    unsigned int MapSelector::GetEffectID() const
    {
        return m_effectID;
    }

    void MapSelector::SetMapID(int mapID, const bool silent)
    {
        if (mapID == m_mapID)
            return;

        const auto sfxNavigate  = &m_resources.AddFromFile<sf::Sound>(Sound::Effects::EF_07);
        const auto map          = FindChild<Gx::Image>(Resource::Map::IDC_IMAGE_MAP);
        const auto randomMap    = FindChild<Gx::Image>(Resource::Map::IDC_IMAGE_RANDOM_MAP);
        const auto mapName      = FindChild<Gx::Label>(Resource::Map::IDC_TEXT_MAP_NAME);
        const auto effectGroup1 = FindChild<UiContainer>(Resource::Map::IDC_CONTAINER_EFFECT_1);
        const auto effectGroup2 = FindChild<UiContainer>(Resource::Map::IDC_CONTAINER_EFFECT_2);

        if (mapID < 0)
            mapID = map->GetFrameCount() - (randomMap ? 0 : 1);

        if (mapID - (randomMap ? 1 : 0) >= map->GetFrameCount())
            mapID = 0;

        for (const auto effect : effectGroup1->GetChildren())
        {
            if (const auto control = dynamic_cast<Gx::Animation*>(effect); control)
                control->SetVisible(false);
        }

        for (const auto effect : effectGroup2->GetChildren())
        {
            if (const auto control = dynamic_cast<Gx::Animation*>(effect); control)
                control->SetVisible(false);
        }

        m_mapID = mapID;
        if (randomMap)
        {
            if (m_mapID == 0)
            {
                randomMap->SetVisible(true);
                map->SetVisible(false);

                mapName->SetString("Random");
            }
            else
            {
                randomMap->SetVisible(false);
                map->SetVisible(true);
                map->SetFrame(m_mapID - 1);

                mapName->SetString(map->GetCurrentFrameName());
            }
        }
        else
        {
            map->SetFrame(m_mapID);
            mapName->SetString(map->GetCurrentFrameName());
        }

        auto animation = effectGroup1->FindChild<Gx::Animation>(Resource::Map::IDC_IMAGE_EFFECT_I_1(m_mapID));
        if (!animation)
            animation = effectGroup1->FindChild<Gx::Animation>(Resource::Map::IDC_IMAGE_EFFECT0_1);

        if (animation)
            animation->SetVisible(true);

        animation = effectGroup2->FindChild<Gx::Animation>(Resource::Map::IDC_IMAGE_EFFECT_I_2(m_mapID));
        if (!animation)
            animation = effectGroup1->FindChild<Gx::Animation>(Resource::Map::IDC_IMAGE_EFFECT0_2);

        if (animation)
            animation->SetVisible(true);

        if (!silent)
        {
            m_mixer.Play(*sfxNavigate, Sound::Channel::SFX);

            if (m_mapCallback)
                m_mapCallback(m_mapID);
        }
    }

    void MapSelector::SetEffectID(const unsigned int effectID)
    {
        const auto mapEffectTopButton = FindChild<Gx::RadioButton>(Resource::Map::IDC_RADIO_MAP_SELECT_TOP);
        const auto mapEffectBottomButton = FindChild<Gx::RadioButton>(Resource::Map::IDC_RADIO_MAP_SELECT_BOTTOM);

        mapEffectTopButton->SetCheckedState(effectID <= 1);
        mapEffectBottomButton->SetCheckedState(effectID == 2);

        if (m_effectID == effectID)
            return;

        m_effectID = effectID;
        if (m_effectCallback)
            m_effectCallback(m_effectID);
    }

    void MapSelector::SetControlsEnabled(const bool enabled) const
    {
        if (const auto leftButton = FindChild<Gx::Button>(Resource::Map::IDC_BUTTON_MAP_LEFT))
        {
            leftButton->SetEnabled(enabled);
            leftButton->SetVisible(enabled);
        }

        if (const auto rightButton = FindChild<Gx::Button>(Resource::Map::IDC_BUTTON_MAP_RIGHT))
        {
            rightButton->SetEnabled(enabled);
            rightButton->SetVisible(enabled);
        }

        if (const auto leftCover = FindChild<Gx::Image>(Resource::Map::IDC_IMAGE_COVER_LEFT))

            leftCover->SetVisible(!enabled);

        if (const auto rightCover = FindChild<Gx::Image>(Resource::Map::IDC_IMAGE_COVER_RIGHT))
            rightCover->SetVisible(!enabled);
    }

    std::size_t MapSelector::GetMapCount() const
    {
        const auto map = FindChild<Gx::Image>(Resource::Map::IDC_IMAGE_MAP);
        return !map ? 0 : map->GetFrameCount();
    }

    void MapSelector::SetMapChangedCallback(const std::function<void(unsigned int)> &callback)
    {
        m_mapCallback = callback;
    }

    void MapSelector::SetEffectChangedCallback(const std::function<void(unsigned int)> &callback)
    {
        m_effectCallback = callback;
    }

    void MapSelector::OnMapLeftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        SetMapID(static_cast<int>(m_mapID) - 1);
    }

    void MapSelector::OnMapRightButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        SetMapID(static_cast<int>(m_mapID) + 1);
    }

    void MapSelector::OnMapSelectTopCheckChanged(Gx::RadioButton& sender)
    {
        if (sender.IsChecked())
            SetEffectID(1);
    }

    void MapSelector::OnMapSelectBottomCheckChanged(Gx::RadioButton& sender)
    {
        if (sender.IsChecked())
            SetEffectID(2);
    }
}
