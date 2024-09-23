#include <OTwo/UI/Waiting/InstrumentSelector.hpp>

#include <Genode/System/Application.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/Image.hpp>

InstrumentSelector::InstrumentSelector(Gx::Mixer& mixer, Gx::ResourceManager& resources) :
    m_mixer(mixer),
    m_resources(resources),
    m_currentItem(nullptr),
    m_currentInstrument(),
    m_currentIndex(0)
{
}

void InstrumentSelector::Initialize()
{
    Node::Initialize();

    const auto sfxNavigate = &m_resources.AddFromFile<sf::Sound>("bgEffect/07");
    if (const auto previousButton = FindChild<Gx::Button>("IDC_BUTTON_INSTRUMENT_LEFT"); previousButton)
    {
        previousButton->SetClickCallback([this, sound = sfxNavigate] (auto& sender, auto& ev)
        {
            m_mixer.Play(sound);
            if (m_currentInstrument == Instrument::None)
                return;

            m_currentIndex--;
            Invalidate();
        });
    }

    if (const auto nextButton = FindChild<Gx::Button>("IDC_BUTTON_INSTRUMENT_RIGHT"); nextButton)
    {
        nextButton->SetClickCallback([this, sound = sfxNavigate] (auto& sender, auto& ev)
        {
            m_mixer.Play(sound);
            if (m_currentInstrument == Instrument::None)
                return;

            m_currentIndex++;
            Invalidate();
        });
    }

    if (const auto guitar = FindChild<Gx::RadioButton>("IDC_RADIO_GUITAR"); guitar)
    {
        guitar->SetEnabled(false);
        guitar->SetCheckStateChangeCallback([this] (auto& sender)
        {
            if (!sender.IsChecked() || m_currentInstrument == Instrument::Guitar)
                return;

            m_currentIndex = 0;
            m_currentInstrument = Instrument::Guitar;
            Invalidate();
        });
    }

    if (const auto bass = FindChild<Gx::RadioButton>("IDC_RADIO_BASS"); bass)
    {
        bass->SetEnabled(false);
        bass->SetCheckStateChangeCallback([this] (auto& sender)
        {
            if (!sender.IsChecked() || m_currentInstrument == Instrument::Bass)
                return;

            m_currentIndex = 0;
            m_currentInstrument = Instrument::Bass;
            Invalidate();
        });
    }

    if (const auto keyboard = FindChild<Gx::RadioButton>("IDC_RADIO_KEYBOARD"); keyboard)
    {
        keyboard->SetEnabled(false);
        keyboard->SetCheckStateChangeCallback([this] (auto& sender)
        {
            if (!sender.IsChecked() || m_currentInstrument == Instrument::Keyboard)
                return;

            m_currentIndex = 0;
            m_currentInstrument = Instrument::Keyboard;
            Invalidate();
        });
    }

    if (const auto drum = FindChild<Gx::RadioButton>("IDC_RADIO_DRUM"); drum)
    {
        drum->SetEnabled(false);
        drum->SetCheckStateChangeCallback([this] (auto& sender)
        {
            if (!sender.IsChecked() || m_currentInstrument == Instrument::Drum)
                return;

            m_currentIndex = 0;
            m_currentInstrument = Instrument::Drum;

            Invalidate();
        });
    }

    Invalidate();
}

void InstrumentSelector::AddInstrument(Item* item)
{
    if (!item || item->GetInstrument() == Instrument::None)
        return;

    const auto key = item->GetInstrument();
    if (const auto it = m_items.find(key); it == m_items.end())
        m_items[key] = std::vector<Item*>();

    for (const auto i : m_items[key])
    {
        if (i->GetID() == item->GetID())
            return;
    }

    m_items[key].push_back(item);
    Gx::RadioButton* button = nullptr;
    switch (key)
    {
        case Instrument::Guitar:   button = FindChild<Gx::RadioButton>("IDC_RADIO_GUITAR"); break;
        case Instrument::Bass:     button = FindChild<Gx::RadioButton>("IDC_RADIO_BASS"); break;
        case Instrument::Keyboard: button = FindChild<Gx::RadioButton>("IDC_RADIO_KEYBOARD"); break;
        case Instrument::Drum:     button = FindChild<Gx::RadioButton>("IDC_RADIO_DRUM"); break;
        default: break;
    }

    if (button)
    {
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

Item* InstrumentSelector::GetInstrument() const
{
    return m_currentItem;
}

void InstrumentSelector::SetInstrument(int itemID)
{
    for (auto& [_, items] : m_items)
    {
        m_currentIndex = 0;
        for (const auto item : items)
        {
            m_currentIndex++;
            if (item->GetID() == itemID)
            {
                m_currentInstrument = item->GetInstrument();
                if (m_currentInstrument != Instrument::None)
                    m_currentItem = item;

                return;
            }
        }
    }

    m_currentIndex = 0;
    Invalidate();
}

void InstrumentSelector::SetInstrumentSelectCallack(const std::function<void(Item *)>& callback)
{
    if (const auto selectButton = FindChild<Gx::Button>("IDC_BUTTON_INSTRUMENT_SELECT"); selectButton)
        selectButton->SetClickCallback([this, callback] (auto& sender, auto& ev) { callback(m_currentItem); });
}

void InstrumentSelector::Invalidate()
{
    UiContainer::Invalidate();

    const auto items = m_items[m_currentInstrument];
    const auto instrumentThumbnail = FindChild<Gx::Image>("IDC_IMAGE_INSTRUMENT");
    if (!instrumentThumbnail)
        return;

    const auto instrumentLabel = FindChild<Gx::Label>("IDC_TEXT_INSTRUMENT_NAME");
    if (m_currentIndex >= static_cast<int>(items.size()))
        m_currentIndex = 0;

    if (m_currentIndex < 0)
        m_currentIndex = static_cast<int>(items.size()) - 1;

    if (m_currentIndex >= items.size())
    {
        m_currentItem = nullptr;
        instrumentThumbnail->SetVisible(false);
        if (instrumentLabel)
            instrumentLabel->SetString("(None)");

        return;
    }

    m_currentItem = items[m_currentIndex];
    if (const auto texture = m_currentItem->GetLargeThumbnail()->GetTexture(); texture)
    {
        instrumentThumbnail->SetVisible(true);
        instrumentThumbnail->SetTexture(*texture);
        instrumentThumbnail->SetTexCoords(m_currentItem->GetLargeThumbnail()->GetTexCoords());

        if (instrumentLabel)
            instrumentLabel->SetString(m_currentItem->GetName());
    }
}
