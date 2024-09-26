#ifndef O2JAM_CART_CONTEXT_HPP
#define O2JAM_CART_CONTEXT_HPP

#include <OTwo/IO/Serializable.hpp>
#include <OTwo/Serializable/Models.g.hpp>

class CartContext : public Serializable<Mx::Cart>
{
public:
    CartContext() = default;

    bool AddEquipment(unsigned int id);
    bool AddMusic(unsigned int id);
    bool AddEquipmentSet(unsigned int id);

    bool Remove(std::size_t index);

    bool Contains(Mx::CartItemType type, unsigned int id);
    const std::vector<Mx::CartItem>& GetItems() const;
};

#endif
