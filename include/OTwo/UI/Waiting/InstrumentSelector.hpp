#ifndef O2JAM_WAITING_INSTRUMENT_SELECTOR_HPP
#define O2JAM_WAITING_INSTRUMENT_SELECTOR_HPP

#include <OTwo/Avatar/Item.hpp>

#include <Genode/UI/UiContainer.hpp>

#include <unordered_map>

class  InstrumentSelector : public Gx::UiContainer
{
public:
    InstrumentSelector() = default;
    explicit InstrumentSelector(Gx::UiContainer &&copy) noexcept;

    void Initialize() override;

    void AddInstrument(Item *item);

    Item *GetInstrument() const;
    void SetInstrument(int itemID);

    void SetInstrumentSelectCallack(const std::function<void(Item*)>& callback);

    void Invalidate() override;

private:
    Item *m_currentItem;
    Instrument m_currentInstrument;
    int m_currentIndex;
    std::unordered_map<Instrument, std::vector<Item*>> m_items;
};

#endif