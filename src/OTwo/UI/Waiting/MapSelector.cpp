#include <OTwo/UI/Waiting/MapSelector.hpp>
#include <OTwo/States/State.hpp>

#include <OTwo/StringTable/Identifiers/Sound.hpp>
#include <OTwo/StringTable/Identifiers/Map.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/Image.hpp>

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

    const auto map = FindChild<Gx::Image>(Resource::Map::IDC_IMAGE_MAP);
    map->SetFrame(0);

    const auto mapName = FindChild<Gx::Label>(Resource::Map::IDC_TEXT_MAP_NAME);
    mapName->SetString("Random");

    const auto effectGroup1 = FindChild<Gx::UiContainer>(Resource::Map::IDC_CONTAINER_EFFECT_1);
    for (const auto effect : effectGroup1->GetChildren())
    {
        if (const auto control = dynamic_cast<Gx::Animation*>(effect); control)
            control->SetVisible(false);
    }

    const auto effectGroup2 = FindChild<Gx::UiContainer>(Resource::Map::IDC_CONTAINER_EFFECT_2);
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
    mapLeftButton->SetClickCallback([=] (auto& sender, auto& ev){
        SetMapID(static_cast<int>(m_mapID) - 1);
    });

    const auto mapRightButton = FindChild<Gx::Button>(Resource::Map::IDC_BUTTON_MAP_RIGHT);
    mapRightButton->SetClickCallback([=] (auto& sender, auto& ev){
       SetMapID(static_cast<int>(m_mapID) + 1);
    });

    const auto mapEffectTopButton = FindChild<Gx::RadioButton>(Resource::Map::IDC_RADIO_MAP_SELECT_TOP);
    mapEffectTopButton->SetCheckStateChangeCallback([=] (auto& sender)
    {
        if (sender.IsChecked())
            SetEffectID(1);
    });

    const auto mapEffectBottomButton = FindChild<Gx::RadioButton>(Resource::Map::IDC_RADIO_MAP_SELECT_BOTTOM);
    mapEffectBottomButton->SetCheckStateChangeCallback([=] (auto& sender)
    {
        if (sender.IsChecked())
            SetEffectID(2);
    });
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
    const auto sfxNavigate  = &m_resources.AddFromFile<sf::Sound>(Sound::Effects::EF_07);
    const auto map          = FindChild<Gx::Image>(Resource::Map::IDC_IMAGE_MAP);
    const auto mapName      = FindChild<Gx::Label>(Resource::Map::IDC_TEXT_MAP_NAME);
    const auto effectGroup1 = FindChild<Gx::UiContainer>(Resource::Map::IDC_CONTAINER_EFFECT_1);
    const auto effectGroup2 = FindChild<Gx::UiContainer>(Resource::Map::IDC_CONTAINER_EFFECT_2);

    if (mapID < 0)
        mapID = static_cast<unsigned int>(map->GetFrameCount() - 1);

    if (mapID >= map->GetFrameCount())
        mapID = 0;

    if (mapID == m_mapID)
        return;

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
    map->SetFrame(m_mapID);
    mapName->SetString(map->GetCurrentFrameName());

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
        m_mixer.Play(*sfxNavigate, Sound::Channel::SFX);

    if (m_mapCallback)
        m_mapCallback(m_mapID);
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

void MapSelector::SetMapChangedCallback(const std::function<void(unsigned int)> &callback)
{
    m_mapCallback = callback;
}

void MapSelector::SetEffectChangedCallback(const std::function<void(unsigned int)> &callback)
{
    m_effectCallback = callback;
}
