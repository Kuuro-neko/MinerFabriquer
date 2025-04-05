// ItemStack.hpp
#pragma once

#include <string>

class ItemStack {
public:
    ItemStack(const std::string& itemType, int quantity)
            : itemType(itemType), quantity(quantity) {}

    inline const std::string& getItemType() const { return itemType; }
    inline int getQuantity() const { return quantity; }
    inline void setQuantity(int newQuantity) { quantity = newQuantity; }

private:
    std::string itemType;
    int quantity;
};