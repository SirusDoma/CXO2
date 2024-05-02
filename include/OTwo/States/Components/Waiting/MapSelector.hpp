#ifndef O2JAM_WAITING_MAP_SELECTOR_HPP
#define O2JAM_WAITING_MAP_SELECTOR_HPP

#include <Genode/UI/UiContainer.hpp>

class MapSelector : public Gx::UiContainer
{
public:
    MapSelector() = default;
    explicit MapSelector(Gx::UiContainer &&copy) noexcept;

    void Initialize() override;

    unsigned int GetMapID() const;
    unsigned int GetEffectID() const;

    void SetMapID(int mapID);
    void SetEffectID(unsigned int effectID);

    void SetMapChangedCallback(const std::function<void(unsigned int)> &callback);
    void SetEffectChangedCallback(const std::function<void(unsigned int)> &callback);

private:
    unsigned int m_mapID;
    unsigned int m_effectID;

    std::function<void(unsigned int)> m_mapCallback;
    std::function<void(unsigned int)> m_effectCallback;
};

#endif