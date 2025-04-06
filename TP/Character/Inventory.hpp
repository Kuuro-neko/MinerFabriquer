// Inventory.hpp
#pragma once

#include "ItemStack.hpp"
#include <vector>
#include <string>

class Inventory {
public:
    void addItem(const ItemStack& itemStack) {
        for (auto& stack : items) {
            if (stack.getItemName() == itemStack.getItemName()) {
                stack.setQuantity(stack.getQuantity() + itemStack.getQuantity());
                return;
            }
        }
        items.push_back(itemStack);
        if (items.size() == 1) {
            selectedItem = 0;
        }
    }

    void removeItem(const std::string& itemType, int quantity) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->getItemName() == itemType) {
                if (it->getQuantity() > quantity) {
                    it->setQuantity(it->getQuantity() - quantity);
                } else {
                    items.erase(it);
                }
                return;
            }
        }
    }

    ItemStack* getSelectedItem() {
        return (selectedItem >= 0 && selectedItem < items.size()) ? &items[selectedItem] : nullptr;
    }

    void scrollSelectedItem(int direction) {
        if (items.empty()) return;

        selectedItem -= direction;
        if (selectedItem < 0) {
            selectedItem = items.size() - 1;
        } else if (selectedItem >= items.size()) {
            selectedItem = 0;
        }
    }

    const std::vector<ItemStack>& getItems() const { return items; }


    const void printInventory() const {
        std::cout << "Inventory:" << std::endl;
        for (int i = 0; i < items.size(); ++i) {
            if (i == selectedItem) {
                std::cout << "-> ";
            } else {
                std::cout << "   ";
            }
            std::cout << "Item " << i << ": " << items[i].getItemName() << " (ID: " << items[i].getItemId() << ", Quantity: " << items[i].getQuantity() << ")" << std::endl;
        }
    }

private:
    std::vector<ItemStack> items;
    int selectedItem = -1;
};