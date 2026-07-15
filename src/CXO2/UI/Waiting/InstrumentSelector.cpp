#include <CXO2/UI/Waiting/InstrumentSelector.hpp>
#include <CXO2/Avatar/ItemFactory.hpp>

#include <CXO2/StringTable/Identifiers/Sound.hpp>
#include <CXO2/StringTable/Identifiers/Instrument.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/Image.hpp>

namespace Cx
{
    using namespace StringTable::Identifiers;

    InstrumentSelector::InstrumentSelector(Gx::AudioMixer& mixer, Gx::ResourceManager& resources, ItemFactory& items) :
        m_mixer(mixer),
        m_resources(resources),
        m_items(items),
        m_currentItem(),
        m_currentItemHeader(),
        m_currentInstrument(),
        m_currentIndex(0)
    {
    }

    void InstrumentSelector::Initialize()
    {
        Node::Initialize();

        m_resources.AddFromFile<sf::Sound>(Sound::Effects::EF_07);
        if (const auto previousButton = FindChild<Gx::Button>(Resource::Instrument::IDC_BUTTON_INSTRUMENT_LEFT); previousButton)
            previousButton->SetClickCallback([this] (auto& sender, auto& ev) { OnInstrumentLeftButtonClicked(sender, ev); });

        if (const auto nextButton = FindChild<Gx::Button>(Resource::Instrument::IDC_BUTTON_INSTRUMENT_RIGHT); nextButton)
            nextButton->SetClickCallback([this] (auto& sender, auto& ev) { OnInstrumentRightButtonClicked(sender, ev); });

        if (const auto guitar = FindChild<Gx::RadioButton>(Resource::Instrument::IDC_RADIO_GUITAR); guitar)
        {
            guitar->SetVisible(false);
            guitar->SetEnabled(false);
            guitar->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnGuitarCheckChanged(sender, ev); });
        }

        if (const auto bass = FindChild<Gx::RadioButton>(Resource::Instrument::IDC_RADIO_BASS); bass)
        {
            bass->SetVisible(false);
            bass->SetEnabled(false);
            bass->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnBassCheckChanged(sender, ev); });
        }

        if (const auto drum = FindChild<Gx::RadioButton>(Resource::Instrument::IDC_RADIO_DRUM); drum)
        {
            drum->SetVisible(false);
            drum->SetEnabled(false);
            drum->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnDrumCheckChanged(sender, ev); });
        }

        if (const auto keyboard = FindChild<Gx::RadioButton>(Resource::Instrument::IDC_RADIO_KEYBOARD); keyboard)
        {
            keyboard->SetVisible(false);
            keyboard->SetEnabled(false);
            keyboard->SetCheckStateChangeCallback([this] (auto& sender, auto& ev) { OnKeyboardCheckChanged(sender, ev); });
        }

        Invalidate();
    }

    void InstrumentSelector::AddInstrumentMetadata(const ItemMetadata& item)
    {
        auto instrument = Instrument::None;
        switch(item.EquipmentType)
        {
            case EquipmentType::Bass:     instrument = Instrument::Bass;     break;
            case EquipmentType::Guitar:   instrument = Instrument::Guitar;   break;
            case EquipmentType::Keyboard: instrument = Instrument::Keyboard; break;
            case EquipmentType::Drum:     instrument = Instrument::Drum;     break;
            default:                      instrument = Instrument::None;     break;
        }

        if (instrument == Instrument::None)
            return;

        const auto key = instrument;
        if (const auto it = m_headers.find(key); it == m_headers.end())
            m_headers[key] = std::vector<ItemMetadata>();

        for (const auto i : m_headers[key])
        {
            if (i.ID == item.ID)
                return;
        }

        m_headers[key].push_back(item);
        Gx::RadioButton* button = nullptr;
        switch (key)
        {
            case Instrument::Guitar:   button = FindChild<Gx::RadioButton>(Resource::Instrument::IDC_RADIO_GUITAR);   break;
            case Instrument::Bass:     button = FindChild<Gx::RadioButton>(Resource::Instrument::IDC_RADIO_BASS);     break;
            case Instrument::Drum:     button = FindChild<Gx::RadioButton>(Resource::Instrument::IDC_RADIO_DRUM);     break;
            case Instrument::Keyboard: button = FindChild<Gx::RadioButton>(Resource::Instrument::IDC_RADIO_KEYBOARD); break;
            default: break;
        }

        if (button)
        {
            button->SetVisible(true);
            button->SetEnabled(true);

            bool init = false;
            for (const auto child : GetChildren())
            {
                if (const auto radio = dynamic_cast<Gx::RadioButton*>(child); radio && radio->IsChecked())
                {
                    init = true;
                    break;
                }
            }

            if (!init)
            {
                button->SetCheckedState(true);
                Invalidate();
            }
        }
    }

    const ItemMetadata& InstrumentSelector::GetInstrumentMetadata() const
    {
        return m_currentItemHeader;
    }

    void InstrumentSelector::SetSelectedInstrument(int itemID)
    {
        for (auto& [_, items] : m_headers)
        {
            m_currentIndex = 0;
            for (const auto item : items)
            {
                m_currentIndex++;
                if (item.ID == itemID)
                {
                    switch(item.EquipmentType)
                    {
                        case EquipmentType::Bass:     m_currentInstrument = Instrument::Bass;     break;
                        case EquipmentType::Guitar:   m_currentInstrument = Instrument::Guitar;   break;
                        case EquipmentType::Keyboard: m_currentInstrument = Instrument::Keyboard; break;
                        case EquipmentType::Drum:     m_currentInstrument = Instrument::Drum;     break;
                        default:                      m_currentInstrument = Instrument::None;     break;
                    }

                    if (m_currentInstrument != Instrument::None)
                        m_currentItemHeader = item;

                    return;
                }
            }
        }

        m_currentIndex = 0;
        Invalidate();
    }

    void InstrumentSelector::SetInstrumentSelectCallback(const std::function<void(const ItemMetadata&)>& callback) const
    {
        if (const auto selectButton = FindChild<Gx::Button>(Resource::Instrument::IDC_BUTTON_INSTRUMENT_SELECT); selectButton)
        {
            m_selectCallback = callback;
            selectButton->SetClickCallback([this] (auto& sender, auto& ev) { OnInstrumentSelectButtonClicked(sender, ev); });
        }
    }

    void InstrumentSelector::Invalidate()
    {
        UiContainer::Invalidate();

        const auto items = m_headers[m_currentInstrument];
        const auto instrumentThumbnail = FindChild<Gx::Image>(Resource::Instrument::IDC_IMAGE_INSTRUMENT);
        if (!instrumentThumbnail)
            return;

        const auto instrumentLabel = FindChild<Gx::Label>(Resource::Instrument::IDC_TEXT_INSTRUMENT_NAME);
        if (m_currentIndex >= static_cast<int>(items.size()))
            m_currentIndex = 0;

        if (m_currentIndex < 0)
            m_currentIndex = static_cast<int>(items.size()) - 1;

        if (m_currentIndex >= items.size())
        {
            m_currentItemHeader = ItemMetadata();
            instrumentThumbnail->SetVisible(false);
            if (instrumentLabel)
                instrumentLabel->SetString("(None)");

            return;
        }

        m_currentItemHeader = items[m_currentIndex];
        m_currentItem = m_items.Create(m_currentItemHeader.ID);
        if (m_currentItem.GetID() == 0)
            return;

        if (const auto texture = m_currentItem.GetLargeThumbnail().GetTexture(); texture)
        {
            instrumentThumbnail->SetVisible(true);
            instrumentThumbnail->SetTexture(*texture);
            instrumentThumbnail->SetTexCoords(m_currentItem.GetLargeThumbnail().GetTexCoords());

            if (instrumentLabel)
                instrumentLabel->SetString(m_currentItem.GetName());
        }
    }

    void InstrumentSelector::SelectInstrument(Gx::RadioButton& sender, const Instrument instrument)
    {
        if (!sender.IsChecked() || m_currentInstrument == instrument)
            return;

        m_currentIndex = 0;
        m_currentInstrument = instrument;
        Invalidate();
    }

    void InstrumentSelector::OnInstrumentLeftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto sfxNavigate = &m_resources.AddFromFile<sf::Sound>(Sound::Effects::EF_07);
        m_mixer.Play(*sfxNavigate, Sound::Channel::SFX);
        if (m_currentInstrument == Instrument::None)
            return;

        m_currentIndex--;
        Invalidate();
    }

    void InstrumentSelector::OnInstrumentRightButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto sfxNavigate = &m_resources.AddFromFile<sf::Sound>(Sound::Effects::EF_07);
        m_mixer.Play(*sfxNavigate, Sound::Channel::SFX);
        if (m_currentInstrument == Instrument::None)
            return;

        m_currentIndex++;
        Invalidate();
    }

    void InstrumentSelector::OnGuitarCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev)
    {
        SelectInstrument(sender, Instrument::Guitar);
    }

    void InstrumentSelector::OnBassCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev)
    {
        SelectInstrument(sender, Instrument::Bass);
    }

    void InstrumentSelector::OnDrumCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev)
    {
        SelectInstrument(sender, Instrument::Drum);
    }

    void InstrumentSelector::OnKeyboardCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev)
    {
        SelectInstrument(sender, Instrument::Keyboard);
    }

    void InstrumentSelector::OnInstrumentSelectButtonClicked(Gx::Control& sender, Gx::Control::Event& ev) const
    {
        m_selectCallback(m_currentItemHeader);
    }
}
