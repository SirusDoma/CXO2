#pragma once

#include <OTwo/Metadata/Avatar/ItemMetadata.hpp>
#include <Genode/UI/UiContainer.hpp>

#include <unordered_map>
#include <OTwo/Avatar/ItemFactory.hpp>

namespace Gx
{
    class AudioMixer;
    class ResourceManager;
}

class ItemFactory;
class InstrumentSelector : public Gx::UiContainer
{
public:
    InstrumentSelector(Gx::AudioMixer& mixer, Gx::ResourceManager& resources, ItemFactory& items);

    void Initialize() override;

    void AddInstrumentMetadata(const ItemMetadata& item);

    const ItemMetadata& GetInstrumentMetadata() const;
    void SetSelectedInstrument(int itemID);

    void SetInstrumentSelectCallack(const std::function<void(const ItemMetadata&)>& callback);

    void Invalidate() override;

private:
    Gx::AudioMixer& m_mixer;
    Gx::ResourceManager& m_resources;
    ItemFactory& m_items;

    Item m_currentItem;
    ItemMetadata m_currentItemHeader;
    Instrument m_currentInstrument;
    int m_currentIndex;
    std::unordered_map<Instrument, std::vector<ItemMetadata>> m_headers;
};
