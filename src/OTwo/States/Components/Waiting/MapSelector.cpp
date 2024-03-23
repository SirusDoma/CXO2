#include <OTwo/States/Components/Waiting/MapSelector.hpp>

#include <Genode/UI.hpp>
#include <OTwo/States/State.hpp>
#include <Genode/Graphics/Animation.hpp>

MapSelector::MapSelector(Gx::UiContainer &&copy) noexcept :
    Gx::UiContainer(copy),
    Gx::Node(copy),
    m_mapID(),
    m_effectID()
{
}

void MapSelector::Initialize()
{
    Node::Initialize();

    auto map = FindChild<Gx::Image>("IDC_IMAGE_MAP");
    map->SetFrame(0);

    auto mapName = FindChild<Gx::Label>("IDC_TEXT_MAP_NAME");
    mapName->SetString("Random");

    auto effectGroup1 = FindChild<Gx::UiContainer>("IDC_CONTAINER_EFFECT_1");
    for (auto effect : effectGroup1->GetChildren())
    {
        if (auto control = dynamic_cast<Gx::Animation*>(effect); control)
            control->SetVisible(false);
    }

    auto effectGroup2 = FindChild<Gx::UiContainer>("IDC_CONTAINER_EFFECT_2");
    for (auto effect : effectGroup2->GetChildren())
    {
        if (auto control = dynamic_cast<Gx::Animation*>(effect); control)
            control->SetVisible(false);
    }

    if (auto effect1 = effectGroup1->FindChild<Gx::Animation>("IDC_IMAGE_EFFECT0_1"); effect1)
        effect1->SetVisible(true);

    if (auto effect2 = effectGroup2->FindChild<Gx::Animation>("IDC_IMAGE_EFFECT0_2"); effect2)
        effect2->SetVisible(true);

    auto mapLeftButton = FindChild<Gx::Button>("IDC_BUTTON_MAP_LEFT");
    mapLeftButton->SetClickCallback([=] (auto& sender, auto& ev){
        SetMapID(m_mapID - 1);
    });

    auto mapRightButton = FindChild<Gx::Button>("IDC_BUTTON_MAP_RIGHT");
    mapRightButton->SetClickCallback([=] (auto& sender, auto& ev){
       SetMapID(m_mapID + 1);
    });

    auto mapEffectTopButton = FindChild<Gx::RadioButton>("IDC_RADIO_MAP_SELECT_TOP");
    mapEffectTopButton->SetCheckStateChangeCallback([=] (auto sender)
    {
        if (sender->IsChecked())
            SetEffectID(1);
    });

    auto mapEffectBottomButton = FindChild<Gx::RadioButton>("IDC_RADIO_MAP_SELECT_BOTTOM");
    mapEffectBottomButton->SetCheckStateChangeCallback([=] (auto sender)
    {
        if (sender->IsChecked())
            SetEffectID(2);
    });

    SetEffectID(1);
}

unsigned int MapSelector::GetMapID() const
{
    return m_mapID;
}

unsigned int MapSelector::GetEffectID() const
{
    return m_effectID;
}

void MapSelector::SetMapID(int mapID)
{
    auto parent = GetParent<::State>();
    if (!parent)
        return;

    auto& app   = Gx::Application::Instance();
    auto& mixer = app.Require<Gx::Mixer>();

    auto sfxNavigate  = parent->Load<sf::Sound>("STATE_WAITING/IDC_SOUND_07");
    auto map          = FindChild<Gx::Image>("IDC_IMAGE_MAP");
    auto mapName      = FindChild<Gx::Label>("IDC_TEXT_MAP_NAME");
    auto effectGroup1 = FindChild<Gx::UiContainer>("IDC_CONTAINER_EFFECT_1");
    auto effectGroup2 = FindChild<Gx::UiContainer>("IDC_CONTAINER_EFFECT_2");

    if (mapID < 0)
        mapID = map->GetFrameCount() - 1;

    if (mapID >= map->GetFrameCount())
        mapID = 0;

    if (mapID == m_mapID)
        return;

    for (auto effect : effectGroup1->GetChildren())
    {
        if (auto control = dynamic_cast<Gx::Animation*>(effect); control)
            control->SetVisible(false);
    }

    for (auto effect : effectGroup2->GetChildren())
    {
        if (auto control = dynamic_cast<Gx::Animation*>(effect); control)
            control->SetVisible(false);
    }

    m_mapID = mapID;
    map->SetFrame(m_mapID);
    mapName->SetString(map->GetCurrentFrameName());

    auto animation = effectGroup1->FindChild<Gx::Animation>("IDC_IMAGE_EFFECT" + std::to_string(m_mapID) + "_1");
    if (!animation)
        animation = effectGroup1->FindChild<Gx::Animation>("IDC_IMAGE_EFFECT0_1");

    if (animation)
        animation->SetVisible(true);

    animation = effectGroup2->FindChild<Gx::Animation>("IDC_IMAGE_EFFECT" + std::to_string(m_mapID) + "_2");
    if (!animation)
        animation = effectGroup1->FindChild<Gx::Animation>("IDC_IMAGE_EFFECT0_2");

    if (animation)
        animation->SetVisible(true);

    mixer.Play(sfxNavigate);
}

void MapSelector::SetEffectID(unsigned int effectID)
{
    if (m_effectID == effectID)
        return;

    m_effectID = effectID;
    auto mapEffectTopButton = FindChild<Gx::RadioButton>("IDC_RADIO_MAP_SELECT_TOP");
    auto mapEffectBottomButton = FindChild<Gx::RadioButton>("IDC_RADIO_MAP_SELECT_BOTTOM");

    if (m_effectID == 1)
        mapEffectTopButton->SetCheckedState(true);
    else
        mapEffectBottomButton->SetCheckedState(true);
}
