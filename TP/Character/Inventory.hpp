// Inventory.hpp
#pragma once

#include "ItemStack.hpp"
#include <vector>
#include <string>

class Inventory {
public:
    void addItem(const ItemStack& itemStack) {
        for (auto& stack : items) {
            if (stack.getItemType() == itemStack.getItemType()) {
                stack.setQuantity(stack.getQuantity() + itemStack.getQuantity());
                return;
            }
        }
        items.push_back(itemStack);
    }

    void removeItem(const std::string& itemType, int quantity) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->getItemType() == itemType) {
                if (it->getQuantity() > quantity) {
                    it->setQuantity(it->getQuantity() - quantity);
                } else {
                    items.erase(it);
                }
                return;
            }
        }
    }

    const std::vector<ItemStack>& getItems() const { return items; }


    const void printInventory() const {
        for (const auto& item : items) {
            std::cout << "Item: " << item.getItemType() << ", Quantity: " << item.getQuantity() << std::endl;
        }
    }

private:
    std::vector<ItemStack> items;
};