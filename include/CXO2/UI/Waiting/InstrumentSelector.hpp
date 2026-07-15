#pragma once

#include <CXO2/Metadata/Avatar/ItemMetadata.hpp>
#include <Genode/UI/UiContainer.hpp>

#include <unordered_map>
#include <CXO2/Avatar/ItemFactory.hpp>

namespace Gx
{
    class AudioMixer;
    class RadioButton;
    class ResourceManager;
}

namespace Cx
{

    class ItemFactory;
    class InstrumentSelector : public Gx::UiContainer
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
        void SelectInstrument(Gx::RadioButton& sender, Instrument instrument);

        void OnInstrumentLeftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnInstrumentRightButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnGuitarCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev);
        void OnBassCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev);
        void OnDrumCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev);
        void OnKeyboardCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev);
        void OnInstrumentSelectButtonClicked(Gx::Control& sender, Gx::Control::Event& ev) const;

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
