#pragma once

#include <CXO2/Metadata/Avatar/ItemMetadata.hpp>
#include <CXO2/UI/UiContainer.hpp>

#include <unordered_map>
#include <CXO2/Avatar/ItemFactory.hpp>

namespace Gx
{
    class AudioMixer;
    class ResourceManager;
}

namespace Cx
{
    class RadioButton;

    class ItemFactory;
    class InstrumentSelector : public Cx::UiContainer
    {
    public:
        InstrumentSelector(Gx::AudioMixer& mixer, Gx::ResourceManager& resources, ItemFactory& items);

        void Initialize() override;

        void AddInstrumentMetadata(const ItemMetadata& item);

        const ItemMetadata& GetInstrumentMetadata() const;
        void SetSelectedInstrument(int itemID);

        void SetInstrumentSelectCallback(const std::function<void(const ItemMetadata&)>& callback) const;

        void Invalidate() override;

    private:
        void SelectInstrument(Cx::RadioButton& sender, Instrument instrument);

        void OnInstrumentLeftButtonClicked(Control& sender, Control::Event& ev);
        void OnInstrumentRightButtonClicked(Control& sender, Control::Event& ev);
        void OnGuitarCheckChanged(Cx::RadioButton& sender, Control::Event& ev);
        void OnBassCheckChanged(Cx::RadioButton& sender, Control::Event& ev);
        void OnDrumCheckChanged(Cx::RadioButton& sender, Control::Event& ev);
        void OnKeyboardCheckChanged(Cx::RadioButton& sender, Control::Event& ev);
        void OnInstrumentSelectButtonClicked(Control& sender, Control::Event& ev) const;

        Gx::AudioMixer& m_mixer;
        Gx::ResourceManager& m_resources;
        ItemFactory& m_items;

        Item m_currentItem;
        ItemMetadata m_currentItemHeader;
        Instrument m_currentInstrument;
        int m_currentIndex;
        std::unordered_map<Instrument, std::vector<ItemMetadata>> m_headers;
        mutable std::function<void(const ItemMetadata&)> m_selectCallback;
    };
}
