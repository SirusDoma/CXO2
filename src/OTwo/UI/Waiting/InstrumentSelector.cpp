#include <OTwo/UI/Waiting/InstrumentSelector.hpp>

#include <Genode/System/Application.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/Image.hpp>

InstrumentSelector::InstrumentSelector(Gx::UiContainer &&copy) noexcept :
    Gx::UiContainer(copy),
    Gx::Node(copy),
    m_currentItem(),
    m_currentIndex(0),
    m_currentInstrument(Instrument::None),
    m_items()
{
}

void InstrumentSelector::Initialize()
{
    Node::Initialize();

    auto& app        = Gx::Application::Instance();
    auto& resources  = app.Require<Gx::ResourceManager>();
    auto& mixer      = app.Require<Gx::Mixer>();

    auto sfxNavigate = &resources.AddFromFile<sf::Sound>("Interface/Sound/Effect/07.json");
    if (auto previousButton = FindChild<Gx::Button>("IDC_BUTTON_INSTRUMENT_LEFT"); previousButton)
    {
        previousButton->SetClickCallback([this, &mixer, sound = sfxNavigate] (auto &sender, auto &ev)
        {
            mixer.Play(sound);
            if (m_currentInstrument == Instrument::None)
                return;

            m_currentIndex--;
            Invalidate();
        });
    }

    if (auto nextButton = FindChild<Gx::Button>("IDC_BUTTON_INSTRUMENT_RIGHT"); nextButton)
    {
        nextButton->SetClickCallback([this, &mixer, sound = sfxNavigate] (auto &sender, auto &ev)
        {
            mixer.Play(sound);
            if (m_currentInstrument == Instrument::None)
                return;

            m_currentIndex++;
            Invalidate();
        });
    }

    if (auto guitar = FindChild<Gx::RadioButton>("IDC_RADIO_GUITAR"); guitar)
    {
        guitar->SetEnabled(false);
        guitar->SetCheckStateChangeCallback([this] (auto sender)
        {
            if (!sender->IsChecked() || m_currentInstrument == Instrument::Guitar)
                return;

            m_currentIndex = 0;
            m_currentInstrument = Instrument::Guitar;
            Invalidate();
        });
    }

    if (auto bass = FindChild<Gx::RadioButton>("IDC_RADIO_BASS"); bass)
    {
        bass->SetEnabled(false);
        bass->SetCheckStateChangeCallback([this] (auto sender)
        {
            if (!sender->IsChecked() || m_currentInstrument == Instrument::Bass)
                return;

            m_currentIndex = 0;
            m_currentInstrument = Instrument::Bass;
            Invalidate();
        });
    }

    if (auto keyboard = FindChild<Gx::RadioButton>("IDC_RADIO_KEYBOARD"); keyboard)
    {
        keyboard->SetEnabled(false);
        keyboard->SetCheckStateChangeCallback([this] (auto sender)
        {
            if (!sender->IsChecked() || m_currentInstrument == Instrument::Keyboard)
                return;

            m_currentIndex = 0;
            m_currentInstrument = Instrument::Keyboard;
            Invalidate();
        });
    }

    if (auto drum = FindChild<Gx::RadioButton>("IDC_RADIO_DRUM"); drum)
    {
        drum->SetEnabled(false);
        drum->SetCheckStateChangeCallback([this] (auto sender)
        {
            if (!sender->IsChecked() || m_currentInstrument == Instrument::Drum)
                return;

            m_currentIndex = 0;
            m_currentInstrument = Instrument::Drum;

            Invalidate();
        });
    }

    Invalidate();
}

void InstrumentSelector::AddInstrument(Item *item)
{
    if (!item || item->GetInstrument() == Instrument::None)
        return;

    auto key = item->GetInstrument();
    if (auto it = m_items.find(key); it == m_items.end())
        m_items[key] = std::vector<Item*>();

    for (auto i : m_items[key])
    {
        if (i->GetID() == item->GetID())
            return;
    }

    m_items[key].push_back(item);
    Gx::RadioButton *button = nullptr;
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
        for (auto child : GetChildren())
        {
            if (auto radio = dynamic_cast<Gx::RadioButton*>(child); radio && radio->IsChecked())
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

Item *InstrumentSelector::GetInstrument() const
{
    return m_currentItem;
}

void InstrumentSelector::SetInstrument(int itemID)
{
    for (auto& [_, items] : m_items)
    {
        m_currentIndex = 0;
        for (auto item : items)
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
    if (auto selectButton = FindChild<Gx::Button>("IDC_BUTTON_INSTRUMENT_SELECT"); selectButton)
        selectButton->SetClickCallback([this, callback] (auto &sender, auto &ev) { callback(m_currentItem); });
}

void InstrumentSelector::Invalidate()
{
    UiContainer::Invalidate();

    auto items = m_items[m_currentInstrument];
    auto instrumentPreview = FindChild<Gx::Image>("IDC_IMAGE_INSTRUMENT");
    if (!instrumentPreview)
        return;

    auto instrumentLabel = FindChild<Gx::Label>("IDC_TEXT_INSTRUMENT_NAME");
    if (m_currentIndex >= static_cast<int>(items.size()))
        m_currentIndex = 0;

    if (m_currentIndex < 0)
        m_currentIndex = static_cast<int>(items.size()) - 1;

    if (m_currentIndex >= items.size())
    {
        m_currentItem = nullptr;
        instrumentPreview->SetVisible(false);
        if (instrumentLabel)
            instrumentLabel->SetString("(None)");

        return;
    }

    m_currentItem = items[m_currentIndex];
    if (auto texture = m_currentItem->GetLargePreview()->GetTexture(); texture)
    {
        instrumentPreview->SetVisible(true);
        instrumentPreview->SetTexture(*texture);
        instrumentPreview->SetTexCoords(m_currentItem->GetLargePreview()->GetTexCoords());

        if (instrumentLabel)
            instrumentLabel->SetString(m_currentItem->GetName());
    }
}
